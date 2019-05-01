#pragma once
// Classes for operating on the openGl window.

#include <GlWindow.h>

namespace Gin {

class CPixelRect;
class CClipRect;
//////////////////////////////////////////////////////////////////////////

// Switcher for the GL window's viewport value.
class GINAPI CViewportSwitcher {
public:
	explicit CViewportSwitcher( CVector2<int> newSize );
	CViewportSwitcher( CVector2<int> newOffset, CVector2<int> newSize );
	CViewportSwitcher( CAARect<int> viewportRect );
	~CViewportSwitcher();

	static CVector2<int> GetViewportSize();

	static void SetBaseViewport( CVector2<int> baseOffset, CVector2<int> baseSize );

private:
	CVector2<int> prevSize;
	CVector2<int> prevOffset;

	static void updateTransformations( CVector2<int> size );
};

//////////////////////////////////////////////////////////////////////////

// Switcher for the scissor test.
class GINAPI CScissorsSwitcher {
public:
	explicit CScissorsSwitcher( CClipRect clipRect, const CMatrix3<float>& clipToPixel );
	explicit CScissorsSwitcher( CPixelRect pixelRect );
	explicit CScissorsSwitcher( CAARect<int> windowRect );
	~CScissorsSwitcher();

private:
	static CAARect<int> getWindowRect( CClipRect clipRect, const CMatrix3<float>& clipToPixel );
	static CAARect<int> getWindowRect( CPixelRect pixelRect );
};

// Gin defines two coordinate spaces.
// Clip space is the same as OpengGL clip space.
// Pixel space is a 2D space that starts in the bottom left viewport corner. 1 unit == 1 pixel.
namespace Coordinates {

//////////////////////////////////////////////////////////////////////////

// Clip and pixel space transformations.
GINAPI const CMatrix3<float>& ClipToPixel();
GINAPI const CMatrix3<float>& PixelToClip();

CMatrix4<float> GINAPI FindPerspectiveMatrix( float zNear, float zFar, float frustumScale );

}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

