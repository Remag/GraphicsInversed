#include <common.h>
#pragma hdrstop

#include <VideoSettingsUtils.h>

namespace Gin {

namespace Settings {

//////////////////////////////////////////////////////////////////////////

TVSyncMode GetVSyncMode()
{
	const auto extValue = wgl::GetSwapIntervalEXT();
	return extValue == 0 ? VSM_Off : extValue == 1 ? VSM_On : VSM_Adaptive;
}

void SetVSyncMode( TVSyncMode newValue )
{
	const auto extValue = newValue == VSM_Off ? 0 : newValue == VSM_On ? 1 : -1;
	wgl::SwapIntervalEXT( extValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Settings.

}	// namespace Gin.
