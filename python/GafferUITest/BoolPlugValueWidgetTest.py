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

class BoolPlugValueWidgetTest( GafferUITest.TestCase ) :

	def test( self ) :

		n = Gaffer.Node()
		n["user"]["p1"] = Gaffer.BoolPlug( flags = Gaffer.Plug.Flags.Default | Gaffer.Plug.Flags.Dynamic )
		n["user"]["p2"] = Gaffer.BoolPlug( flags = Gaffer.Plug.Flags.Default | Gaffer.Plug.Flags.Dynamic )

		w = GafferUI.BoolPlugValueWidget( n["user"]["p1"] )
		self.assertEqual( w.getPlug(), n["user"]["p1"] )
		self.assertEqual( w.getPlugs(), { n["user"]["p1"] } )
		self.assertEqual( w.boolWidget().getState(), False )

		n["user"]["p1"].setValue( True )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.boolWidget().getState(), True )

		w.setPlugs( n["user"].children() )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.boolWidget().getState(), w.boolWidget().State.Indeterminate )

		n["user"]["p2"].setValue( True )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.boolWidget().getState(), True )

		w.setPlugs( [] )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertEqual( w.boolWidget().getState(), w.boolWidget().State.Indeterminate )

	def testInitialValue( self ) :

		n = Gaffer.Node()
		n["user"]["p"] = Gaffer.BoolPlug( flags = Gaffer.Plug.Flags.Default | Gaffer.Plug.Flags.Dynamic )

		for v in ( True, False ) :
			n["user"]["p"].setValue( v )
			w = GafferUI.BoolPlugValueWidget( n["user"]["p"] )
			GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
			self.assertEqual( w.boolWidget().getState(), v )

	def testErrorHandling( self ) :

		script = Gaffer.ScriptNode()

		script["n"] = Gaffer.Node()
		script["n"]["user"]["p"] = Gaffer.BoolPlug( flags = Gaffer.Plug.Flags.Default | Gaffer.Plug.Flags.Dynamic )

		w = GafferUI.BoolPlugValueWidget( script["n"]["user"]["p"] )
		self.assertFalse( w.boolWidget().getErrored() )

		script["b"] = GafferTest.BadNode()
		script["n"]["user"]["p"].setInput( script["b"]["out3"] )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertTrue( w.boolWidget().getErrored() )

		script["n"]["user"]["p"].setInput( None )
		GafferUITest.PlugValueWidgetTest.waitForUpdate( w )
		self.assertFalse( w.boolWidget().getErrored() )

if __name__ == "__main__":
	unittest.main()
