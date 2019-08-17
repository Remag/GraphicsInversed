#pragma once

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// General window class singleton wrapper. Registers a window class on creation.
// Owns a dispatcher that handles window messages.
template <class Dispatcher>
class CWindowClass : public CSingleton<CWindowClass<Dispatcher>> {
public:
	explicit CWindowClass( CUnicodeView className, HCURSOR cursor, HICON windowIcon, Dispatcher dispatcher );
	~CWindowClass();

	Dispatcher& GetDispatcher()
		{ return dispatcher; }
	CUnicodeView GetClassName() const
		{ return className; }

private:
	// Class name.
	CUnicodeString className;
	// Window message handler.
	Dispatcher dispatcher;
	// Should the mouse leave function be tracked.
	bool startLeaveTracking;

	static CWindowClass<Dispatcher>& getInstance()
		{ return *CSingleton<CWindowClass<Dispatcher>>::GetInstance(); }
	static Dispatcher& getDispatcher()
		{ return getInstance().dispatcher; }
	static LRESULT WINAPI windowProcedure( HWND window, UINT message, WPARAM wParam, LPARAM lParam );

	static void tryStartMouseTracking();
	static void startMouseTracking();
};

//////////////////////////////////////////////////////////////////////////

template<class Dispatcher>
CWindowClass<Dispatcher>::CWindowClass( CUnicodeView _className, HCURSOR cursor, HICON windowIcon, Dispatcher _dispatcher ) :
	className( _className ),
	dispatcher( move( _dispatcher ) ),
	startLeaveTracking( dispatcher.ShouldTrackMouseLeave() )
{
	WNDCLASSEX wndclass;
	::ZeroMemory( &wndclass, sizeof( wndclass ) );
	wndclass.cbSize = sizeof( wndclass );
	wndclass.hbrBackground = ( HBRUSH )::GetStockObject( BLACK_BRUSH );
	wndclass.hCursor = cursor;
	wndclass.hInstance = ::GetModuleHandle( 0 );
	wndclass.lpfnWndProc = windowProcedure;
	wndclass.hIcon =  windowIcon;
	wndclass.lpszClassName = className.Ptr();
	wndclass.style = CS_OWNDC | CS_DBLCLKS;
	ATOM result = ::RegisterClassEx( &wndclass );
	checkLastError( result != 0 );
}

template<class Dispatcher>
CWindowClass<Dispatcher>::~CWindowClass()
{
	::UnregisterClass( className.Ptr(), GetModuleHandle( 0 ) );
}

template<class Dispatcher>
LRESULT WINAPI CWindowClass<Dispatcher>::windowProcedure( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) {
		case WM_ERASEBKGND:
			return getDispatcher().OnEraseBackground( wnd, wParam, lParam );
		case WM_DESTROY:
			getDispatcher().OnWindowDestroy( wnd );
			break;
		case WM_CLOSE:
			getDispatcher().OnWindowClose( wnd );
			break;
		case WM_ACTIVATE:
			getDispatcher().OnWindowActivation( wnd, wParam );
			return ::DefWindowProc( wnd, msg, wParam, lParam );
		case WM_SIZE:
			getDispatcher().OnWindowResize( wnd, wParam, LOWORD( lParam ), HIWORD( lParam ) );
			break;
		case WM_SIZING:
			getDispatcher().OnWindowActiveResize( wnd );
			break;
		case WM_MOVE:
			getDispatcher().OnWindowMove( wnd );
			break;
		case WM_PAINT:
			return getDispatcher().OnPaint( wnd, wParam, lParam );
		case WM_CHAR:
			getDispatcher().OnChar( wnd, wParam );
			break;
		case WM_INPUT:
			getDispatcher().OnInput( wnd, lParam );
			// The application must call DefWindowProc so the system can perform cleanup.
			return ::DefWindowProc( wnd, msg, wParam, lParam );
		case WM_MOUSEWHEEL:
			getDispatcher().OnMouseWheel( wnd, GET_WHEEL_DELTA_WPARAM( wParam ) );
			break;
		case WM_MOUSEMOVE:
			getDispatcher().OnMouseMove( wnd, static_cast<int>( (short)LOWORD( lParam ) ), static_cast<int>( (short)HIWORD( lParam ) ) );
			tryStartMouseTracking();
			break;
		case WM_MOUSELEAVE:
			getDispatcher().OnMouseLeave( wnd );
			getInstance().startLeaveTracking = true;
			break;
		case WM_LBUTTONDOWN:
			getDispatcher().OnMousePress( wnd, VK_LBUTTON, true );
			break;
		case WM_LBUTTONUP:
			getDispatcher().OnMousePress( wnd, VK_LBUTTON, false );
			break;
		case WM_RBUTTONDOWN:
			getDispatcher().OnMousePress( wnd, VK_RBUTTON, true );
			break;
		case WM_RBUTTONUP:
			getDispatcher().OnMousePress( wnd, VK_RBUTTON, false );
			break;
		case WM_MBUTTONDOWN:
			getDispatcher().OnMousePress( wnd, VK_MBUTTON, true );
			break;
		case WM_MBUTTONUP:
			getDispatcher().OnMousePress( wnd, VK_MBUTTON, false );
			break;
		case WM_XBUTTONDOWN:
			getDispatcher().OnMousePress( wnd, GET_XBUTTON_WPARAM( wParam ) == 1 ? VK_XBUTTON1 : VK_XBUTTON2, true );
			break;
		case WM_XBUTTONUP:
			getDispatcher().OnMousePress( wnd, GET_XBUTTON_WPARAM( wParam ) == 1 ? VK_XBUTTON1 : VK_XBUTTON2, false );
			break;
		case WM_LBUTTONDBLCLK:
			getDispatcher().OnMousePress( wnd, VK_LBUTTON, true );
			getDispatcher().OnMousePress( wnd, 0x3A, true );
			getDispatcher().OnMousePress( wnd, 0x3A, false );
			break;
		case WM_RBUTTONDBLCLK:
			getDispatcher().OnMousePress( wnd, VK_RBUTTON, true );
			getDispatcher().OnMousePress( wnd, 0x3B, true );
			getDispatcher().OnMousePress( wnd, 0x3B, false );
			break;
		case WM_MBUTTONDBLCLK:
			getDispatcher().OnMousePress( wnd, VK_MBUTTON, true );
			getDispatcher().OnMousePress( wnd, 0x3C, true );
			getDispatcher().OnMousePress( wnd, 0x3C, false );
			break;
		default:
			return ::DefWindowProc( wnd, msg, wParam, lParam );
	}
	return 0;
}

template<class Dispatcher>
void CWindowClass<Dispatcher>::tryStartMouseTracking()
{
	if( getInstance().startLeaveTracking ) {
		startMouseTracking();
	}
}

template<class Dispatcher>
void CWindowClass<Dispatcher>::startMouseTracking()
{
	getInstance().startLeaveTracking = false;

	TRACKMOUSEEVENT e;
	e.cbSize = sizeof( e );
	e.dwFlags = TME_LEAVE;
	e.dwHoverTime = 0;
	e.hwndTrack = GetMainWindow().Handle();
	::TrackMouseEvent( &e );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

