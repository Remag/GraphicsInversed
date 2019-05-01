#include <common.h>
#pragma hdrstop

#include <PixelRect.h>
#include <ClipRect.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CClipRect CPixelRect::FindScreenRect( const CMatrix3<float>& pixelToClip ) const
{
	const CClipVector bottomLeft = BottomLeft().FindClipPos( pixelToClip );
	const CClipVector topRight = TopRight().FindClipPos( pixelToClip );
	return CClipRect( bottomLeft, topRight );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
