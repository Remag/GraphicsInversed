#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Blend function mode.
enum TBlendFunction {
	BF_Zero = 0,	// gl::ZERO
	BF_One = 1,	// gl::ONE
	BF_SrcAlpha = 0x0302,	// gl::SRC_ALPHA
	BF_OneMinusSrcAlpha = 0x0303,	// gl::ONE_MINUS_SRC_ALPHA
};

// Mechanism for enabling or disabling blend mode. Currently installed blending function is used.
class GINAPI CBlendModeEnabler {
public:
	explicit CBlendModeEnabler( bool isEnabled );
	~CBlendModeEnabler();

	static bool IsEnabled()
		{ return isEnabled; }
	static void SetBlendModeEnabled( bool isSet );

private:
	bool prevMode;

	static bool isEnabled;
};

// Mechanism for switching the current blending function. Blend mode is automatically enabled when the switcher is constructed.
class GINAPI CBlendModeSwitcher {
public:
	// Enable blending with the given blend functions.
	CBlendModeSwitcher( TBlendFunction srcBlend, TBlendFunction destBlend );
	// Revert to previous blending function and revert the blend mode enable flag.
	~CBlendModeSwitcher();

	static TBlendFunction GetSrcBlend()
		{ return currentSrcBlend; }
	static TBlendFunction GetDestBlend()
		{ return currentDestBlend; }

	static void SetBlendMode( TBlendFunction srcBlend, TBlendFunction destBlend );
	static void SetBlendMode( TBlendFunction srcBlend, TBlendFunction destBlend, bool isEnabled );

private:
	TBlendFunction prevSrcBlend;
	TBlendFunction prevDestBlend;

	static TBlendFunction currentSrcBlend;
	static TBlendFunction currentDestBlend;

	CBlendModeEnabler enabler;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

