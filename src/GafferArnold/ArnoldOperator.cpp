//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2012, John Haddon. All rights reserved.
//  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
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

#define GAFFER_AI_TYPE_OPERATOR -1

#include "GafferArnold/ArnoldOperator.h"

#include "GafferArnold/ParameterHandler.h"

#include "GafferArnold/ArnoldOperatorPlug.h"

#include "Gaffer/CompoundNumericPlug.h"
#include "Gaffer/NumericPlug.h"
#include "Gaffer/StringPlug.h"

#include "IECoreArnold/UniverseBlock.h"

#include "IECore/LRUCache.h"
#include "IECore/MessageHandler.h"

#include "boost/format.hpp"

#include "ai.h"

using namespace std;
using namespace boost;
using namespace Imath;
using namespace IECore;
using namespace GafferScene;
using namespace GafferArnold;
using namespace Gaffer;

IE_CORE_DEFINERUNTIMETYPED( ArnoldOperator );

ArnoldOperator::ArnoldOperator( const std::string &name )
	:	GafferScene::Shader( name )
{
}

ArnoldOperator::~ArnoldOperator()
{
}

Gaffer::Plug *ArnoldOperator::correspondingInput( const Gaffer::Plug *output )
{
	// better to do a few harmless casts than manage a duplicate implementation
	return const_cast<Gaffer::Plug *>(
		const_cast<const ArnoldOperator *>( this )->correspondingInput( output )
	);
}

const Gaffer::Plug *ArnoldOperator::correspondingInput( const Gaffer::Plug *output ) const
{
	if( output != outPlug() )
	{
		return Shader::correspondingInput( output );
	}

	const CompoundData *metadata = ArnoldOperator::metadata();
	if( !metadata )
	{
		return nullptr;
	}

	const StringData *primaryInput = static_cast<const StringData*>( metadata->member<IECore::CompoundData>( "operator" )->member<IECore::Data>( "primaryInput" ) );
	if( !primaryInput )
	{
		return nullptr;
	}

	const Plug *result = parametersPlug()->getChild<Plug>( primaryInput->readable() );
	if( !result )
	{
		IECore::msg( IECore::Msg::Error, "ArnoldOperator::correspondingInput", boost::format( "Parameter \"%s\" does not exist" ) % primaryInput->readable() );
		return nullptr;
	}

	return result;
}

void ArnoldOperator::loadShader( const std::string &operatorName, bool keepExistingValues )
{
	IECoreArnold::UniverseBlock arnoldUniverse( /* writable = */ false );

	const AtNodeEntry *operator = AiNodeEntryLookUp( AtString( operatorName.c_str() ) );
	if( !operator )
	{
		throw Exception( str( format( "Operator \"%s\" not found" ) % operatorName ) );
	}

	if( !keepExistingValues )
	{
		parametersPlug()->clearChildren();
		if( Plug *out = outPlug() )
		{
			removeChild( out );
		}
	}

	const bool isLightShader = AiNodeEntryGetType( operator ) == AI_NODE_LIGHT;
	namePlug()->setValue( AiNodeEntryGetName( operator ) );

	int aiOutputType = GAFFER_AI_TYPE_OPERATOR;
	string type = "ai:light";
	if( !isLightShader )
	{
		const CompoundData *metadata = ArnoldOperator::metadata();
		const StringData *operatorTypeData = static_cast<const StringData*>( metadata->member<IECore::CompoundData>( "operator" )->member<IECore::Data>( "operatorType" ) );
		if( operatorTypeData )
		{
			type = "ai:" + operatorTypeData->readable();
		}
		else
		{
			type = "ai:operator";
		}

		if( type == "ai:operator" )
		{
			aiOutputType = AiNodeEntryGetOutputType( operator );
		}
	}

	if( !keepExistingValues && type == "ai:operator" )
	{
		attributeSuffixPlug()->setValue( operatorName );
	}

	typePlug()->setValue( type );

	ParameterHandler::setupPlugs( operator, parametersPlug() );
	ParameterHandler::setupPlug( "out", aiOutputType, this, Plug::Out );

}

//////////////////////////////////////////////////////////////////////////
// Metadata loading code
//////////////////////////////////////////////////////////////////////////

namespace {
	const AtString g_nullArnoldString( nullptr );
	const AtString g_primaryInputArnoldString( "primaryInput" );
	const AtString g_operatorTypeArnoldString( "operatorType" );
}

static IECore::ConstCompoundDataPtr metadataGetter( const std::string &key, size_t &cost )
{
	IECoreArnold::UniverseBlock arnoldUniverse( /* writable = */ false );

	const AtNodeEntry *operator = AiNodeEntryLookUp( AtString( key.c_str() ) );
	if( !operator )
	{
		throw Exception( str( format( "Operator \"%s\" not found" ) % key ) );
	}

	CompoundDataPtr metadata = new CompoundData;

	CompoundDataPtr operatorMetadata = new CompoundData;
	metadata->writable()["operator"] = operatorMetadata;

	// Currently we don't store metadata for parameters.
	// We add the "parameter" CompoundData mainly so that we are consistent with the OSLShader.
	// Eventually we will load all metadata here and access it from ArnoldOperatorUI.
	CompoundDataPtr parameterMetadata = new CompoundData;
	metadata->writable()["parameter"] = parameterMetadata;

	AtString value;
	if( AiMetaDataGetStr( operator, /* look up metadata on node, not on parameter */ g_nullArnoldString , g_primaryInputArnoldString, &value ) )
	{
		operatorMetadata->writable()["primaryInput"] = new StringData( value.c_str() );
	}

	AtString operatorType;
	if( AiMetaDataGetStr( operator, /* look up metadata on node, not on parameter */ g_nullArnoldString , g_operatorTypeArnoldString, &operatorType ) )
	{
		operatorMetadata->writable()["operatorType"] = new StringData( operatorType.c_str() );
	}

	return metadata;
}

typedef LRUCache<std::string, IECore::ConstCompoundDataPtr> MetadataCache;
MetadataCache g_arnoldMetadataCache( metadataGetter, 10000 );

const IECore::CompoundData *ArnoldOperator::metadata() const
{
	if( m_metadata )
	{
		return m_metadata.get();
	}

	m_metadata = g_arnoldMetadataCache.get( namePlug()->getValue() );
	return m_metadata.get();
}
