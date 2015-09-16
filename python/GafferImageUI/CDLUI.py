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

import PyOpenColorIO

import IECore

import Gaffer
import GafferUI
import GafferImage

Gaffer.Metadata.registerNode(

	GafferImage.CDL,

	"description",
	"""
	Applies color transformations provided by
	OpenColorIO via an OCIO CDLTransform.
	""",

	plugs = {

		"slope" : [

			"description",
			"""
			Slope for the ASC CDL color correction formula.
			""",

		],

		"offset" : [

			"description",
			"""
			Offset for the ASC CDL color correction formula.
			""",

		],

		"power" : [

			"description",
			"""
			Power for the ASC CDL color correction formula.
			""",

		],

		"saturation" : [

			"description",
			"""
			Saturation from the v1.2 release of the ASC CDL color correction formula.
			""",

		],

		"direction" : [

			"description",
			"""
			The direction to perform the color transformation.
			""",

			"plugValueWidget:type", "GafferUI.PresetsPlugValueWidget",
			## \todo: is there a way to query the TransformDirection enum from OpenColorIO directly?
			"preset:Forward", 1,
			"preset:Inverse", 2,

		],

	}

)