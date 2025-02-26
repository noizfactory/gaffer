//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018, Image Engine Design Inc. All rights reserved.
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

#include "GafferUI/StandardNodule.h"

namespace GafferUI
{

class NoduleLayout;

/// A Nodule subclass to represent CompoundNumericPlugs
/// so that connections can optionally be made to the
/// child plugs.
class GAFFERUI_API CompoundNumericNodule : public StandardNodule
{

	public :

		explicit CompoundNumericNodule( Gaffer::PlugPtr plug );
		~CompoundNumericNodule() override;

		GAFFER_GRAPHCOMPONENT_DECLARE_TYPE( GafferUI::CompoundNumericNodule, CompoundNumericNoduleTypeId, StandardNodule );

		Nodule *nodule( const Gaffer::Plug *plug ) override;
		const Nodule *nodule( const Gaffer::Plug *plug ) const override;

		bool canCreateConnection( const Gaffer::Plug *endpoint ) const override;
		void createConnection( Gaffer::Plug *endpoint ) override;

		Imath::Box3f bound() const override;

	protected :

		void renderLayer( Layer layer, const Style *style, RenderReason reason ) const override;
		unsigned layerMask() const override;
		Imath::Box3f renderBound() const override;

	private :

		NoduleLayout *noduleLayout();
		const NoduleLayout *noduleLayout() const;

		void plugMetadataChanged( const Gaffer::Plug *plug, IECore::InternedString key );
		void updateChildNoduleVisibility();

		static NoduleTypeDescription<CompoundNumericNodule> g_noduleTypeDescription;

};

IE_CORE_DECLAREPTR( CompoundNumericNodule );

} // namespace GafferUI
