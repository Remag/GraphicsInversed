#pragma once
#include <DefaultWindowDispatcher.h>
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

class CMainWindowDispatcher : public CDefaultWindowDispatcher {
public:
	CMainWindowDispatcher( CMainFrame& _mainFrame, CGlWindow& _mainWindow, CApplication& _application, CStateManager& _stateManager, CInputHandler& _inputHandler, bool _trackMouseLeave ) :
		mainFrame( _mainFrame ), mainWindow( _mainWindow ), application( _application ), stateManager( _stateManager ), inputHandler( _inputHandler ), trackMouseLeave( _trackMouseLeave ) {}

	bool ShouldTrackMouseLeave() const
		{ return trackMouseLeave; }

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

private:
	CMainFrame& mainFrame;
	CGlWindow& mainWindow;
	CApplication& application;
	CStateManager& stateManager;
	CInputHandler& inputHandler;
	bool trackMouseLeave;

	int processRawKeyboard( RAWKEYBOARD input ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

