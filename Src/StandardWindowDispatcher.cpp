#include <common.h>
#pragma hdrstop

#include <StandardWindowDispatcher.h>
#include <GinGlobals.h>
#include <MainFrame.h>
#include <RenderMechanism.h>
#include <Application.h>
#include <GlWindow.h>
#include <StateManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CStandardWindowDispatcher::AcceptNewWindow( CGlWindow& newWindow )
{
	targetWindow = &newWindow;
}

LRESULT CStandardWindowDispatcher::OnEraseBackground( HWND wnd, WPARAM wParam, LPARAM lParam ) const
{
	return GinInternal::GetMainFrame().GetRenderer().OnEraseBackground( wnd, wParam, lParam );
}

LRESULT CStandardWindowDispatcher::OnPaint( HWND wnd, WPARAM wParam, LPARAM lParam ) const
{
	const auto currentState = GetStateManager().TryGetCurrentState();
	if( currentState != nullptr ) {
		return targetWindow->GetRenderer().OnPaintMessage( wnd, wParam, lParam, *currentState );
	} else {
		return ::DefWindowProc( wnd, WM_PAINT, wParam, lParam );
	}
}

void CStandardWindowDispatcher::OnWindowDestroy( HWND ) const
{
	GinInternal::GetInputHandler().OnWindowDestruction( targetWindow );
	if( GinInternal::GetApplication().HandleWindowDestruction( *targetWindow ) ) {
		::PostQuitMessage( 0 );
	}
}

void CStandardWindowDispatcher::OnWindowClose( HWND wnd ) const
{
	if( GinInternal::GetApplication().TryCloseWindow( *targetWindow ) ) {
		checkLastError( ::DestroyWindow( wnd ) != 0 );
	}
}

void CStandardWindowDispatcher::OnWindowActivation( HWND, WPARAM wParam ) const
{
	auto& application = GinInternal::GetApplication();
	if( wParam != WA_INACTIVE ) {
		application.OnRestore();
		targetWindow->SetActivation( true );
	} else {
		GinInternal::GetInputHandler().ClearPressedKeys();
		application.OnSleep();
		targetWindow->SetActivation( false );
	}
}

void CStandardWindowDispatcher::OnWindowResize( HWND, WPARAM wParam, int newWidth, int newHeight ) const
{
	if( wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED ) {
		auto& application = GinInternal::GetApplication();
		// Window may post WM_SIZE during its creation.
		if( targetWindow->IsCreated() ) {
			targetWindow->RefreshCachedValues();
			const CVector2<int> newSize{ newWidth, newHeight };
			targetWindow->GetRenderer().OnWindowResize( newSize );
			application.OnWindowResize( *targetWindow, newSize, wParam == SIZE_MAXIMIZED );
		}
	}
}

void CStandardWindowDispatcher::OnWindowActiveResize( HWND ) const
{
	auto& application = GinInternal::GetApplication();
	targetWindow->RefreshCachedValues();
	const auto newSize = targetWindow->WindowSize();
	auto& renderer = targetWindow->GetRenderer();
	renderer.OnWindowResize( newSize );
	application.OnWindowResize( *targetWindow, newSize, false );

	const auto state = GetStateManager().TryGetCurrentState();
	if( state != nullptr ) {
		renderer.OnDraw( *state );
		renderer.OnPostDraw();
	}
}

void CStandardWindowDispatcher::OnWindowMove( HWND ) const
{
	targetWindow->RefreshCachedValues();
}

void CStandardWindowDispatcher::OnChar( HWND, WPARAM wParam ) const
{
	GinInternal::GetInputHandler().OnSymbolMessage( wParam );
}

void CStandardWindowDispatcher::OnInput( HWND, LPARAM lParam ) const
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
	GinInternal::GetInputHandler().OnKeyboardPress( processedInput, isKeyDown );
	// For keys that are often doubled on a keyboard also send their generic virtual key.
	const auto vKey = resultInput.data.keyboard.VKey;
	if( vKey == VK_SHIFT || vKey == VK_CONTROL ) {
		GinInternal::GetInputHandler().OnKeyboardPress( vKey, isKeyDown );
	}
}

int CStandardWindowDispatcher::processRawKeyboard( RAWKEYBOARD input ) const
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

void CStandardWindowDispatcher::OnMouseWheel( HWND, int wheelDelta ) const
{
	const auto isDown = wheelDelta < 0;
	const int wheelCode = isDown ? 0x0E : 0x0F;
	GinInternal::GetInputHandler().OnMousePress( wheelCode, true );
	GinInternal::GetInputHandler().OnMousePress( wheelCode, false );
}

void CStandardWindowDispatcher::OnMouseMove( HWND, int mouseX, int mouseY ) const
{
	GinInternal::GetInputHandler().OnMouseMove( mouseX, mouseY, *targetWindow );
}

void CStandardWindowDispatcher::OnMouseLeave( HWND ) const
{
	GinInternal::GetInputHandler().OnMouseLeave();
}

void CStandardWindowDispatcher::OnMousePress( HWND, int virtualCode, bool isPressed ) const
{
	GinInternal::GetInputHandler().OnMousePress( virtualCode, isPressed );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

