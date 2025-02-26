//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2022, Cinesite VFX Ltd. All rights reserved.
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

#include "GafferScene/FilteredSceneProcessor.h"

#include "Gaffer/StringPlug.h"

namespace GafferScene
{

class GAFFERSCENE_API Rename : public FilteredSceneProcessor
{

	public :

		explicit Rename( const std::string &name=defaultName<Rename>() );
		~Rename() override;

		GAFFER_NODE_DECLARE_TYPE( GafferScene::Rename, RenameTypeId, FilteredSceneProcessor );

		Gaffer::StringPlug *namePlug();
		const Gaffer::StringPlug *namePlug() const;

		Gaffer::StringPlug *deletePrefixPlug();
		const Gaffer::StringPlug *deletePrefixPlug() const;

		Gaffer::StringPlug *deleteSuffixPlug();
		const Gaffer::StringPlug *deleteSuffixPlug() const;

		Gaffer::StringPlug *findPlug();
		const Gaffer::StringPlug *findPlug() const;

		Gaffer::StringPlug *replacePlug();
		const Gaffer::StringPlug *replacePlug() const;

		Gaffer::BoolPlug *useRegularExpressionsPlug();
		const Gaffer::BoolPlug *useRegularExpressionsPlug() const;

		Gaffer::StringPlug *addPrefixPlug();
		const Gaffer::StringPlug *addPrefixPlug() const;

		Gaffer::StringPlug *addSuffixPlug();
		const Gaffer::StringPlug *addSuffixPlug() const;

		void affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const override;

	protected :

		void hash( const Gaffer::ValuePlug *output, const Gaffer::Context *context, IECore::MurmurHash &h ) const override;
		void compute( Gaffer::ValuePlug *output, const Gaffer::Context *context ) const override;

		Gaffer::ValuePlug::CachePolicy hashCachePolicy( const Gaffer::ValuePlug *output ) const override;
		Gaffer::ValuePlug::CachePolicy computeCachePolicy( const Gaffer::ValuePlug *output ) const override;

		void hashBound( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		Imath::Box3f computeBound( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashTransform( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		Imath::M44f computeTransform( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashAttributes( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		IECore::ConstCompoundObjectPtr computeAttributes( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashObject( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		IECore::ConstObjectPtr computeObject( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashChildNames( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		IECore::ConstInternedStringVectorDataPtr computeChildNames( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const override;

		void hashSet( const IECore::InternedString &setName, const Gaffer::Context *context, const ScenePlug *parent, IECore::MurmurHash &h ) const override;
		IECore::ConstPathMatcherDataPtr computeSet( const IECore::InternedString &setName, const Gaffer::Context *context, const ScenePlug *parent ) const override;

	private :

		// Used to compute a per-location map of renamed children. Must be evaluated
		// in a context suitable for the _input_ scene.
		const Gaffer::ObjectPlug *nameMapPlug() const;
		// Used to compute the input location for a particular output location.
		// Must be evaluated in a context suitable for the _output_ scene.
		const Gaffer::InternedStringVectorDataPlug *inputPathPlug() const;

		bool affectsOutputName( const Gaffer::Plug *input ) const;
		void hashOutputName( IECore::MurmurHash &h ) const;
		std::string outputName( IECore::InternedString inputName ) const;

		bool affectsNameMap( const Gaffer::Plug *input ) const;
		void hashNameMap( const Gaffer::Context *context, IECore::MurmurHash &h ) const;
		IECore::ConstObjectPtr computeNameMap( const Gaffer::Context *context ) const;

		bool affectsInputPath( const Gaffer::Plug *input ) const;
		void hashInputPath( const Gaffer::Context *context, IECore::MurmurHash &h ) const;
		IECore::ConstInternedStringVectorDataPtr computeInputPath( const Gaffer::Context *context ) const;

		struct InputScope;

		static size_t g_firstPlugIndex;

};

IE_CORE_DECLAREPTR( Rename )

} // namespace GafferScene
