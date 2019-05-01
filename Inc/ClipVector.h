#pragma once
#include <PixelVector.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Position in clip coordinates.
class CClipVector {
public:
	CClipVector() = default;
	explicit CClipVector( float x, float y ) : pos( x, y ) {}
	explicit CClipVector( CVector2<float> _pos ) : pos( _pos ) {}

	float X() const
		{ return pos.X(); }
	float& X()
		{ return pos.X(); }
	float Y() const
		{ return pos.Y(); }
	float& Y()
		{ return pos.Y(); }
	CVector2<float> GetClipPos() const
		{ return pos; }

	float operator[]( int index ) const
		{ return pos[index]; }
	float& operator[]( int index )
		{ return pos[index]; }

	// Position in pixel coordinates.
	CPixelVector FindPixelPos( CMatrix3<float> clipToPixel ) const;
	// Convert to offset in pixel coordinates.
	CPixelVector FindPixelOffset( CMatrix3<float> clipToPixel ) const;

	// Vector operations. 
	// Addition and subtraction.
	CClipVector& operator+=( CClipVector other )
		{ pos += other.pos; return *this; }
	CClipVector& operator-=( CClipVector other )
		{ pos -= other.pos; return *this; }

	// Scalar multiplication and division.
	CClipVector& operator*=( float mul )
		{ pos *= mul; return *this; }
	CClipVector& operator/=( float mul )
		{ pos /= mul; return *this; }

	// Unary minus.
	CClipVector operator-() const
		{ return CClipVector( -pos ); }

private:
	CVector2<float> pos;
};

//////////////////////////////////////////////////////////////////////////

inline CPixelVector CClipVector::FindPixelPos( CMatrix3<float> clipToPixel ) const
{
	return CPixelVector( PointTransform( clipToPixel, pos ) );
}

inline CPixelVector CClipVector::FindPixelOffset( CMatrix3<float> clipToPixel ) const
{
	return CPixelVector{ VecTransform( clipToPixel, pos ) };
}

// Vector math functions.

inline CClipVector operator+( CClipVector left, const CClipVector right )
{
	left += right;
	return left;
}

inline CClipVector operator-( CClipVector left, const CClipVector right )
{
	left -= right;
	return left;
}

inline CClipVector operator*( CClipVector left, float mul )
{
	left *= mul;
	return left;
}

inline CClipVector operator*( float mul, CClipVector right )
{
	right *= mul;
	return right;
}

inline CClipVector operator/( CClipVector left, float mul )
{
	left /= mul;
	return left;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

