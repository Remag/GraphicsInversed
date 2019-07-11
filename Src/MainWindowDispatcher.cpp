#include <common.h>
#pragma hdrstop

#include <MainWindowDispatcher.h>
#include <MainFrame.h>
#include <RenderMechanism.h>
#include <Application.h>
#include <GlWindow.h>
#include <StateManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

LRESULT CMainWindowDispatcher::OnEraseBackground( HWND wnd, WPARAM wParam, LPARAM lParam ) const
{
	return mainFrame.GetRenderer().OnEraseBackground( wnd, wParam, lParam );
}

LRESULT CMainWindowDispatcher::OnPaint( HWND wnd, WPARAM wParam, LPARAM lParam ) const
{
	const auto currentState = stateManager.TryGetCurrentState();
	if( currentState != nullptr ) {
		return mainFrame.GetRenderer().OnPaintMessage( wnd, wParam, lParam, *currentState );
	} else {
		return ::DefWindowProc( wnd, WM_PAINT, wParam, lParam );
	}
}

void CMainWindowDispatcher::OnWindowDestroy( HWND wnd ) const
{
	if( application.TryQuitOnWindowDestruction( wnd ) ) {
		::PostQuitMessage( 0 );
	}
}

void CMainWindowDispatcher::OnWindowClose( HWND wnd ) const
{
	if( application.TryCloseMainWindow() ) {
		checkLastError( ::DestroyWindow( wnd ) != 0 );
	}
}

void CMainWindowDispatcher::OnWindowActivation( HWND, WPARAM wParam ) const
{
	if( wParam != WA_INACTIVE ) {
		application.OnRestore();
		mainWindow.SetActivation( true );
	} else {
		inputHandler.ClearPressedKeys();
		application.OnSleep();
		mainWindow.SetActivation( false );
	}
}

void CMainWindowDispatcher::OnWindowResize( HWND, WPARAM wParam, int newWidth, int newHeight ) const
{
	if( wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED ) {
		// Window may post WM_SIZE during its creation.
		if( mainWindow.Handle() != nullptr ) {
			mainWindow.RefreshCachedValues();
			const CVector2<int> newSize{ newWidth, newHeight };
			mainFrame.GetRenderer().OnWindowResize( newSize );
			application.OnMainWindowResize( newSize, wParam == SIZE_MAXIMIZED );
		}
	}
}

void CMainWindowDispatcher::OnWindowActiveResize( HWND ) const
{
	mainWindow.RefreshCachedValues();
	const auto newSize = mainWindow.WindowSize();
	auto& renderer = mainFrame.GetRenderer();
	renderer.OnWindowResize( newSize );
	application.OnMainWindowResize( newSize, false );

	const auto state = stateManager.TryGetCurrentState();
	if( state != nullptr ) {
		renderer.OnDraw( *state, mainWindow );
		renderer.OnPostDraw( mainWindow );
	}
}

void CMainWindowDispatcher::OnWindowMove( HWND ) const
{
	mainWindow.RefreshCachedValues();
}

void CMainWindowDispatcher::OnChar( HWND, WPARAM wParam ) const
{
	inputHandler.OnSymbolMessage( wParam );
}

void CMainWindowDispatcher::OnInput( HWND, LPARAM lParam ) const
{
	RAWINPUT resultInput;
	UINT size = sizeof( resultInput );
	::GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, &resultInput, &size, sizeof( RAWINPUTHEADER ) );
	if( resultInput.header.dwType != RIM_TYPEKEYBOARD ) {
		return;
	}

	const auto processedInput = processRawKeyboard( resultInput.data.keyboard );
	if( processedInput >= 256 ) {
		return;
	}

	const bool isKeyDown = !HasFlag( resultInput.data.keyboard.Flags, RI_KEY_BREAK );
	inputHandler.OnKeyboardPress( processedInput, isKeyDown );
}

int CMainWindowDispatcher::processRawKeyboard( RAWKEYBOARD input ) const
{
	const auto virtualKey = input.VKey;
	const bool isE0 = HasFlag( input.Flags, RI_KEY_E0 );
	switch( virtualKey ) {
		case VK_SHIFT:
			// Left and right shift have different scan codes.
			return ::MapVirtualKey( input.MakeCode, MAPVK_VSC_TO_VK_EX );
		case VK_CONTROL:
			// Left and right control have different extension flags.
			return isE0 ? VK_RCONTROL : VK_LCONTROL;
		case VK_MENU:
			// Left and right alt have different extension flags.
			return isE0 ? VK_RMENU : VK_LMENU;
		case VK_RETURN:
			// Return on the numpad has a different extension flag.
			// Since there's no virtual code for the numpad enter, use the unassigned value.
			return isE0 ? 0x88 : VK_RETURN;

			// Distinguish between numpad and standard keys.
		case VK_INSERT:
			return isE0 ? VK_INSERT : VK_NUMPAD0;
		case VK_DELETE:
			return isE0 ? VK_DELETE : 0x89;
		case VK_HOME:
			return isE0 ? VK_HOME : VK_NUMPAD7;
		case VK_END:
			return isE0 ? VK_END : VK_NUMPAD1;
		case VK_PRIOR:
			return isE0 ? VK_PRIOR : VK_NUMPAD9;
		case VK_NEXT:
			return isE0 ? VK_NEXT : VK_NUMPAD3;
		case VK_LEFT:
			return isE0 ? VK_LEFT : VK_NUMPAD4;
		case VK_RIGHT:
			return isE0 ? VK_RIGHT : VK_NUMPAD6;
		case VK_UP:
			return isE0 ? VK_UP : VK_NUMPAD8;
		case VK_DOWN:
			return isE0 ? VK_DOWN : VK_NUMPAD2;
		case VK_CLEAR:
			return isE0 ? VK_CLEAR : VK_NUMPAD5;
		default:
			return virtualKey;
	}
}

void CMainWindowDispatcher::OnMouseWheel( HWND, int wheelDelta ) const
{
	const auto isDown = wheelDelta < 0;
	const int wheelCode = isDown ? 0x0E : 0x0F;
	inputHandler.OnMousePress( wheelCode, true );
	inputHandler.OnMousePress( wheelCode, false );
}

void CMainWindowDispatcher::OnMouseMove( HWND, int mouseX, int mouseY ) const
{
	inputHandler.OnMouseMove( mouseX, mouseY );
}

void CMainWindowDispatcher::OnMouseLeave( HWND ) const
{
	inputHandler.OnMouseLeave();
}

void CMainWindowDispatcher::OnMousePress( HWND, int virtualCode, bool isPressed ) const
{
	inputHandler.OnMousePress( virtualCode, isPressed );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

