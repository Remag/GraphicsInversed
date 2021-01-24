#include <common.h>
#pragma hdrstop

#include <GlWindowUtils.h>
#include <GinGlobals.h>
#include <PixelRect.h>
#include <ClipRect.h>
#include <GinError.h>

namespace Gin {

static CVector2<int> currentViewportSize;
static CVector2<int> currentViewportOffset;
static CMatrix3<float> pixelToClipTransformation( 1.0f );
static CMatrix3<float> clipToPixelTransformation( 1.0f );
//////////////////////////////////////////////////////////////////////////

CViewportSwitcher::CViewportSwitcher( CVector2<int> newSize ) :
	CViewportSwitcher( CVector2<int>{}, newSize )
{
}

CViewportSwitcher::CViewportSwitcher( CVector2<int> newOffset, CVector2<int> newSize ) :
	prevSize( currentViewportSize ),
	prevOffset( currentViewportOffset )
{
	SetBaseViewport( newOffset, newSize );
}

CViewportSwitcher::CViewportSwitcher( CAARect<int> viewportRect ) :
	prevSize( currentViewportSize ),
	prevOffset( currentViewportOffset )
{
	CVector2<int> rectSize{ viewportRect.Width(), viewportRect.Height() };
	SetBaseViewport( viewportRect.BottomLeft(), rectSize );
}

CViewportSwitcher::~CViewportSwitcher()
{
	SetBaseViewport( prevOffset, prevSize );
}

CVector2<int> CViewportSwitcher::GetViewportSize()
{
	return currentViewportSize;
}

void CViewportSwitcher::SetBaseViewport( CVector2<int> baseOffset, CVector2<int> baseSize )
{
	currentViewportSize = baseSize;
	currentViewportOffset = baseOffset;
	gl::Viewport( baseOffset.X(), baseOffset.Y(), baseSize.X(), baseSize.Y() );
	CheckGlError();
	updateTransformations( baseSize );
}

void CViewportSwitcher::updateTransformations( CVector2<int> viewSize )
{
	assert( viewSize.X() != 0 && viewSize.Y() != 0 );
	const CVector2<float> viewScale{ 2.0f / viewSize.X(), 2.0f / viewSize.Y() };
	pixelToClipTransformation( 0, 0 ) = viewScale.X();
	pixelToClipTransformation( 1, 1 ) = viewScale.Y();
	clipToPixelTransformation( 0, 0 ) = 1.0f / viewScale.X();
	clipToPixelTransformation( 1, 1 ) = 1.0f / viewScale.Y();

	pixelToClipTransformation( 2, 0 ) = -1.0f;
	pixelToClipTransformation( 2, 1 ) = -1.0f;
	clipToPixelTransformation( 2, 0 ) = viewSize.X() * 0.5f;
	clipToPixelTransformation( 2, 1 ) = viewSize.Y() * 0.5f;
}

//////////////////////////////////////////////////////////////////////////

CScissorsSwitcher::CScissorsSwitcher( CAARect<int> rect )
{
	const auto width = rect.Width();
	const auto height = rect.Height();
	assert( width >= 0 );
	assert( height >= 0 );
	gl::Enable( gl::SCISSOR_TEST );
	gl::Scissor( rect.Left(), rect.Bottom(), width, height );
}

CScissorsSwitcher::CScissorsSwitcher( CClipRect clipRect, const CMatrix3<float>& clipToPixel ) :
	CScissorsSwitcher( getWindowRect( clipRect, clipToPixel ) )
{
}

CScissorsSwitcher::CScissorsSwitcher( CPixelRect pixelRect ) :
	CScissorsSwitcher( getWindowRect( pixelRect ) )
{
}

CAARect<int> CScissorsSwitcher::getWindowRect( CClipRect clipRect, const CMatrix3<float>& clipToPixel )
{
	return getWindowRect( clipRect.FindPixelRect( clipToPixel ) );
}

CAARect<int> CScissorsSwitcher::getWindowRect( CPixelRect pixelRect )
{
	const auto blOffset = currentViewportOffset;
	return pixelRect.GetGridRect().GetOffsetRect( blOffset );
}

CScissorsSwitcher::~CScissorsSwitcher()
{
	gl::Disable( gl::SCISSOR_TEST );
}

//////////////////////////////////////////////////////////////////////////

CMatrix3<float> Gin::Coordinates::ClipToPixel()
{
	return clipToPixelTransformation;
}

CMatrix3<float> Gin::Coordinates::PixelToClip()
{
	return pixelToClipTransformation;
}

CMatrix4<float> Gin::Coordinates::FindPerspectiveMatrix( float zNear, float zFar, float frustumScale )
{
	CMatrix4<float> result;
	result( 0, 0 ) = frustumScale;
	result( 1, 1 ) = frustumScale;
	const float zDelta = zNear - zFar;
	result( 2, 2 ) = ( zNear + zFar ) / zDelta;
	result( 3, 2 ) = 2 * zNear * zFar / zDelta;
	result( 2, 3 ) = -1.0f;

	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
