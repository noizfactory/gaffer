//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2012, John Haddon. All rights reserved.
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

#include "boost/python.hpp"

#include "ViewportGadgetBinding.h"

#include "GafferUIBindings/GadgetBinding.h"

#include "GafferUI/ViewportGadget.h"

#include "GafferBindings/SignalBinding.h"

#include "IECorePython/RunTimeTypedBinding.h"

using namespace boost::python;
using namespace GafferBindings;
using namespace GafferUI;
using namespace GafferUIBindings;

namespace
{

GadgetPtr getPrimaryChild( ViewportGadget &v )
{
	return v.getPrimaryChild();
}

void setViewport( ViewportGadget &v, const Imath::V2i &viewport )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.setViewport( viewport );
}

void setCamera( ViewportGadget &v, IECoreScene::Camera &camera )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.setCamera( &camera );
}

IECoreScene::CameraPtr getCamera( const ViewportGadget &v )
{
	return v.getCamera()->copy();
}

void setCameraTransform( ViewportGadget &v, const Imath::M44f &transform )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.setCameraTransform( transform );
}

void frame1( ViewportGadget &v, const Imath::Box3f &box )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.frame( box );
}

void frame2( ViewportGadget &v, const Imath::Box3f &box, const Imath::V3f &viewDirection, Imath::V3f &upVector )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.frame( box, viewDirection, upVector );
}

void fitClippingPlanes( ViewportGadget &v, const Imath::Box3f &box )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.fitClippingPlanes( box );
}

list gadgetsAt( ViewportGadget &v, const Imath::V2f &position )
{
	std::vector<Gadget*> gadgets = v.gadgetsAt( position );

	boost::python::list result;
	for( Gadget *gadget : gadgets )
	{
		result.append( GadgetPtr( gadget ) );
	}
	return result;
}

list gadgetsAt2( ViewportGadget &v, const Imath::Box2f &region, Gadget::Layer filterLayer = Gadget::Layer::None )
{
	std::vector<Gadget*> gadgets = v.gadgetsAt( region, filterLayer );

	boost::python::list result;
	for( Gadget *gadget : gadgets )
	{
		result.append( GadgetPtr( gadget ) );
	}
	return result;
}

struct ViewportGadgetSlotCaller
{
	template<typename... Args>
	void operator()( boost::python::object slot, ViewportGadgetPtr g, Args&&... args )
	{
		try
		{
			slot( g, std::forward<Args>( args )... );
		}
		catch( const error_already_set & )
		{
			IECorePython::ExceptionAlgo::translatePythonException();
		}
	}
};

void render( const ViewportGadget &v )
{
	IECorePython::ScopedGILRelease gilRelease;
	v.render();
}

class RasterScopeWrapper : boost::noncopyable
{

	public :

		RasterScopeWrapper( ViewportGadget &viewportGadget )
			:	m_viewportGadget( &viewportGadget )
		{
		}

		void enter()
		{
			m_rasterScope.emplace( m_viewportGadget.get() );
		}

		void exit( object type, object value, object traceback )
		{
			m_rasterScope.reset();
		}

	private :

		ViewportGadgetPtr m_viewportGadget;
		std::optional<ViewportGadget::RasterScope> m_rasterScope;

};

} // namespace

void GafferUIModule::bindViewportGadget()
{
	scope s = GadgetClass<ViewportGadget>()
		.def( init<>() )
		.def( init<GadgetPtr>() )
		.def( "setPrimaryChild", &ViewportGadget::setPrimaryChild )
		.def( "getPrimaryChild", &getPrimaryChild )
		.def( "getViewport", &ViewportGadget::getViewport, return_value_policy<copy_const_reference>() )
		.def( "setViewport", &setViewport )
		.def( "viewportChangedSignal", &ViewportGadget::viewportChangedSignal, return_internal_reference<1>() )
		.def( "getPlanarMovement", &ViewportGadget::getPlanarMovement )
		.def( "setPlanarMovement", &ViewportGadget::setPlanarMovement )
		.def( "getPreciseMotionAllowed", &ViewportGadget::getPreciseMotionAllowed )
		.def( "setPreciseMotionAllowed", &ViewportGadget::setPreciseMotionAllowed )
		.def( "getCamera", &getCamera )
		.def( "setCamera", &setCamera )
		.def( "getCameraTransform", &ViewportGadget::getCameraTransform, return_value_policy<copy_const_reference>() )
		.def( "setCameraTransform", &setCameraTransform )
		.def( "cameraChangedSignal", &ViewportGadget::cameraChangedSignal, return_internal_reference<1>() )
		.def( "getCameraEditable", &ViewportGadget::getCameraEditable )
		.def( "setCameraEditable", &ViewportGadget::setCameraEditable )
		.def( "setCenterOfInterest", &ViewportGadget::setCenterOfInterest )
		.def( "getCenterOfInterest", (float (ViewportGadget::*)())&ViewportGadget::getCenterOfInterest )
		.def( "setTumblingEnabled", &ViewportGadget::setTumblingEnabled )
		.def( "getTumblingEnabled", &ViewportGadget::getTumblingEnabled )
		.def( "setDollyingEnabled", &ViewportGadget::setDollyingEnabled )
		.def( "getDollyingEnabled", &ViewportGadget::getDollyingEnabled )
		.def( "setMaxPlanarZoom", &ViewportGadget::setMaxPlanarZoom )
		.def( "getMaxPlanarZoom", (Imath::V2f (ViewportGadget::*)())&ViewportGadget::getMaxPlanarZoom )
		.def( "frame", &frame1 )
		.def( "frame", &frame2, ( arg_( "box" ), arg_( "viewDirection" ), arg_( "upVector" ) = Imath::V3f( 0, 1, 0 ) ) )
		.def( "fitClippingPlanes", &fitClippingPlanes )
		.def( "setDragTracking", &ViewportGadget::setDragTracking )
		.def( "getDragTracking", &ViewportGadget::getDragTracking )
		.def( "setVariableAspectZoom", &ViewportGadget::setVariableAspectZoom )
		.def( "getVariableAspectZoom", &ViewportGadget::getVariableAspectZoom )
		.def( "gadgetsAt", &gadgetsAt )
		.def( "gadgetsAt", &gadgetsAt2, ( arg_( "rasterRegion" ), arg_( "filterLayer" ) = Gadget::Layer::None )  )
		.def( "rasterToGadgetSpace", &ViewportGadget::rasterToGadgetSpace, ( arg_( "rasterPosition" ), arg_( "gadget" ) ) )
		.def( "gadgetToRasterSpace", &ViewportGadget::gadgetToRasterSpace, ( arg_( "gadgetPosition" ), arg_( "gadget" ) ) )
		.def( "rasterToWorldSpace", &ViewportGadget::rasterToWorldSpace, ( arg_( "rasterPosition" ) ) )
		.def( "worldToRasterSpace", &ViewportGadget::worldToRasterSpace, ( arg_( "worldPosition" ) ) )
		.def( "render", &render )
		.def( "preRenderSignal", &ViewportGadget::preRenderSignal, return_internal_reference<1>() )
		.def( "renderRequestSignal", &ViewportGadget::renderRequestSignal, return_internal_reference<1>() )
		.def( "setPostProcessShader", &ViewportGadget::setPostProcessShader )
		.def( "getPostProcessShader", &ViewportGadget::getPostProcessShader )
	;

	enum_<ViewportGadget::CameraFlags>( "CameraFlags" )
		.value( "None_", ViewportGadget::CameraFlags::None )
		.value( "Camera", ViewportGadget::CameraFlags::Camera )
		.value( "Transform", ViewportGadget::CameraFlags::Transform )
		.value( "CenterOfInterest", ViewportGadget::CameraFlags::CenterOfInterest )
		.value( "All", ViewportGadget::CameraFlags::All )
	;

	enum_<ViewportGadget::DragTracking>( "DragTracking" )
		.value( "NoDragTracking", ViewportGadget::NoDragTracking )
		.value( "XDragTracking", ViewportGadget::XDragTracking )
		.value( "YDragTracking", ViewportGadget::YDragTracking )
	;

	class_<RasterScopeWrapper, boost::noncopyable>( "RasterScope", no_init )
		.def( init<ViewportGadget &>() )
		.def( "__enter__", &RasterScopeWrapper::enter )
		.def( "__exit__", &RasterScopeWrapper::exit )
	;

	SignalClass<ViewportGadget::UnarySignal, DefaultSignalCaller<ViewportGadget::UnarySignal>, ViewportGadgetSlotCaller>( "UnarySignal" );
	SignalClass<ViewportGadget::CameraChangedSignal, DefaultSignalCaller<ViewportGadget::CameraChangedSignal>, ViewportGadgetSlotCaller>( "UnarySignal" );

}
