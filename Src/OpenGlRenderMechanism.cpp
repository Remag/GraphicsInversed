#include <common.h>
#pragma hdrstop

#include <OpenGlRenderMechanism.h>
#include <GlContextManager.h>
#include <GlWindowUtils.h>
#include <DrawEnums.h>
#include <State.h>
#include <Framebuffer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

COpenGlRenderMechanism::COpenGlRenderMechanism( const CGlWindow& window, CGlContextManager& _glContextManager ) :
	glContextManager( _glContextManager )
{
	backgroundBrush = ::CreateSolidBrush( RGB( 0, 0, 0 ) );
	assert( window.IsCreated() );
	OnWindowResize( window.WindowSize() );
}

COpenGlRenderMechanism::~COpenGlRenderMechanism()
{
	::DeleteObject( backgroundBrush );
}

void COpenGlRenderMechanism::ActivateWindowTarget( const CGlWindow& newTarget )
{
	glContextManager.SetContextTarget( newTarget.GetDeviceContext() );
}

void COpenGlRenderMechanism::SetBackgroundColor( CColor newValue )
{
	backgroundColor = newValue;
	::DeleteObject( backgroundBrush );
	backgroundBrush = ::CreateSolidBrush( RGB( newValue.R, newValue.G, newValue.B ) );
}

void COpenGlRenderMechanism::OnWindowResize( CVector2<int> newSize )
{
	newSize.X() = max( 1, newSize.X() );
	newSize.Y() = max( 1, newSize.Y() );
	CViewportSwitcher::SetBaseViewport( CVector2<int>{}, newSize );
}

LRESULT COpenGlRenderMechanism::OnEraseBackground( HWND, WPARAM wParam, LPARAM )
{	
	RECT bgRect{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };
	HDC bgDC = reinterpret_cast<HDC>( wParam );
	::FillRect( bgDC, &bgRect, backgroundBrush );
	return 1;
}

void COpenGlRenderMechanism::OnDraw( const IState& currentState, const CGlWindow& target ) const
{
	// Set the clear values.
	gl::ClearColor( backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha() );
	gl::ClearDepth( glContextManager.GetDepthZFar() );

	gl::Enable( gl::DEPTH_TEST );
	gl::Disable( gl::BLEND );
	// Clear the buffer.
	gl::Clear( gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT );

	currentState.Draw( COpenGlRenderParameters( target ) );
}

void COpenGlRenderMechanism::OnPostDraw( const CGlWindow& target ) const
{
	::SwapBuffers( target.GetDeviceContext() );
}

LRESULT COpenGlRenderMechanism::OnPaintMessage( HWND window, WPARAM wParam, LPARAM lParam, const IState& ) const
{
	return ::DefWindowProc( window, WM_PAINT, wParam, lParam );
}

CArray<BYTE> COpenGlRenderMechanism::ReadScreenBuffer( TTexelFormat format, CVector2<int>& bufferSize ) const
{
	assert( CFramebufferSwitcher::GetReadTarget() == 0 );
	const auto viewportSize = CViewportSwitcher::GetViewportSize();
	const int rowSize = CeilTo( 3 * viewportSize.X(), 4 );
	const int dataSize = rowSize * viewportSize.Y();

	CArray<BYTE> data;
	data.IncreaseSizeNoInitialize( dataSize );
	gl::PixelStorei( AT_Pack, 4 );
	gl::ReadPixels( 0, 0, viewportSize.X(), viewportSize.Y(), format, TDT_UnsignedByte, data.Ptr() );
	gl::PixelStorei( AT_Pack, 1 );
	
	bufferSize = viewportSize;
	return data;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
