##########################################################################
#
#  Copyright (c) 2019, Alex Fuller. All rights reserved.
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

import math
import imath

import Gaffer

Gaffer.Metadata.registerValue( "cycles:light:spot_light", "type", "spot" )
Gaffer.Metadata.registerValue( "cycles:light:spot_light", "coneAngleParameter", "spot_angle" )
Gaffer.Metadata.registerValue( "cycles:light:spot_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:spot_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:spot_light", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:spot_light", "lensRadiusParameter", "size" )

Gaffer.Metadata.registerValue( "cycles:light:point_light", "type", "point" )
Gaffer.Metadata.registerValue( "cycles:light:point_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:point_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:point_light", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:point_light", "radiusParameter", "size" )

Gaffer.Metadata.registerValue( "cycles:light:distant_light", "type", "distant" )
Gaffer.Metadata.registerValue( "cycles:light:distant_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:distant_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:distant_light", "colorParameter", "color" )

Gaffer.Metadata.registerValue( "cycles:light:quad_light", "type", "quad" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "textureNameParameter", "image" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "widthParameter", "width" )
Gaffer.Metadata.registerValue( "cycles:light:quad_light", "heightParameter", "height" )

Gaffer.Metadata.registerValue( "cycles:light:portal", "type", "quad" )
Gaffer.Metadata.registerValue( "cycles:light:portal", "widthParameter", "width" )
Gaffer.Metadata.registerValue( "cycles:light:portal", "heightParameter", "height" )

Gaffer.Metadata.registerValue( "cycles:light:disk_light", "type", "disk" )
Gaffer.Metadata.registerValue( "cycles:light:disk_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:disk_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:disk_light", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:disk_light", "widthParameter", "width" )
Gaffer.Metadata.registerValue( "cycles:light:disk_light", "textureNameParameter", "image" )

Gaffer.Metadata.registerValue( "cycles:light:background_light", "type", "environment" )
Gaffer.Metadata.registerValue( "cycles:light:background_light", "textureNameParameter", "image" )
Gaffer.Metadata.registerValue( "cycles:light:background_light", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:background_light", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:background_light", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:background_light", "visualiserOrientation", imath.M44f().rotate( imath.V3f( 0, -0.5 * math.pi, 0 ) ) )

Gaffer.Metadata.registerValue( "cycles:light:emission", "intensityParameter", "intensity" )
Gaffer.Metadata.registerValue( "cycles:light:emission", "exposureParameter", "exposure" )
Gaffer.Metadata.registerValue( "cycles:light:emission", "colorParameter", "color" )
Gaffer.Metadata.registerValue( "cycles:light:emission", "type", "mesh" )
