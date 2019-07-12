#include <common.h>
#pragma hdrstop

#include <WinGdiRenderMechanism.h>
#include <State.h>
#include <DrawEnums.h>
#include <GlWindow.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CWinGdiRenderMechanism::CWinGdiRenderMechanism()
{
}

CWinGdiRenderMechanism::~CWinGdiRenderMechanism()
{
	::DeleteObject( backgroundBrush );
	::DeleteObject( clipRegion );
	::DeleteObject( backBufferBitmap );
	::DeleteDC( backBufferDc );
}

void CWinGdiRenderMechanism::AttachNewWindow( const CGlWindow& newWindow )
{
	windowDc = newWindow.GetDeviceContext();
	backBufferDc = ::CreateCompatibleDC( windowDc );
	const auto size = newWindow.WindowSize();
	backBufferBitmap = ::CreateCompatibleBitmap( windowDc, size.X(), size.Y() );
	backgroundBrush = ::CreateSolidBrush( RGB( 0, 0, 0 ) );
	clipRegion = ::CreateRectRgn( 0, 0, 1, 1 );
}

void CWinGdiRenderMechanism::ActivateWindowTarget()
{
}

void CWinGdiRenderMechanism::SetBackgroundColor( CColor newValue )
{
	::DeleteObject( backgroundBrush );
	backgroundBrush = ::CreateSolidBrush( RGB( newValue.R, newValue.G, newValue.B ) );
}

void CWinGdiRenderMechanism::OnWindowResize( CVector2<int> newSize )
{
	assert( backBufferBitmap != nullptr );
	::DeleteObject( backBufferBitmap );
	backBufferBitmap = ::CreateCompatibleBitmap( windowDc, newSize.X(), newSize.Y() );
	windowSize = newSize;
}

void CWinGdiRenderMechanism::OnDraw( const IState& ) const
{
}

void CWinGdiRenderMechanism::OnPostDraw() const
{
}

LRESULT CWinGdiRenderMechanism::OnPaintMessage( HWND window, WPARAM, LPARAM, const IState& currentState ) const
{
	assert( backBufferBitmap != nullptr );
	PAINTSTRUCT ps;
	::GetUpdateRgn( window, clipRegion, false );
	const auto paintDc = ::BeginPaint( window, &ps );
	const auto paintRect = ps.rcPaint;

	const auto defaultBitmap = ::SelectObject( backBufferDc, backBufferBitmap );
	::SelectClipRgn( backBufferDc, clipRegion );
	::FillRect( backBufferDc, &paintRect, backgroundBrush );
	currentState.Draw( CWinGdiRenderParameters( backBufferDc ) );

	::BitBlt( paintDc, paintRect.left, paintRect.top, paintRect.right - paintRect.left, paintRect.bottom - paintRect.top, backBufferDc, paintRect.left, paintRect.top, SRCCOPY );
	::EndPaint( window, &ps );

	::SelectObject( backBufferDc, defaultBitmap );
	return 0;
}

CArray<BYTE> CWinGdiRenderMechanism::ReadScreenBuffer( TTexelFormat format, CVector2<int>& screenSize ) const
{
	assert( backBufferBitmap != nullptr );
	BITMAPINFO info;
	::memset( &info, 0, sizeof( info ) );
	info.bmiHeader.biSize = sizeof( info.bmiHeader );
	const auto getResult = ::GetDIBits( backBufferDc, backBufferBitmap, 0, windowSize.Y(), nullptr, &info, DIB_RGB_COLORS );
	checkLastError( getResult != 0 );
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biBitCount = 24;

	CArray<BYTE> result;
	const auto pixelSize = Ceil( info.bmiHeader.biBitCount, CHAR_BIT );
	const int scanLineWidth = CeilTo( info.bmiHeader.biWidth * pixelSize, sizeof( DWORD ) );
	result.IncreaseSizeNoInitialize( windowSize.Y() * scanLineWidth );
	::GetDIBits( backBufferDc, backBufferBitmap, 0, windowSize.Y(), result.Ptr(), &info, DIB_RGB_COLORS );

	if( format == TF_RGB ) {
		swapRedBlue( result, scanLineWidth, windowSize.Y(), pixelSize );
	} else {
		assert( format == TF_BGR );
	}

	screenSize = windowSize;
	return result;
}

void CWinGdiRenderMechanism::swapRedBlue( CArrayBuffer<BYTE> result, int scanLineWidth, int lineCount, int pixelSize ) const
{
	for( int y = 0; y < lineCount; y++ ) {
		const int lineStartOffset = y * scanLineWidth;
		for( int i = 0; i < scanLineWidth - pixelSize; i += pixelSize ) {
			const int pixelStart = i + lineStartOffset;
			swap( result[pixelStart], result[pixelStart + 2] );
		}
	}
}

LRESULT CWinGdiRenderMechanism::OnEraseBackground( HWND, WPARAM, LPARAM )
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
