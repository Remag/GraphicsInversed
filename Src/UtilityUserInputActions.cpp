#include <common.h>
#pragma hdrstop

#include <UtilityUserInputActions.h>
#include <GinGlobals.h>
#include <InputHandler.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CAdditionalKeyAction::Invoke() const
{
	const auto& inputHandler = GinInternal::GetInputHandler();

	if( inputHandler.IsKeyPressed( additionalKey ) ) {
		action->Invoke();
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
