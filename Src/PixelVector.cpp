#include <common.h>
#pragma hdrstop

#include <PixelVector.h>
#include <ClipVector.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CClipVector CPixelVector::FindClipPos( const CMatrix3<float>& pixelToClip ) const
{
	return CClipVector( PointTransform( pixelToClip, pos ) );
}

CClipVector CPixelVector::FindClipSize( const CMatrix3<float>& pixelToClip ) const
{
	return CClipVector( VecTransform( pixelToClip, pos ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
