#pragma once
#include <InputUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mouse movement controller that does nothing.
class CNullMouseMoveController : public CBaseMouseMoveController, public CSingleton<CNullMouseMoveController> {
public:
	virtual void OnMouseMove() override final {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

