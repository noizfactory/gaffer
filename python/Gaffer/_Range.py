##########################################################################
#
#  Copyright (c) 2020, Cinesite VFX Ltd. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#
#      * Neither the name of Cinesite VFX Ltd. nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import functools

import Gaffer

# Rather than bind the C++ GraphComponent Range classes to
# Python, we just reimplement them in pure Python. This allows
# us to properly support filtering for subclasses defined in
# Python.

def __range( cls, parent ) :

	for child in parent.children() :
		if isinstance( child, cls ) :
			yield child

def __recursiveRange( cls, parent ) :

	for child in parent.children() :
		if isinstance( child, cls ) :
			yield child
		for r in __recursiveRange( cls, child ) :
			yield r

Gaffer.GraphComponent.Range = classmethod( __range )
Gaffer.GraphComponent.RecursiveRange = classmethod( __recursiveRange )

def __recursiveNodeRange( cls, parent ) :

	for i in range( 0, len( parent ) ) :
		child = parent[i]
		if isinstance( child, cls ) :
			yield child
		if isinstance( child, Gaffer.Node ) :
			for r in __recursiveNodeRange( cls, child ) :
				yield r

Gaffer.Node.RecursiveRange = classmethod( __recursiveNodeRange )

def __plugRange( cls, parent, direction ) :

	for i in range( 0, len( parent ) ) :
		child = parent[i]
		if isinstance( child, cls ) and child.direction() == direction :
			yield child

def __recursivePlugRange( cls, parent, direction = None ) :

	for i in range( 0, len( parent ) ) :
		child = parent[i]
		if isinstance( child, cls ) and ( direction is None or child.direction() == direction ):
			yield child
		if isinstance( child, Gaffer.Plug ) :
			for r in __recursivePlugRange( cls, child, direction ) :
				yield r

Gaffer.Plug.InputRange = classmethod( functools.partial( __plugRange, direction = Gaffer.Plug.Direction.In ) )
Gaffer.Plug.OutputRange = classmethod( functools.partial( __plugRange, direction = Gaffer.Plug.Direction.Out ) )
Gaffer.Plug.RecursiveRange = classmethod( __recursivePlugRange )
Gaffer.Plug.RecursiveInputRange = classmethod( functools.partial( __recursivePlugRange, direction = Gaffer.Plug.Direction.In ) )
Gaffer.Plug.RecursiveOutputRange = classmethod( functools.partial( __recursivePlugRange, direction = Gaffer.Plug.Direction.Out ) )
