##########################################################################
#
#  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
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

import Gaffer
import GafferTest
import GafferUI
import GafferUITest

class NumericPlugValueWidgetTest( GafferUITest.TestCase ) :

	def test( self ) :

		n = Gaffer.Node()
		n["i"]= Gaffer.IntPlug()
		n["f"] = Gaffer.FloatPlug()

		w = GafferUI.NumericPlugValueWidget( n["i"] )
		self.assertTrue( w.getPlug().isSame( n["i"] ) )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertTrue( isinstance( w.numericWidget().getValue(), int ) )

		w.setPlug( n["f"] )
		self.assertTrue( w.getPlug().isSame( n["f"] ) )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertTrue( isinstance( w.numericWidget().getValue(), float ) )

		w = GafferUI.NumericPlugValueWidget( plugs = [] )
		self.assertEqual( w.getPlug(), None )
		self.assertEqual( w.numericWidget().getEditable(), False )

		w.setPlug( n["f"] )
		self.assertTrue( w.getPlug().isSame( n["f"] ) )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertTrue( isinstance( w.numericWidget().getValue(), float ) )
		self.assertEqual( w.numericWidget().getEditable(), True )

	def testEditMultiplePlugs( self ) :

		n = Gaffer.Node()
		n["user"]["i1"] = Gaffer.IntPlug()
		n["user"]["i2"] = Gaffer.IntPlug()

		w = GafferUI.NumericPlugValueWidget( n["user"].children() )
		self.assertEqual( w.getPlugs(), { n["user"]["i1"], n["user"]["i2"] } )

		n["user"]["i1"].setValue( 2 )
		n["user"]["i2"].setValue( 2 )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getText(), "2" )

		n["user"]["i1"].setValue( 1 )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getText(), "" )
		self.assertEqual( w.numericWidget()._qtWidget().placeholderText(), "---" )

		w.numericWidget().setValue( 10 )
		self.assertEqual( n["user"]["i1"].getValue(), 10 )
		self.assertEqual( n["user"]["i2"].getValue(), 10 )

		Gaffer.MetadataAlgo.setReadOnly( n["user"]["i1"], True )
		self.assertFalse( w.numericWidget().getEditable() )

	def testChangeToMixedPlugsDoesntOverwriteExistingPlugValues( self ) :

		n = Gaffer.Node()
		n["user"]["i1"] = Gaffer.IntPlug()
		n["user"]["i2"] = Gaffer.IntPlug()
		n["user"]["i1"].setValue( 1 )
		n["user"]["i2"].setValue( 2 )

		w = GafferUI.NumericPlugValueWidget( n["user"]["i1"] )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getText(), "1" )
		self.assertEqual( w.numericWidget()._qtWidget().placeholderText(), "" )

		w.setPlugs( n["user"].children() )
		self.assertEqual( w.getPlugs(), { n["user"]["i1"], n["user"]["i2"] } )

		self.assertEqual( n["user"]["i1"].getValue(), 1 )
		self.assertEqual( n["user"]["i2"].getValue(), 2 )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getText(), "" )
		self.assertEqual( w.numericWidget()._qtWidget().placeholderText(), "---" )

	def testMixedOrInvalidValuesPreservesExisting( self ) :

		n = Gaffer.Node()
		n["user"]["i1"] = Gaffer.IntPlug()
		n["user"]["i2"] = Gaffer.IntPlug()
		n["user"]["i1"].setValue( 1 )
		n["user"]["i2"].setValue( 2 )

		w = GafferUI.NumericPlugValueWidget( n["user"]["i1"] )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getValue(), 1 )

		w.numericWidget().setText( "" )
		w.numericWidget()._qtWidget().editingFinished.emit()

		self.assertEqual( n["user"]["i1"].getValue(), 1 )

		w = GafferUI.NumericPlugValueWidget( n["user"].children() )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.numericWidget().getText(), "" )

		w.numericWidget()._qtWidget().editingFinished.emit()

		self.assertEqual( n["user"]["i1"].getValue(), 1 )
		self.assertEqual( n["user"]["i2"].getValue(), 2 )

	def testFixedCharacterWidth( self ) :

		n = Gaffer.Node()
		n["user"]["i1"] = Gaffer.IntPlug( minValue = 0, maxValue = 100 )
		n["user"]["i2"] = Gaffer.IntPlug( minValue = 0, maxValue = 1000 )
		n["user"]["i3"] = Gaffer.IntPlug( minValue = -5, maxValue = 0 )

		w = GafferUI.NumericPlugValueWidget( n["user"]["i2"] )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 4 )

		w.setPlug( n["user"]["i1"] )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 3 )

		w.setPlugs( n["user"].children() )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 4 )

		w.setPlugs( { n["user"]["i1"] } )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 3 )

		w.setPlugs( { n["user"]["i3"] } )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 2 )

	def testFixedCharacterWidthMetadata( self ) :

		n = Gaffer.Node()
		n["user"]["i1"] = Gaffer.IntPlug()
		n["user"]["i2"] = Gaffer.IntPlug()

		Gaffer.Metadata.registerValue( n["user"]["i1"], "numericPlugValueWidget:fixedCharacterWidth", 3 )
		w = GafferUI.NumericPlugValueWidget( n["user"]["i1"] )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 3 )

		w.setPlug( n["user"]["i2"] )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), None )

		Gaffer.Metadata.registerValue( n["user"]["i2"], "numericPlugValueWidget:fixedCharacterWidth", 4 )
		self.assertEqual( w.numericWidget().getFixedCharacterWidth(), 4 )

	def testEditability( self ) :

		n = GafferTest.AddNode()

		w = GafferUI.NumericPlugValueWidget( n["op1"] )
		self.assertTrue( w.numericWidget().getEditable() )

		Gaffer.MetadataAlgo.setReadOnly( n["op1"], True )
		self.assertFalse( w.numericWidget().getEditable() )

		w.setPlug( n["op2"] )
		self.assertTrue( w.numericWidget().getEditable() )

		w.setPlug( None )
		self.assertFalse( w.numericWidget().getEditable() )

if __name__ == "__main__":
	unittest.main()
