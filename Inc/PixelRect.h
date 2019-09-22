#pragma once
#include <PixelVector.h>

namespace Gin {

class CClipRect;
//////////////////////////////////////////////////////////////////////////

// Axis aligned rectangle in pixel space.
class GINAPI CPixelRect {
public:
	CPixelRect() = default;
	CPixelRect( int left, int top, int right, int bottom ) : 
		rect( numeric_cast<float>( left ), numeric_cast<float>( top ), numeric_cast<float>( right ), numeric_cast<float>( bottom ) ) {}
	CPixelRect( float left, float top, float right, float bottom ) : rect( left, top, right, bottom ) {}
	CPixelRect( CPixelVector bottomLeft, CPixelVector topRight );
	CPixelRect( CPixelVector bottomLeft, int width, int height );
	CPixelRect( CPixelVector bottomLeft, float width, float height );

	explicit CPixelRect( CAARect<int> _rect ) : CPixelRect( _rect.Left(), _rect.Top(), _rect.Right(), _rect.Bottom() ) {}
	explicit CPixelRect( CAARect<float> _rect ) : rect( _rect ) {}
	
	CAARect<float> GetPixelRect() const
		{ return rect; }
	void SetPixelRect( CAARect<float> newValue )
		{ rect = newValue; }
	void SetPixelRect( CAARect<int> newValue )
		{ rect = static_cast<CAARect<float>>( newValue ); }
	void SetPixelRect( int left, int top, int right, int bottom )
		{ rect.SetRect( numeric_cast<float>( left ), numeric_cast<float>( top ), numeric_cast<float>( right ), numeric_cast<float>( bottom ) ); }
	void SetPixelRect( float left, float top, float right, float bottom )
		{ rect.SetRect( left, top, right, bottom ); }
		
	// Closest grid aligned rectangle.
	CAARect<int> GetGridRect() const;
	// Smallest bounding grid aligned rectangle.
	CAARect<int> GetBoundGridRect() const;

	// Get rectangle in clip space.
	CClipRect FindScreenRect( const CMatrix3<float>& pixelToClip ) const;

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

	int GridLeft() const
		{ return Round( Left() ); }
	int GridTop() const
		{ return Round( Top() ); }
	int GridRight() const
		{ return Round( Right() ); }
	int GridBottom() const
		{ return Round( Bottom() ); }

	CPixelVector TopLeft() const;
	CPixelVector BottomLeft() const;
	CPixelVector TopRight() const;
	CPixelVector BottomRight() const;

	float Width() const
		{ return rect.Width(); }
	float Height() const
		{ return rect.Height(); }

	float Area() const
		{ return rect.Area(); }
	CPixelVector CenterPoint() const;

	CPixelRect GetOffsetRect( int leftDelta, int topDelta, int rightDelta, int bottomDelta ) const;
	CPixelRect GetOffsetRect( float leftDelta, float topDelta, float rightDelta, float bottomDelta ) const;
	void OffsetRect( CPixelVector offset );
	bool Has( CPixelVector pos ) const;
	bool Has( const CPixelRect& other ) const;
	bool StrictHas( CPixelVector pos ) const;
	
private:
	CAARect<float> rect;
};

//////////////////////////////////////////////////////////////////////////

inline CPixelRect::CPixelRect( CPixelVector bottomLeft, CPixelVector topRight ) :
	rect( bottomLeft.X(), topRight.Y(), topRight.X(), bottomLeft.Y() )
{
}

inline CPixelRect::CPixelRect( CPixelVector bottomLeft, int width, int height ) :
	rect( bottomLeft.GetPixelPos(), numeric_cast<float>( width ), numeric_cast<float>( height ) )
{
}

inline CPixelRect::CPixelRect( CPixelVector bottomLeft, float width, float height ) :
	rect( bottomLeft.GetPixelPos(), width, height )
{

}

inline CAARect<int> CPixelRect::GetGridRect() const
{
	return CAARect<int>{ Round( rect.Left() ), Round( rect.Top() ), Round( rect.Right() ), Round( rect.Bottom() ) };
}

inline CAARect<int> CPixelRect::GetBoundGridRect() const
{
	return CAARect<int>{ Floor( rect.Left() ), Ceil( rect.Top() ), Ceil( rect.Right() ), Floor( rect.Bottom() ) };
}

inline CPixelVector CPixelRect::TopLeft() const
{
	return CPixelVector( rect.Left(), rect.Top() );
}

inline CPixelVector CPixelRect::BottomLeft() const
{
	return CPixelVector( rect.Left(), rect.Bottom() );
}

inline CPixelVector CPixelRect::TopRight() const
{
	return CPixelVector( rect.Right(), rect.Top() );
}

inline CPixelVector CPixelRect::BottomRight() const
{
	return CPixelVector( rect.Right(), rect.Bottom() );
}

inline CPixelVector CPixelRect::CenterPoint() const
{
	return CPixelVector( rect.CenterPoint() );
}

inline CPixelRect CPixelRect::GetOffsetRect( int leftDelta, int topDelta, int rightDelta, int bottomDelta ) const
{
	return rect.GetOffsetRect( static_cast<float>( leftDelta ), static_cast<float>( topDelta ), static_cast<float>( rightDelta ), static_cast<float>( bottomDelta ) );
}

inline CPixelRect CPixelRect::GetOffsetRect( float leftDelta, float topDelta, float rightDelta, float bottomDelta ) const
{
	return rect.GetOffsetRect( leftDelta, topDelta, rightDelta, bottomDelta );
}

inline void CPixelRect::OffsetRect( CPixelVector offset )
{
	rect.OffsetRect( offset.GetPixelPos() );
}

inline bool CPixelRect::Has( CPixelVector pos ) const
{
	return rect.Has( pos.GetPixelPos() );
}

inline bool CPixelRect::Has( const CPixelRect& other ) const
{
	return rect.Has( other.rect );
}

inline bool CPixelRect::StrictHas( CPixelVector pos ) const
{
	return rect.StrictHas( pos.GetPixelPos() );
}

//////////////////////////////////////////////////////////////////////////

inline CPixelRect GetIntersection( CPixelRect left, CPixelRect right )
{
	return CPixelRect( GetIntersection( left.GetPixelRect(), right.GetPixelRect() ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

