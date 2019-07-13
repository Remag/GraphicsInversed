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

COpenGlRenderMechanism::COpenGlRenderMechanism( CGlContextManager& _glContextManager ) :
	glContextManager( _glContextManager )
{
	backgroundBrush = ::CreateSolidBrush( RGB( 0, 0, 0 ) );
}

COpenGlRenderMechanism::~COpenGlRenderMechanism()
{
	::DeleteObject( backgroundBrush );
}

void COpenGlRenderMechanism::AttachNewWindow( const CGlWindow& newWindow )
{
	assert( newWindow.IsCreated() );
	targetWindow = &newWindow;
	OnWindowResize( newWindow.WindowSize() );
}

void COpenGlRenderMechanism::ActivateWindowTarget()
{
	glContextManager.SetContextTarget( targetWindow->GetDeviceContext() );
}

void COpenGlRenderMechanism::SetBackgroundColor( CColor newValue )
{
	backgroundColor = newValue;
	::DeleteObject( backgroundBrush );
	backgroundBrush = ::CreateSolidBrush( RGB( newValue.R, newValue.G, newValue.B ) );
}

void COpenGlRenderMechanism::OnWindowResize( CVector2<int> )
{
}

LRESULT COpenGlRenderMechanism::OnEraseBackground( HWND, WPARAM wParam, LPARAM )
{	
	RECT bgRect{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };
	HDC bgDC = reinterpret_cast<HDC>( wParam );
	::FillRect( bgDC, &bgRect, backgroundBrush );
	return 1;
}

void COpenGlRenderMechanism::OnDraw( const IState& currentState ) const
{
	CViewportSwitcher::SetBaseViewport( CVector2<int>{}, targetWindow->WindowSize() );
	// Set the clear values.
	gl::ClearColor( backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha() );
	gl::ClearDepth( glContextManager.GetDepthZFar() );

	gl::Enable( gl::DEPTH_TEST );
	gl::Disable( gl::BLEND );
	// Clear the buffer.
	gl::Clear( gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT );

	currentState.Draw( COpenGlRenderParameters( *targetWindow ) );
}

void COpenGlRenderMechanism::OnPostDraw() const
{
	::SwapBuffers( targetWindow->GetDeviceContext() );
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
