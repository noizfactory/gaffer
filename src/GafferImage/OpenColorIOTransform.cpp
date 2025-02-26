//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include "GafferImage/OpenColorIOTransform.h"

#include "GafferImage/OpenColorIOAlgo.h"

#include "Gaffer/Context.h"
#include "Gaffer/Process.h"

#include "IECore/SimpleTypedData.h"

using namespace std;
using namespace IECore;
using namespace Gaffer;
using namespace GafferImage;

namespace
{

struct ProcessorProcess : public Process
{

	public :

		ProcessorProcess( InternedString type, const OpenColorIOTransform *node )
			:	Process( type, node->outPlug() )
		{
		}

		static InternedString processorProcessType;
		static InternedString processorHashProcessType;

};

InternedString ProcessorProcess::processorProcessType( "openColorIOTransform:processor" );
InternedString ProcessorProcess::processorHashProcessType( "openColorIOTransform:processorHash" );

} // namespace

GAFFER_NODE_DEFINE_TYPE( OpenColorIOTransform );

size_t OpenColorIOTransform::g_firstPlugIndex = 0;

OpenColorIOTransform::OpenColorIOTransform( const std::string &name , bool withContextPlug )
	:	ColorProcessor( name ), m_hasContextPlug( withContextPlug )
{
	storeIndexOfNextChild( g_firstPlugIndex );
	if( m_hasContextPlug )
	{
		addChild( new CompoundDataPlug( "context" ) );
	}
}

OpenColorIOTransform::~OpenColorIOTransform()
{
}

Gaffer::CompoundDataPlug *OpenColorIOTransform::contextPlug()
{
	if( !m_hasContextPlug )
	{
		return nullptr;
	}
	return getChild<CompoundDataPlug>( g_firstPlugIndex );
}

const Gaffer::CompoundDataPlug *OpenColorIOTransform::contextPlug() const
{
	if( !m_hasContextPlug )
	{
		return nullptr;
	}
	return getChild<CompoundDataPlug>( g_firstPlugIndex );
}

OCIO_NAMESPACE::ConstProcessorRcPtr OpenColorIOTransform::processor() const
{
	// Process is necessary to trigger substitutions for plugs
	// pulled on by `transform()` and `ocioContext()`.
	ProcessorProcess process( ProcessorProcess::processorProcessType, this );

	OCIO_NAMESPACE::ConstTransformRcPtr colorTransform = transform();
	if( !colorTransform )
	{
		return OCIO_NAMESPACE::ConstProcessorRcPtr();
	}

	auto [config, context] = OpenColorIOAlgo::currentConfigAndContext();
	context = modifiedOCIOContext( context );
	return config->getProcessor( context, colorTransform, OCIO_NAMESPACE::TRANSFORM_DIR_FORWARD );
}

IECore::MurmurHash OpenColorIOTransform::processorHash() const
{
	// Process is necessary to trigger substitutions for plugs
	// that may be pulled on by `hashTransform()`.
	ProcessorProcess process( ProcessorProcess::processorHashProcessType, this );

	IECore::MurmurHash result;
	hashTransform( Context::current(), result );

	result.append( OpenColorIOAlgo::currentConfigAndContextHash() );

	if( auto *p = contextPlug() )
	{
		p->hash( result );
	}
	return result;
}

bool OpenColorIOTransform::affectsColorProcessor( const Gaffer::Plug *input ) const
{
	if( contextPlug() && contextPlug()->isAncestorOf( input ) )
	{
		return true;
	}
	return affectsTransform( input );
}

void OpenColorIOTransform::hashColorProcessor( const Gaffer::Context *context, IECore::MurmurHash &h ) const
{
	h.append( processorHash() );
}

OCIO_NAMESPACE::ConstContextRcPtr OpenColorIOTransform::modifiedOCIOContext( OCIO_NAMESPACE::ConstContextRcPtr context ) const
{

	const CompoundDataPlug *p = contextPlug();
	if( !p )
	{
		return context;
	}

	if( !p->children().size() )
	{
		return context;
	}

	OCIO_NAMESPACE::ContextRcPtr mutableContext;
	std::string name;
	std::string value;

	for( NameValuePlug::Iterator it( p ); !it.done(); ++it )
	{
		IECore::DataPtr d = p->memberDataAndName( it->get(), name );
		if( d )
		{
			StringDataPtr data = runTimeCast<StringData>( d );
			if( !data )
			{
				throw( Exception(  "OpenColorIOTransform: Failed to convert context value to string." ) );
			}
			value = data->readable();
			if( !name.empty() && !value.empty() )
			{
				if( !mutableContext )
				{
					mutableContext = context->createEditableCopy();
				}
				mutableContext->setStringVar(name.c_str(), value.c_str() );
			}
		}
	}

	return mutableContext ? mutableContext : context;
}

ColorProcessor::ColorProcessorFunction OpenColorIOTransform::colorProcessor( const Gaffer::Context *context ) const
{
	OCIO_NAMESPACE::ConstProcessorRcPtr processor = this->processor();
	if( !processor || processor->isNoOp() )
	{
		return ColorProcessorFunction();
	}

	OCIO_NAMESPACE::ConstCPUProcessorRcPtr cpuProcessor = processor->getDefaultCPUProcessor();

	return [cpuProcessor] ( IECore::FloatVectorData *r, IECore::FloatVectorData *g, IECore::FloatVectorData *b ) {

		if( !r->readable().size() )
		{
			// Deep image with no samples. OCIO will throw if we give it an empty
			// PlanarImageDesc.
			return;
		}

		OCIO_NAMESPACE::PlanarImageDesc image(
			r->baseWritable(),
			g->baseWritable(),
			b->baseWritable(),
			nullptr, // alpha
			r->readable().size(), // Treat all pixels as a single line, since geometry doesn't affect OCIO
			1 // height
		);

		cpuProcessor->apply( image );
	};
}
