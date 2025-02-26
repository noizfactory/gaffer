//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2019, Image Engine Design Inc. All rights reserved.
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
#include "GafferScene/Shader.h"
#include "GafferScene/ShaderPlug.h"

namespace GafferSceneModule
{

	// Forward declaration to enable friend declaration
	class LightFilterSerialiser;

} // namespace GafferModule

namespace GafferScene
{

class GAFFERSCENE_API LightFilter : public ObjectSource
{

	public :

		GAFFER_NODE_DECLARE_TYPE( GafferScene::LightFilter, LightFilterTypeId, ObjectSource );

		~LightFilter() override;

		void loadShader( const std::string &shaderName, bool keepExistingValues=false );

		Gaffer::StringPlug *filteredLightsPlug();
		const Gaffer::StringPlug *filteredLightsPlug() const;

		Gaffer::Plug *parametersPlug();
		const Gaffer::Plug *parametersPlug() const;

		void affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const override;

	protected :

		explicit LightFilter( GafferScene::ShaderPtr shader, const std::string &name=defaultName<LightFilter>() );

		void hashSource( const Gaffer::Context *context, IECore::MurmurHash &h ) const override;
		IECore::ConstObjectPtr computeSource( const Gaffer::Context *context ) const override;

		void hashAttributes( const SceneNode::ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		IECore::ConstCompoundObjectPtr computeAttributes( const SceneNode::ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashBound( const SceneNode::ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		Imath::Box3f computeBound( const SceneNode::ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashStandardSetNames( const Gaffer::Context *context, IECore::MurmurHash &h ) const override;
		IECore::ConstInternedStringVectorDataPtr computeStandardSetNames() const override;

	private :

		friend class GafferSceneModule::LightFilterSerialiser;

		GafferScene::ShaderPlug *shaderPlug();
		const GafferScene::ShaderPlug *shaderPlug() const;

		GafferScene::Shader *shaderNode();
		const GafferScene::Shader *shaderNode() const;

		static size_t g_firstPlugIndex;

};

IE_CORE_DECLAREPTR( LightFilter )

} // namespace GafferScene
