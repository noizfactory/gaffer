##########################################################################
#
#  Copyright (c) 2015, Image Engine Design Inc. All rights reserved.
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
#      * Neither the name of John Haddon nor the names of
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

import unittest

import IECore

import Gaffer
import GafferUITest
import GafferImage
import GafferImageUI

class ImageGadgetTest( GafferUITest.TestCase ) :

	def testBound( self ) :

		g = GafferImageUI.ImageGadget()
		self.assertEqual( g.bound(), IECore.Box3f() )

		c = GafferImage.Constant()
		c["format"].setValue( GafferImage.Format( 200, 100 ) )

		g.setImage( c["out"] )
		self.assertEqual( g.bound(), IECore.Box3f( IECore.V3f( 0 ), IECore.V3f( 200, 100, 0) ) )

		c["format"].setValue( GafferImage.Format( 200, 100, 2 ) )
		self.assertEqual( g.bound(), IECore.Box3f( IECore.V3f( 0 ), IECore.V3f( 400, 100, 0) ) )

		c2 = GafferImage.Constant()
		g.setImage( c2["out"] )

		f = GafferImage.FormatPlug.getDefaultFormat( g.getContext() ).getDisplayWindow()
		self.assertEqual( g.bound(), IECore.Box3f( IECore.V3f( f.min.x, f.min.y, 0 ), IECore.V3f( f.max.x, f.max.y, 0 ) ) )

		GafferImage.FormatPlug.setDefaultFormat( g.getContext(), GafferImage.Format( IECore.Box2i( IECore.V2i( 10, 20 ), IECore.V2i( 30, 40 ) ) ) )
		self.assertEqual( g.bound(), IECore.Box3f( IECore.V3f( 10, 20, 0 ), IECore.V3f( 30, 40, 0 ) ) )

	def testGetImage( self ) :

		g = GafferImageUI.ImageGadget()
		self.assertEqual( g.getImage(), None )

		c = GafferImage.Constant()
		g.setImage( c["out"] )
		self.assertTrue( g.getImage().isSame( c["out"] ) )

if __name__ == "__main__":
	unittest.main()
