#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

class GINAPI CFreeTypeInitializer : public CSingleton<CFreeTypeInitializer> {
public:
	CFreeTypeInitializer();
	~CFreeTypeInitializer();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.