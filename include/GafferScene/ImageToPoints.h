//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2023, Cinesite VFX Ltd. All rights reserved.
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

#pragma once

#include "GafferScene/ObjectSource.h"

#include "GafferImage/ImagePlug.h"

namespace GafferScene
{

class GAFFERSCENE_API ImageToPoints : public ObjectSource
{

	public :

		GAFFER_NODE_DECLARE_TYPE( GafferScene::ImageToPoints, ImageToPointsTypeId, ObjectSource );

		explicit ImageToPoints( const std::string &name=defaultName<ImageToPoints>() );
		~ImageToPoints() override;

		GafferImage::ImagePlug *imagePlug();
		const GafferImage::ImagePlug *imagePlug() const;

		Gaffer::StringPlug *viewPlug();
		const Gaffer::StringPlug *viewPlug() const;

		Gaffer::StringVectorDataPlug *positionPlug();
		const Gaffer::StringVectorDataPlug *positionPlug() const;

		Gaffer::StringPlug *primitiveVariablesPlug();
		const Gaffer::StringPlug *primitiveVariablesPlug() const;

		Gaffer::FloatPlug *widthPlug();
		const Gaffer::FloatPlug *widthPlug() const;

		Gaffer::StringPlug *widthChannelPlug();
		const Gaffer::StringPlug *widthChannelPlug() const;

		Gaffer::BoolPlug *ignoreTransparentPlug();
		const Gaffer::BoolPlug *ignoreTransparentPlug() const;

		Gaffer::FloatPlug *alphaThresholdPlug();
		const Gaffer::FloatPlug *alphaThresholdPlug() const;

		void affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const override;

	protected :

		void hashSource( const Gaffer::Context *context, IECore::MurmurHash &h ) const override;
		IECore::ConstObjectPtr computeSource( const Gaffer::Context *context ) const override;

	private :

		struct Destination
		{
			std::string name;
			IECore::TypeId type;
			size_t offset;
			float *data = nullptr;
		};

		struct ChannelMapping
		{
			std::string name;
			std::vector<Destination> destinations;
		};

		std::vector<ChannelMapping> channelMappings() const;

		static size_t g_firstPlugIndex;

};

IE_CORE_DECLAREPTR( ImageToPoints )

} // namespace GafferScene
