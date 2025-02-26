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

#include "GafferScene/Export.h"

#include "IECore/PathMatcher.h"

using namespace IECore;

namespace GafferScene
{

/// Defines a subset of the scene hierarchy to be rendered.
/// A location will be rendered if _either_ of the following is true :
///
/// 1. All its ancestors appear in `expansions`. This maps neatly to "tree view" style navigation
/// as provided by the HierarchyView.
/// 2. At least one of its ancestors appears in `inclusions`. This allows entire subtrees of the
/// scene to be included concisely, without them cluttering the `expansions` (and therefore the HierarchyView).
///
/// Regardless of all the above, a location will _never_ be rendered if it - or any ancestor -
/// appears in `exclusions`. This allows expensive or irrelevant portions of the scene to be ignored,
/// regardless of any other setting.
struct GAFFERSCENE_API VisibleSet
{

	PathMatcher expansions;
	PathMatcher inclusions;
	PathMatcher exclusions;

	struct Visibility
	{
		/// The draw mode of a location as defined by the VisibleSet.
		enum DrawMode
		{
			/// The location is not visible.
			None = 0,
			/// The location is visible and will be drawn. If the location has descendants then their combined bounding
			/// box will also be drawn when `descendantsVisible` is false.
			Visible = 1,
			/// The location is visible, but only ever drawn as a bounding box as it is excluded from the VisibleSet.
			ExcludedBounds = 2,
		};

		Visibility( DrawMode drawMode = DrawMode::None, bool descendantsVisible = false )
			: drawMode( drawMode ), descendantsVisible( descendantsVisible )
		{
		}

		bool operator == ( const Visibility &rhs ) const
		{
			return drawMode == rhs.drawMode && descendantsVisible == rhs.descendantsVisible;
		}

		DrawMode drawMode;
		bool descendantsVisible;
	};

	/// Returns the Visibility of a path tested against the VisibleSet.
	Visibility visibility( const std::vector<InternedString> &path, const size_t minimumExpansionDepth = 0 ) const;

	bool operator == ( const VisibleSet &rhs ) const;
	bool operator != ( const VisibleSet &rhs ) const;

};

void murmurHashAppend( MurmurHash &h, const VisibleSet &data );

} // namespace GafferScene

#include "GafferScene/VisibleSet.inl"
