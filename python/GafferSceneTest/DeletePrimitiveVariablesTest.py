##########################################################################
#
#  Copyright (c) 2012, John Haddon. All rights reserved.
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
import IECoreScene

import Gaffer
import GafferTest
import GafferScene
import GafferSceneTest

class DeletePrimitiveVariablesTest( GafferSceneTest.SceneTestCase ) :

	def test( self ) :

		p = GafferScene.Plane()
		d = GafferScene.DeletePrimitiveVariables()
		d["in"].setInput( p["out"] )

		self.assertEqual( p["out"].object( "/plane" ), d["out"].object( "/plane" ) )
		self.assertSceneHashesEqual( p["out"], d["out"] )
		self.assertIn( "uv", d["out"].object( "/plane" ) )

		d["names"].setValue( "uv e" )

		self.assertNotEqual( p["out"].object( "/plane" ), d["out"].object( "/plane" ) )
		self.assertSceneHashesEqual( p["out"], d["out"], checks = self.allSceneChecks - { "object" } )
		self.assertSceneHashesEqual( p["out"], d["out"], pathsToPrune = ( "/plane" ) )
		self.assertNotIn( "uv", d["out"].object( "/plane" ) )
		self.assertIn( "P", d["out"].object( "/plane" ) )

	def testNonPrimitiveObject( self ) :

		c = GafferScene.Camera()

		d = GafferScene.DeletePrimitiveVariables()
		d["in"].setInput( c["out"] )

		self.assertSceneValid( d["out"] )
		self.assertIsInstance( d["out"].object( "/camera" ), IECoreScene.Camera )

	def testAffects( self ) :

		d = GafferScene.DeletePrimitiveVariables()

		cs = GafferTest.CapturingSlot( d.plugDirtiedSignal() )
		self.assertEqual( len( cs ), 0 )

		d["enabled"].setValue( False )
		self.assertTrue( "out" in [ x[0].getName() for x in cs ] )

	def testWildcards( self ) :

		p = GafferScene.Plane()
		d = GafferScene.DeletePrimitiveVariables()
		d["in"].setInput( p["out"] )

		d["names"].setValue( "*" )
		self.assertEqual( d["out"].object( "/plane" ).keys(), [] )

if __name__ == "__main__":
	unittest.main()
