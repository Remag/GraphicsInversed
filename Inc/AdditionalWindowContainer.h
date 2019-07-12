#pragma once
#include <GlWindow.h>
#include <RenderMechanism.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

enum TWindowRendererType {
	WRT_OpenGl,
	WRT_WinGdi,
	WRT_EnumCount
};

//////////////////////////////////////////////////////////////////////////

class CAdditionalWindowContainer {
public:
	void AddAdditionalWindow( CMainFrame& mainFrame, CPtrOwner<CGlWindow> window, TWindowRendererType rendererType );
	CGlWindow* FindWindowByClass( CUnicodePart className );
	void TryCloseAdditionalWindow( HWND window );
	void DrawAdditionalWindows( CMainFrame& mainFrame, const IState& currentState ) const;

private:
	struct CAdditionalWindowInfo {
		CPtrOwner<CGlWindow> Window;
		CPtrOwner<IRenderMechanism> Renderer;
	};
	CArray<CAdditionalWindowInfo> additionalWindows;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

