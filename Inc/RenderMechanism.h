#pragma once

namespace Gin {

class IState;
class CGlWindow;
enum TTexelFormat;
//////////////////////////////////////////////////////////////////////////

// General renderer parameters that are passed to a draw call of the state.
class IRenderParameters {};

// General interface for the used rendering mechanism.
// The mechanism contains reactions to window and update events.
class IRenderMechanism {
public:
	virtual ~IRenderMechanism() {}

	// Change the render target to a new window.
	virtual void ActivateWindowTarget( const CGlWindow& newTarget ) = 0;
	// Set the initial screen color.
	virtual void SetBackgroundColor( CColor newValue ) = 0;
	// Actions taken on window resize.
	virtual void OnWindowResize( CVector2<int> newSize ) = 0;
	// Actions on the WM_ERASEBKGND message.
	virtual LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) = 0;
	// Actions taken on a draw call that happens on each frame.
	virtual void OnDraw( const IState& currentState ) const = 0;
	virtual void OnPostDraw( const CGlWindow& target ) const = 0;
	// Actions taken on a WM_PAINT message.
	virtual LRESULT OnPaintMessage( HWND window, WPARAM wParam, LPARAM lParam, const IState& currentState ) const = 0;
	// Read the pixels from the screen framebuffer.
	virtual CArray<BYTE> ReadScreenBuffer( TTexelFormat format, CVector2<int>& bufferSize ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

