#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

class GINAPI CStandardWindowDispatcher {
public:
	explicit CStandardWindowDispatcher( bool _trackMouseLeave ) : trackMouseLeave( _trackMouseLeave ) {}

	void AcceptNewWindow( CGlWindow& newWindow );
	bool ShouldTrackMouseLeave() const
		{ return trackMouseLeave; }

	HCURSOR GetCurrentCursor() const
		{ return currentCursor; }
	void SetCurrentCursor( HCURSOR newValue )
		{ currentCursor = newValue; }

	LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) const;
	LRESULT OnPaint( HWND, WPARAM, LPARAM ) const;
	void OnWindowDestroy( HWND ) const;
	void OnWindowClose( HWND ) const;
	void OnWindowActivation( HWND, WPARAM ) const;
	void OnWindowResize( HWND, WPARAM, int, int ) const;
	void OnWindowActiveResize( HWND ) const;
	void OnWindowMove( HWND ) const;
	void OnChar( HWND, WPARAM ) const;
	void OnInput( HWND, LPARAM ) const;
	void OnMouseWheel( HWND, int ) const;
	void OnMouseMove( HWND, int, int ) const;
	void OnMouseLeave( HWND ) const;
	void OnMousePress( HWND, int, bool ) const;

	HCURSOR OnCursorChange( HWND, WPARAM, LPARAM ) const
		{ return currentCursor; }

private:
	CGlWindow* targetWindow = nullptr;
	HCURSOR currentCursor = nullptr;
	bool trackMouseLeave;

	int processRawKeyboard( RAWKEYBOARD input ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

