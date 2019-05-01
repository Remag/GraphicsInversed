#pragma once
#include <Gindefs.h>

namespace Gin {

class CClipVector;
//////////////////////////////////////////////////////////////////////////

// Position or offset in pixel space.
class GINAPI CPixelVector {
public:
	CPixelVector() = default;
	// Create from a position on the grid.
	explicit CPixelVector( int gridX, int gridY ) : pos( numeric_cast<float>( gridX ), numeric_cast<float>( gridY ) ) {}
	explicit CPixelVector( CVector2<int> gridPos ) : CPixelVector( gridPos.X(), gridPos.Y() ) {}
	// Create from an arbitrary position.
	explicit CPixelVector( float x, float y ) : pos( x, y ) {}
	explicit CPixelVector( CVector2<float> _pos ) : pos( _pos ) {}

	static constexpr int Size()
		{ return 2; }

	float X() const
		{ return pos.X(); }
	float& X()
		{ return pos.X(); }
	float Y() const
		{ return pos.Y(); }
	float& Y()
		{ return pos.Y(); }
	CVector2<float> GetPixelPos() const
		{ return pos; }

	float operator[]( int index ) const
		{ return pos[index]; }
	float& operator[]( int index )
		{ return pos[index]; }

	// Find the closest pixel grid position.
	CVector2<int> GetGridPos() const;
	// Position in clip space.
	CClipVector FindClipPos( const CMatrix3<float>& pixelToClip ) const;
	// Size in clip space.
	CClipVector FindClipSize( const CMatrix3<float>& pixelToClip ) const;

	float SquareLength() const
		{ return pos.SquareLength(); }
	float Length() const
		{ return pos.Length(); }

	// Vector operations. 
	// Addition and subtraction.
	CPixelVector& operator+=( const CPixelVector& other )
		{ pos += other.pos; return *this; }
	CPixelVector& operator-=( const CPixelVector& other )
		{ pos -= other.pos; return *this; }

	// Scalar multiplication and division.
	CPixelVector& operator*=( float mul )
		{ pos *= mul; return *this; }
	CPixelVector& operator*=( int mul )
		{ pos *= mul; return *this; }
	CPixelVector& operator/=( float mul )
		{ pos /= mul; return *this; }


	// Unary minus.
	CPixelVector operator-() const
		{ return CPixelVector( -pos ); }

private:
	// Pixel position can represent positions misaligned to the pixel grid. That's why it's stored in a floating point vector.
	CVector2<float> pos;
};

//////////////////////////////////////////////////////////////////////////

inline CVector2<int> CPixelVector::GetGridPos() const
{
	return CVector2<int>( Round( pos.X() ), Round( pos.Y() ) );
}

//////////////////////////////////////////////////////////////////////////

// Vector math functions.

inline CPixelVector operator+( CPixelVector left, const CPixelVector right )
{
	left += right;
	return left;
}

inline CPixelVector operator-( CPixelVector left, const CPixelVector right )
{
	left -= right;
	return left;
}

inline CPixelVector operator*( CPixelVector left, float mul )
{
	left *= mul;
	return left;
}

inline CPixelVector operator*( CPixelVector left, int mul )
{
	left *= mul;
	return left;
}

inline CPixelVector operator*( float mul, CPixelVector right )
{
	right *= mul;
	return right;
}

inline CPixelVector operator*( int mul, CPixelVector right )
{
	right *= mul;
	return right;
}

inline CPixelVector operator/( CPixelVector left, float mul )
{
	left /= mul;
	return left;
}

//////////////////////////////////////////////////////////////////////////

namespace Coordinates {

// Fractional part of a clip space origin point offset from pixel space.
// Basically, the returned value on each axis is either 0 for viewports with even number of pixels or 0.5 for viewports with odd number of pixels.
inline CPixelVector PixelOriginOffsetFraction( CMatrix3<float> clipToPixel )
{
	const auto xOffset = abs( clipToPixel( 2, 0 ) - static_cast<int>( clipToPixel( 2, 0 ) ) );
	const auto yOffset = abs( clipToPixel( 2, 1 ) - static_cast<int>( clipToPixel( 2, 1 ) ) );
	return CPixelVector( xOffset, yOffset );
}	

}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

