#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

class GINAPI CDepthTestSwitcher {
public:
	explicit CDepthTestSwitcher( bool enableTesting );
	~CDepthTestSwitcher();

private:
	bool prevTestEnabled;

	static bool isTestEnabled;

	void doEnableTesting( bool enableTesting );
};

//////////////////////////////////////////////////////////////////////////

}	 // namespace Gin.