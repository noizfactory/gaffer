##########################################################################
#
#  Copyright (c) 2017, Image Engine Design Inc. All rights reserved.
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

import IECore

import Gaffer
import GafferTest
import GafferUITest
import GafferScene
import GafferSceneUI

class ContextAlgoTest( GafferUITest.TestCase ) :

	def testExpandedPaths( self ) :

		# A
		# |__B
		#    |__D
		#    |__E
		# |__C
		#    |__F
		#    |__G

		G = GafferScene.Sphere()
		G["name"].setValue( "G" )

		F = GafferScene.Sphere()
		F["name"].setValue( "F" )

		D = GafferScene.Sphere()
		D["name"].setValue( "D" )

		E = GafferScene.Sphere()
		E["name"].setValue( "E" )

		C = GafferScene.Group()
		C["name"].setValue( "C" )

		C["in"][0].setInput( F["out"] )
		C["in"][1].setInput( G["out"] )

		B = GafferScene.Group()
		B["name"].setValue( "B" )

		B["in"][0].setInput( D["out"] )
		B["in"][1].setInput( E["out"] )

		A = GafferScene.Group()
		A["name"].setValue( "A" )
		A["in"][0].setInput( B["out"] )
		A["in"][1].setInput( C["out"] )

		context = Gaffer.Context()

		GafferSceneUI.ContextAlgo.setExpandedPaths( context, IECore.PathMatcher( [ "/" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/" ] ) )

		GafferSceneUI.ContextAlgo.setExpandedPaths( context, IECore.PathMatcher( [ "/", "/A" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A" ] ) )

		GafferSceneUI.ContextAlgo.setExpandedPaths( context, IECore.PathMatcher( [ "/", "/A", "/A/C" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A", "/A/C" ] ) )

		GafferSceneUI.ContextAlgo.clearExpansion( context )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher() )
		GafferSceneUI.ContextAlgo.expand( context, IECore.PathMatcher( [ "/A/B", "/A/C" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A", "/A/B", "/A/C" ] ) )

		GafferSceneUI.ContextAlgo.clearExpansion( context )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher() )
		GafferSceneUI.ContextAlgo.expand( context, IECore.PathMatcher( [ "/A/B", "/A/C" ] ), expandAncestors = False )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/A/B", "/A/C" ] ) )

		GafferSceneUI.ContextAlgo.setExpandedPaths( context, IECore.PathMatcher( [ "/" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/" ] ) )
		newLeafs = GafferSceneUI.ContextAlgo.expandDescendants( context, IECore.PathMatcher( [ "/" ] ), A["out"] )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A", "/A/B", "/A/C" ] ) )
		self.assertEqual( newLeafs, IECore.PathMatcher( [ "/A/B/D", "/A/B/E", "/A/C/G", "/A/C/F" ] ) )

		GafferSceneUI.ContextAlgo.setExpandedPaths( context, IECore.PathMatcher( [ "/" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/" ] ) )
		newLeafs = GafferSceneUI.ContextAlgo.expandDescendants( context, IECore.PathMatcher( [ "/" ] ), A["out"], depth = 1 )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A" ] ) )
		self.assertEqual( newLeafs, IECore.PathMatcher( [ "/A/B", "/A/C" ] ) )

		newLeafs = GafferSceneUI.ContextAlgo.expandDescendants( context, IECore.PathMatcher( [ "/A/C" ] ), A["out"], depth = 1 )
		self.assertEqual( GafferSceneUI.ContextAlgo.getExpandedPaths( context ), IECore.PathMatcher( [ "/", "/A", "/A/C" ] ) )
		self.assertEqual( newLeafs, IECore.PathMatcher( [ "/A/C/G", "/A/C/F" ] ) )

	def testSelectedPaths( self ) :

		# A
		# |__B
		#    |__D
		#    |__E
		# |__C
		#    |__F
		#    |__G

		G = GafferScene.Sphere()
		G["name"].setValue( "G" )

		F = GafferScene.Sphere()
		F["name"].setValue( "F" )

		D = GafferScene.Sphere()
		D["name"].setValue( "D" )

		E = GafferScene.Sphere()
		E["name"].setValue( "E" )

		C = GafferScene.Group()
		C["name"].setValue( "C" )

		C["in"][0].setInput( F["out"] )
		C["in"][1].setInput( G["out"] )

		B = GafferScene.Group()
		B["name"].setValue( "B" )

		B["in"][0].setInput( D["out"] )
		B["in"][1].setInput( E["out"] )

		A = GafferScene.Group()
		A["name"].setValue( "A" )
		A["in"][0].setInput( B["out"] )
		A["in"][1].setInput( C["out"] )

		context = Gaffer.Context()

		GafferSceneUI.ContextAlgo.setSelectedPaths( context, IECore.PathMatcher( [ "/" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getSelectedPaths( context ), IECore.PathMatcher( [ "/" ] ) )

		GafferSceneUI.ContextAlgo.setSelectedPaths( context, IECore.PathMatcher( [ "/", "/A" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getSelectedPaths( context ), IECore.PathMatcher( [ "/", "/A" ] ) )

		GafferSceneUI.ContextAlgo.setSelectedPaths( context, IECore.PathMatcher( [ "/", "/A", "/A/C" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getSelectedPaths( context ), IECore.PathMatcher( [ "/", "/A", "/A/C" ] )  )

		GafferSceneUI.ContextAlgo.setSelectedPaths( context, IECore.PathMatcher( [ "/A/C", "/A/B/D" ] ) )
		self.assertEqual( GafferSceneUI.ContextAlgo.getSelectedPaths( context ), IECore.PathMatcher( [ "/A/C", "/A/B/D" ] )  )

	def testAffectsExpandedPaths( self ) :

		c = Gaffer.Context()
		cs = GafferTest.CapturingSlot( c.changedSignal() )

		GafferSceneUI.ContextAlgo.setExpandedPaths( c, IECore.PathMatcher( [ "/A" ] ) )

		self.assertEqual( len( cs ), 1 )
		self.assertTrue( GafferSceneUI.ContextAlgo.affectsExpandedPaths( cs[0][1] ) )

		self.assertFalse( GafferSceneUI.ContextAlgo.affectsExpandedPaths( "frame" ) )

	def testAffectsSelectedPaths( self ) :

		c = Gaffer.Context()
		cs = GafferTest.CapturingSlot( c.changedSignal() )

		GafferSceneUI.ContextAlgo.setSelectedPaths( c, IECore.PathMatcher( [ "/A" ] ) )

		self.assertEqual( len( cs ), 1 )
		self.assertTrue( GafferSceneUI.ContextAlgo.affectsSelectedPaths( cs[0][1] ) )

		self.assertFalse( GafferSceneUI.ContextAlgo.affectsSelectedPaths( "frame" ) )

if __name__ == "__main__":
	unittest.main()
