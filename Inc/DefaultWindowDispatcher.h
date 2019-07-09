#pragma once

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class that provides default window handling messages.
class CDefaultWindowDispatcher {
public:
	LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) const
		{ return ::DefWindowProc( window, WM_ERASEBKGND, wParam, lParam ); }

	void OnWindowDestroy( HWND ) const {}
	void OnWindowClose( HWND ) const {}
	void OnWindowActivation( HWND, WPARAM ) const {}
	void OnWindowResize( HWND, WPARAM, int, int ) const {}
	void OnWindowActiveResize( HWND ) const {}
	void OnWindowMove( HWND ) const {}
	void OnPaint( HWND, WPARAM, LPARAM ) const {}
	void OnChar( HWND, WPARAM ) const {}
	void OnInput( HWND, LPARAM ) const {}
	void OnMouseWheel( HWND, int ) const {}
	void OnMouseMove( HWND, int, int ) const {}
	void OnMouseLeave( HWND ) const {}
	void OnMousePress( HWND, int, bool ) const {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

