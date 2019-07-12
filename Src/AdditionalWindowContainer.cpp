#include <common.h>
#pragma hdrstop

#include <AdditionalWindowContainer.h>
#include <MainFrame.h>

#include <OpenGlRenderMechanism.h>
#include <WinGdiRenderMechanism.h>


namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CAdditionalWindowContainer::AddAdditionalWindow( CMainFrame& mainFrame, CPtrOwner<CGlWindow> window, TWindowRendererType rendererType )
{
	CPtrOwner<IRenderMechanism> renderer;
	switch( rendererType ) {
		case WRT_OpenGl:
			renderer = CreateOwner<COpenGlRenderMechanism>( mainFrame.GetGlContextManager() );
			break;
		case WRT_WinGdi:
			renderer = CreateOwner<CWinGdiRenderMechanism>();
			break;
		default:
			assert( false );
	}

	renderer->AttachNewWindow( *window );
	window->setRenderMechanism( *renderer );
	auto& result = additionalWindows.Add( move( window ), move( renderer ) );
	result.Window->Show( true );
}

CGlWindow* CAdditionalWindowContainer::FindWindowByClass( CUnicodePart windowClassName )
{
	for( auto& window : additionalWindows ) {
		if( window.Window->GetWindowClassName() == windowClassName ) {
			return window.Window;
		}
	}
	return nullptr;
}

void CAdditionalWindowContainer::TryCloseAdditionalWindow( HWND window )
{
	for( int i = additionalWindows.Size() - 1; i >= 0; i-- ) {
		if( additionalWindows[i].Window->Handle() == window ) {
			additionalWindows.DeleteAt( i );
			return;
		}
	}
}

void CAdditionalWindowContainer::DrawAdditionalWindows( CMainFrame& mainFrame, const IState& currentState ) const
{
	for( const auto& window : additionalWindows ) {
		auto& renderer = const_cast<IRenderMechanism&>( *window.Renderer );
		renderer.ActivateWindowTarget();
		renderer.OnDraw( currentState );
		renderer.OnPostDraw();
	}

	if( !additionalWindows.IsEmpty() ) {
		auto& renderer = const_cast<IRenderMechanism&>( mainFrame.GetRenderer() );
		renderer.ActivateWindowTarget();
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

