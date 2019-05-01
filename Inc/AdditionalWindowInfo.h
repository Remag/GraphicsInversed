#pragma once
#include <GlWindow.h>
#include <RenderMechanism.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

struct CAdditionalWindowInfo {
	CGlWindow Window;
	CPtrOwner<IRenderMechanism> Renderer;

	CAdditionalWindowInfo( CGlWindow wnd, CPtrOwner<IRenderMechanism> renderer ) : Window( move( wnd ) ), Renderer( move( renderer ) ) {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

