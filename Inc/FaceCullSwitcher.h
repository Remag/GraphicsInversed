#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

enum TTriangleWindingOrder {
	TWO_Clockwise = 0x0900,	// gl::CW
	TWO_CounterClockwise = 0x0901 // gl::CCW
};

//////////////////////////////////////////////////////////////////////////

// Switchers for face culling and triangle winding order.
class GINAPI CFaceCullEnabler {
public:
	explicit CFaceCullEnabler( bool isEnabled );
	~CFaceCullEnabler();

private:
	bool prevIsEnabled;

	static bool currentIsEnabled;

	static void enableFaceCulling( bool isEnabled );
};

class GINAPI CFaceCullSwitcher {
public:
	explicit CFaceCullSwitcher( TTriangleWindingOrder windingOrder );
	~CFaceCullSwitcher();

private:
	CFaceCullEnabler enabler;
	TTriangleWindingOrder prevOrder;

	static TTriangleWindingOrder currentOrder;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

