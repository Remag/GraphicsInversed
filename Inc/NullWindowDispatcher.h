#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class that provides noop window handling messages.
class CDefaultWindowDispatcher {
public:
	// Called before window creation.
	void AcceptNewWindow( const CGlWindow& ) {}
	// Called during class creation to determine if the mouse needs to be tracked when leaving the window.
	bool ShouldTrackMouseLeave() const
		{ return false; }

	LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) const
		{ return ::DefWindowProc( window, WM_ERASEBKGND, wParam, lParam ); }
	LRESULT OnPaint( HWND window, WPARAM wParam, LPARAM lParam ) const 
		{ return ::DefWindowProc( window, WM_PAINT, wParam, lParam ); }

	void OnWindowDestroy( HWND ) const {}
	void OnWindowClose( HWND ) const {}
	void OnWindowActivation( HWND, WPARAM ) const {}
	void OnWindowResize( HWND, WPARAM, int, int ) const {}
	void OnWindowActiveResize( HWND ) const {}
	void OnWindowMove( HWND ) const {}
	void OnChar( HWND, WPARAM ) const {}
	void OnInput( HWND, LPARAM ) const {}
	void OnMouseWheel( HWND, int ) const {}
	void OnMouseMove( HWND, int, int ) const {}
	void OnMouseLeave( HWND ) const {}
	void OnMousePress( HWND, int, bool ) const {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

