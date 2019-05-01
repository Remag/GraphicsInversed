#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mechanism for prohibiting writes to the color buffer.
class GINAPI CColorMaskSwitcher {
public:
	explicit CColorMaskSwitcher( bool newValue );
	~CColorMaskSwitcher();

	static bool CurrentValue()
		{ return currentValue; }

private:
	bool prevValue;

	static bool currentValue;
};

// Mechanism for prohibiting writes to the depth buffer.
class GINAPI CDepthMaskSwitcher {
public:
	explicit CDepthMaskSwitcher( bool newValue );
	~CDepthMaskSwitcher();

	static bool CurrentValue()
		{ return currentValue; }

private:
	bool prevValue;

	static bool currentValue;
};

// Mechanism for prohibiting writes to the stencil buffer.
class GINAPI CStencilMaskSwitcher {
public:
	explicit CStencilMaskSwitcher( bool newValue );
	~CStencilMaskSwitcher();

	static bool CurrentValue()
		{ return currentValue; }

private:
	bool prevValue;

	static bool currentValue;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

