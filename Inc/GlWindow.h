#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

enum TWindowOriginPoint {
	WOP_TopLeft,
	WOP_Center,
	WOP_EnumCount
};

// Initial size and style settings for the window.
struct CGlWindowSettings {
	DWORD WindowStyle = 0;
	CVector2<int> WindowPosition;
	CVector2<int> WindowSize;
	TWindowOriginPoint Origin;
	bool IsFullscreen = false;
	bool TrackMouseLeave = false;

	CGlWindowSettings() = default;
	CGlWindowSettings( DWORD style, CVector2<int> topLeft, CVector2<int> size, TWindowOriginPoint origin, bool isFullscreen, bool trackMouseLeave ) : 
		WindowStyle( style ), WindowPosition( topLeft ), WindowSize( size ), Origin( origin ), IsFullscreen( isFullscreen ), TrackMouseLeave( trackMouseLeave ) {}
};

//////////////////////////////////////////////////////////////////////////

// Class for the window that is supposed to have an OpenGL context attached.
class GINAPI CGlWindow {
public:
	CGlWindow() = default;
	~CGlWindow();

	CUnicodeView GetWindowClassName() const
		{ return windowClassName; }
	CVector2<int> WindowSize() const
		{ return windowSize; }
	bool IsFullscreen() const
		{ return isFullscreen; }
	IRenderMechanism& GetRenderer() 
		{ return *renderMechanism; }
	const IRenderMechanism& GetRenderer() const
		{ return *renderMechanism; }

	HDC GetDeviceContext() const
		{ return windowDC; }

	template <class Dispatcher>
	void Create( CWindowClass<Dispatcher>& windowClass, CGlWindowSettings initialSettings );

	// Get current screen resolution.
	CVector2<int> GetScreenResolution() const;
	// Get the window resolution list, supported by the system. All resolutions smaller than minResolutions are filtered out.
	// The returned index is the position of the current resolution in the list.
	int GetSupportedResolutions( CVector2<int> minResolution, CArray<CVector2<int>>& result ) const;
	// Set fullscreen resolution.
	void SetFullscreenResolution( CVector2<int> newValue );

	// Set window style and shape that represents windowed/fullscreen mode.
	void SetFullscreen( bool isSet );
	// Set the window maximized value.
	void SetMaximized( bool isSet );
	// Set the window style.
	void SetWindowStyle( DWORD newStyle );
	// Set a rectangle in a windowed mode.
	void SetWindowRect( CAARect<int> rect );
	// Set a rectangle in a windowed mode. Given rectangle represent the desired client area rect.
	void SetClientRect( CAARect<int> rect );
	// Get the rectangle in windowed mode.
	CAARect<int> GetWindowModeRect() const;

	// Native window handle.
	HWND Handle() const
		{ return windowHandle; }
	// Is the window created.
	bool IsCreated() const
		{ return windowHandle != nullptr; }

	// Update the size cache.
	// Called from the mainframe during WM_SIZE handling.
	void RefreshCachedValues();
	// Update the size cache from an ongoing drag operation.
	void UpdateFromActiveResize( RECT dragRect );
	void SetActivation( bool isSet );
	void BringToForeground();
	void Show( bool isShown );


	// Send the close message emulating the user pressing the close icon.
	void SendCloseCommand();
	// Destroy the window right away.
	void ForceDestroy();

	// Window containers need to attach the renderer information.
	friend class CAdditionalWindowContainer;
	friend class CMainFrame;

private:
	CUnicodeString windowClassName;
	IRenderMechanism* renderMechanism = nullptr;
	// Native handle to the window.
	HWND windowHandle = nullptr;
	HDC windowDC = nullptr;
	// Window style that is used during window creation.
	DWORD windowStyle;
	CVector2<int> windowSize;

	// Screen resolution in fullscreen mode. Gets set every time the window goes in fullscreen mode.
	CVector2<int> fullscreenResolution;
	// Screen resolution at the start of the program. Restored every time window goes out of fullscreen mode.
	CVector2<int> initialScreenResolution;

	// Window rectangle in a windowed mode.
	RECT windowedModeRect;
	// Is the window currently in fullscreen mode.
	bool isFullscreen = false;

	void setRenderMechanism( IRenderMechanism& newValue );
	void doCreate( CUnicodeView className, CGlWindowSettings initialSettings );

	CVector2<int> findDefaultResolution() const;
	static bool compareDisplaySettings( const DEVMODE& left, const DEVMODE& right );
	static CVector2<int> getResolution( const DEVMODE& settings );
	static void setScreenResolution( CVector2<int> resolution );

	CVector2<int> findWindowTopLeft( CVector2<int> tlOffset, TWindowOriginPoint origin, CVector2<int> desktopSize, CVector2<int> windowSize );
	void setWindowedModeRect( RECT windowRect, UINT flags );
	void updateWindowSize();
	void setWindowSize( int sizeX, int sizeY );

	// Copying is prohibited.
	CGlWindow( const CGlWindow& ) = delete;
	void operator=( const CGlWindow& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template<class Dispatcher>
void CGlWindow::Create( CWindowClass<Dispatcher>& windowClass, CGlWindowSettings initialSettings )
{
	windowClass.GetDispatcher().AcceptNewWindow( *this );
	doCreate( windowClass.GetClassName(), initialSettings );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

