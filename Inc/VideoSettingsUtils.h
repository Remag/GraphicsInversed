#pragma once
#include <Gindefs.h>

namespace Gin {

namespace Settings {

//////////////////////////////////////////////////////////////////////////

// Vertical synchronization mode.
enum TVSyncMode {
	VSM_Off,
	VSM_On,
	VSM_Adaptive,
	VSM_EnumCount
};

GINAPI TVSyncMode GetVSyncMode();
GINAPI void SetVSyncMode( TVSyncMode newValue );

//////////////////////////////////////////////////////////////////////////

}	// namespace Settings.

}	// namespace Gin.

