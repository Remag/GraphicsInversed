#include <common.h>
#pragma hdrstop

#include <GlWindow.h>
#include <GinError.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CGlWindow::~CGlWindow()
{
	if( windowDC != nullptr ) {
		::ReleaseDC( windowHandle, windowDC );
	}
}

void CGlWindow::setRenderMechanism( IRenderMechanism& newValue )
{
	assert( renderMechanism == nullptr );
	renderMechanism = &newValue;
}

void CGlWindow::doCreate( CUnicodeView className, CGlWindowSettings initialSettings )
{
	assert( windowHandle == nullptr );

	windowClassName = className;
	windowStyle = initialSettings.WindowStyle;

	// Find screen resolution.
	fullscreenResolution = initialScreenResolution = findDefaultResolution();

	RECT desktopRect;
	::GetWindowRect( ::GetDesktopWindow(), &desktopRect );
	CVector2<int> desktopSize{ static_cast<int>( desktopRect.right - desktopRect.left ), static_cast<int>( desktopRect.bottom - desktopRect.top ) };

	const auto windowStartSize = initialSettings.WindowSize;
	auto windowTL = findWindowTopLeft( initialSettings.WindowPosition, initialSettings.Origin, desktopSize, windowStartSize );
	windowTL.X() = Clamp( windowTL.X(), 0, desktopSize.X() );
	windowTL.Y() = Clamp( windowTL.Y(), 0, desktopSize.Y() );
	// Initial settings specify the client area size. Find the size that is needed for the CreateWindow function.
	windowedModeRect = RECT{ windowTL.X(), windowTL.Y(), windowTL.X() + windowStartSize.X(), windowTL.Y() + windowStartSize.Y() };
	::AdjustWindowRect( &windowedModeRect, windowStyle, FALSE );

	const auto creationStyle = initialSettings.IsFullscreen ? ( windowStyle & ~WS_OVERLAPPEDWINDOW ) : windowStyle;
	const auto creationRect = initialSettings.IsFullscreen ? desktopRect : windowedModeRect;
	const auto newHandle = ::CreateWindowEx( 0, windowClassName.Ptr(), UnicodeStr( Relib::GetAppTitle() ).Ptr(), creationStyle, 
		creationRect.left, creationRect.top, creationRect.right - creationRect.left, creationRect.bottom - creationRect.top, 0, 0, ::GetModuleHandle( 0 ), 0 );
	windowHandle = newHandle;
	windowDC = ::GetDC( windowHandle );

	if( initialSettings.IsFullscreen ) {
		setWindowSize( desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top );
	} else {
		RECT windowClientRect;
		::GetClientRect( windowHandle, &windowClientRect );
		setWindowSize( windowClientRect.right - windowClientRect.left, windowClientRect.bottom - windowClientRect.top );
	}
}

CVector2<int> CGlWindow::findDefaultResolution() const
{
	// Find screen resolution.
	DEVMODE currentSettings;
	::ZeroMemory( &currentSettings, sizeof( currentSettings ) );
	currentSettings.dmSize = sizeof( currentSettings );
	currentSettings.dmDriverExtra = 0;

	if( ::EnumDisplaySettings( nullptr, ENUM_REGISTRY_SETTINGS, &currentSettings ) == 0 ) {
		// Apparently, the previous call can fail on some devices for an unknown reason.
		// Use ENUM_CURRENT_SETTINGS as a fall back and revert to a hard coded resolution if that fails as well.
		Log::Warning( "Failed to fetch registry display settings" );
		::ZeroMemory( &currentSettings, sizeof( currentSettings ) );
		currentSettings.dmSize = sizeof( currentSettings );
		currentSettings.dmDriverExtra = 0;
		if( ::EnumDisplaySettings( nullptr, ENUM_CURRENT_SETTINGS, &currentSettings ) == 0 ) {
			Log::Warning( "Failed to fetch current display settings" );
			return CVector2<int>( 1920, 1080 );
		}
	}
	return getResolution( currentSettings );
}

CVector2<int> CGlWindow::findWindowTopLeft( CVector2<int> originOffset, TWindowOriginPoint origin, CVector2<int> desktopSize, CVector2<int> size )
{
	staticAssert( WOP_EnumCount == 2 );
	if( origin == WOP_TopLeft ) {
		return originOffset;
	} else {
		return originOffset + ( desktopSize - size ) / 2;
	}
}

void CGlWindow::Show( bool isShown )
{
	::ShowWindow( windowHandle, isShown ? SW_SHOW : SW_HIDE );
}

void CGlWindow::SendCloseCommand()
{
	if( windowHandle != nullptr ) {
		::PostMessage( windowHandle, WM_CLOSE, 0, 0 );
	}
}

void CGlWindow::ForceDestroy()
{
	::DestroyWindow( windowHandle );
}

CVector2<int> CGlWindow::GetScreenResolution() const
{
	return isFullscreen ? fullscreenResolution : initialScreenResolution;
}

int CGlWindow::GetSupportedResolutions( CVector2<int> minResolution, CArray<CVector2<int>>& result ) const
{
	DEVMODE currentSettings;
	::ZeroMemory( &currentSettings, sizeof( currentSettings ) );
	currentSettings.dmSize = sizeof( currentSettings );
	currentSettings.dmDriverExtra = 0;
	const bool enumSuccess = ::EnumDisplaySettings( 0, ENUM_CURRENT_SETTINGS, &currentSettings ) != 0 ;
	assert( enumSuccess );
	const CVector2<int> currentResolution = getResolution( currentSettings );

	DEVMODE settings;
	::ZeroMemory( &settings, sizeof( settings ) );
	settings.dmSize = sizeof( settings );
	settings.dmDriverExtra = 0;
	int currentResPos = NotFound;
	for( int modePos = 0; ::EnumDisplaySettings( 0, modePos, &settings ) != 0; modePos++ ) {
		if( !compareDisplaySettings( currentSettings, settings ) ) {
			// Different display mode, doesn't need to be in the resolution list.
			continue;
		}
		// Don't add duplicates and small resolutions.
		const CVector2<int> newResolution = getResolution( settings );
		if( newResolution.X() < minResolution.X() || newResolution.Y() < minResolution.Y() || SearchPos( result, newResolution ) != NotFound ) {
			continue;
		}
		if( newResolution == currentResolution ) {
			// Found current resolution, remember it.
			assert( currentResPos == NotFound );
			currentResPos = result.Size();
		}
		result.Add( newResolution );
	}
	
	if( currentResPos == NotFound ) {
		// Current resolution is not found in the system supported list, add it manually.
		currentResPos = result.Size();
		result.Add( currentResolution );
	}
	return currentResPos;
}

CVector2<int> CGlWindow::getResolution( const DEVMODE& settings )
{
	assert( settings.dmPelsWidth > 0 && settings.dmPelsHeight > 0 );
	return CVector2<int>( numeric_cast<int>( settings.dmPelsWidth ), numeric_cast<int>( settings.dmPelsHeight ) );
}

// Compare general display settings.
bool CGlWindow::compareDisplaySettings( const DEVMODE& left, const DEVMODE& right )
{
	return left.dmBitsPerPel == right.dmBitsPerPel && left.dmDisplayOrientation == right.dmDisplayOrientation;
}

void CGlWindow::updateWindowSize()
{
	RECT windowRect;
	::GetClientRect( windowHandle, &windowRect );
	setWindowSize( windowRect.right, windowRect.bottom );
}

void CGlWindow::setWindowSize( int sizeX, int sizeY )
{
	windowSize.X() = max( 1, sizeX );
	windowSize.Y() = max( 1, sizeY );
}

void CGlWindow::SetFullscreenResolution( CVector2<int> newValue )
{
	fullscreenResolution = newValue;
	if( isFullscreen ) {
		setScreenResolution( newValue );
		RECT windowRect;
		::GetWindowRect( GetDesktopWindow(), &windowRect );
		setWindowSize( windowRect.right - windowRect.left, windowRect.bottom - windowRect.top );
		setWindowedModeRect( windowRect, SWP_NOZORDER );
	}
}

// Change an actual screen resolution in fullscreen mode.
void CGlWindow::setScreenResolution( CVector2<int> resolution )
{
	DEVMODE newSettings;
	::ZeroMemory( &newSettings, sizeof( newSettings ) );
	newSettings.dmSize = sizeof( newSettings );
	SetFlags( newSettings.dmFields, DM_PELSWIDTH | DM_PELSHEIGHT );
	newSettings.dmPelsWidth = numeric_cast<DWORD>( resolution.X() );
	newSettings.dmPelsHeight = numeric_cast<DWORD>( resolution.Y() );

	const int changeResult = ::ChangeDisplaySettings( &newSettings, CDS_FULLSCREEN );
	const bool changeSuccess = changeResult == DISP_CHANGE_SUCCESSFUL;
	assert( changeSuccess );
}

void CGlWindow::SetFullscreen( bool isSet )
{
	if( isFullscreen == isSet ) {
		return;
	}
	if( isSet ) {
		isFullscreen = isSet;
		setScreenResolution( fullscreenResolution );
		RECT desktopRect;
		::GetWindowRect( ::GetDesktopWindow(), &desktopRect );
		::SetWindowLong( windowHandle, GWL_STYLE, ( windowStyle & ~WS_OVERLAPPEDWINDOW ) );

		setWindowSize( desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top );
		setWindowedModeRect( desktopRect, SWP_FRAMECHANGED | SWP_NOZORDER );
	} else {
		::ChangeDisplaySettings( nullptr, CDS_FULLSCREEN );
		::SetWindowLong( windowHandle, GWL_STYLE, windowStyle );
		setWindowedModeRect( windowedModeRect, SWP_FRAMECHANGED | SWP_NOZORDER );
		updateWindowSize();
		isFullscreen = isSet;
	}
}

void CGlWindow::SetMaximized( bool isSet )
{
	::ShowWindow( windowHandle, isSet ? SW_MAXIMIZE : SW_SHOWNORMAL );
}

void CGlWindow::SetWindowStyle( DWORD newStyle )
{
	::SetWindowLong( windowHandle, GWL_STYLE, newStyle );
	::SetWindowPos( windowHandle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
}

void CGlWindow::SetWindowRect( CAARect<int> rect )
{
	windowedModeRect.left = rect.Left();
	windowedModeRect.top = rect.Bottom();
	windowedModeRect.right = rect.Right();
	windowedModeRect.bottom = rect.Top();
	if( !isFullscreen ) {
		setWindowedModeRect( windowedModeRect, SWP_NOZORDER );
		updateWindowSize();
	}
}

void CGlWindow::SetClientRect( CAARect<int> rect )
{
	RECT desiredRect;
	desiredRect.left = rect.Left();
	desiredRect.top = rect.Bottom();
	desiredRect.right = rect.Right();
	desiredRect.bottom = rect.Top();
	::AdjustWindowRectEx( &desiredRect, windowStyle, FALSE, 0 );
	windowedModeRect = desiredRect;
	if( !isFullscreen ) {
		setWindowedModeRect( windowedModeRect, SWP_NOZORDER );
		updateWindowSize();
	}
}

CAARect<int> CGlWindow::GetWindowModeRect() const
{
	CAARect<int> result{ windowedModeRect.left, windowedModeRect.bottom, windowedModeRect.right, windowedModeRect.top };
	return result;
}

void CGlWindow::setWindowedModeRect( RECT windowRect, UINT flags )
{
	const int width = windowRect.right - windowRect.left;
	const int height = windowRect.bottom - windowRect.top;
	::SetWindowPos( windowHandle, HWND_TOP, windowRect.left, windowRect.top, width, height, flags );
}

void CGlWindow::RefreshCachedValues()
{
	// Setting a fullscreen mode involves calling Win API functions that send a WM_SIZE and in turn call this method.
	if( isFullscreen ) {
		return;
	}

	RECT windowRect;
	::GetClientRect( windowHandle, &windowRect );
	setWindowSize( windowRect.right, windowRect.bottom );

	::GetWindowRect( windowHandle, &windowRect );
	windowedModeRect = windowRect;
}

void CGlWindow::UpdateFromActiveResize( RECT dragRect )
{
	if( isFullscreen ) {
		return;
	}

	const auto widthDelta = ( dragRect.right - dragRect.left ) - ( windowedModeRect.right - windowedModeRect.left );
	const auto heightDelta = ( dragRect.bottom - dragRect.top ) - ( windowedModeRect.bottom - windowedModeRect.top );
	setWindowSize( windowSize.X() + widthDelta, windowSize.Y() + heightDelta );
	windowedModeRect = dragRect;
}

void CGlWindow::SetActivation( bool isSet )
{
	if( !isFullscreen ) {
		// Ignore window activation in windowed mode.
		return;
	}

	// Restore initial resolution.
	if( isSet ) {
		if( initialScreenResolution != fullscreenResolution ) {
			setScreenResolution( fullscreenResolution );
		}
	} else if( initialScreenResolution != fullscreenResolution ) {
		// Restore initial system settings.
		// This call can actually fail in a weird case, when the activation is happening after the user has pressed ctrl-alt-delete.
		// Hopefully, in this case, the activation is triggered twice and the second call doesn't fail.
		::ChangeDisplaySettings( nullptr, CDS_FULLSCREEN );
	}
}

void CGlWindow::BringToForeground()
{
	::SetForegroundWindow( windowHandle );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.




