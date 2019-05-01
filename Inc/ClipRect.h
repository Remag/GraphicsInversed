#pragma once
#include <ClipVector.h>
#include <PixelRect.h>

namespace Gin {

class CClipVector;
class CPixelRect;
//////////////////////////////////////////////////////////////////////////

// Axis aligned rectangle in clip coordinates.
class CClipRect {
public:
	CClipRect() = default;
	CClipRect( float left, float top, float right, float bottom ) : rect( left, top, right, bottom ) {}
	CClipRect( CClipVector bottomLeft, CClipVector topRight );
	explicit CClipRect( const CAARect<float> _rect ) : rect( _rect ) {}

	CPixelRect FindPixelRect( const CMatrix3<float>& clipToPixel) const;

	bool IsEmpty() const
		{ return rect.IsEmpty(); }

	// Common rectangle methods.
	float& Left()
		{ return rect.Left(); }
	float Left() const
		{ return rect.Left(); }
	float& Top()
		{ return rect.Top(); }
	float Top() const
		{ return rect.Top(); }
	float& Right()
		{ return rect.Right(); }
	float Right() const
		{ return rect.Right(); }
	float& Bottom()
		{ return rect.Bottom(); }
	float Bottom() const
		{ return rect.Bottom(); }

	CClipVector TopLeft() const;
	CClipVector BottomLeft() const;
	CClipVector TopRight() const;
	CClipVector BottomRight() const;

	float Width() const
		{ return rect.Width(); }
	float Height() const
		{ return rect.Height(); }

	CClipVector CenterPoint() const;

	void OffsetRect( CClipVector offset );
	bool Has( CClipVector pos ) const;

private:
	CAARect<float> rect;
};

//////////////////////////////////////////////////////////////////////////

inline CClipRect::CClipRect( CClipVector bottomLeft, CClipVector topRight ) :
	rect( bottomLeft.X(), topRight.Y(), topRight.X(), bottomLeft.Y() )
{
}

inline CPixelRect CClipRect::FindPixelRect( const CMatrix3<float>& clipToPixel ) const
{
	const CPixelVector bottomLeft = BottomLeft().FindPixelPos( clipToPixel );
	const CPixelVector topRight = TopRight().FindPixelPos( clipToPixel );
	return CPixelRect( bottomLeft, topRight );	
}

inline CClipVector CClipRect::TopLeft() const
{
	return CClipVector( rect.Left(), rect.Top() );
}

inline CClipVector CClipRect::BottomLeft() const
{
	return CClipVector( rect.Left(), rect.Bottom() );
}

inline CClipVector CClipRect::TopRight() const
{
	return CClipVector( rect.Right(), rect.Top() );

}

inline CClipVector CClipRect::BottomRight() const
{
	return CClipVector( rect.Right(), rect.Bottom() );
}

inline CClipVector CClipRect::CenterPoint() const
{
	return CClipVector( rect.CenterPoint() );
}

inline void CClipRect::OffsetRect( CClipVector offset )
{
	rect.OffsetRect( offset.GetClipPos() );
}

inline bool CClipRect::Has( CClipVector pos ) const
{
	return rect.Has( pos.GetClipPos() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

