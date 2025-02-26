//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018, Alex Fuller. All rights reserved.
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

#include "Gaffer/Plug.h"

// Cycles
IECORE_PUSH_DEFAULT_VISIBILITY
#include "graph/node.h"
IECORE_POP_DEFAULT_VISIBILITY
#undef fmix // OpenImageIO's farmhash inteferes with IECore::MurmurHash

namespace GafferCycles
{

namespace SocketHandler
{

/// A helper class for mapping Cycles Sockets to Gaffer Plugs.
Gaffer::Plug *setupPlug( const IECore::InternedString &socketName, int socketType, Gaffer::GraphComponent *plugParent, Gaffer::Plug::Direction direction = Gaffer::Plug::In );
Gaffer::Plug *setupPlug( const ccl::NodeType *nodeType, const ccl::SocketType socketType, Gaffer::GraphComponent *plugParent, Gaffer::Plug::Direction direction = Gaffer::Plug::In );
void setupPlugs( const ccl::NodeType *node, Gaffer::GraphComponent *plugsParent, Gaffer::Plug::Direction direction = Gaffer::Plug::In );
void setupLightPlugs( const std::string &shaderName, const ccl::NodeType *nodeType, Gaffer::GraphComponent *plugsParent, bool keepExistingChildren = false );

} // namespace SocketHandler

} // namespace GafferCycles
