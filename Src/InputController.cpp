#include <common.h>
#pragma hdrstop

#include <InputController.h>
#include <InputUtils.h>
#include <InputHandler.h>
#include <GinGlobals.h>

namespace Gin {

namespace GinInternal {

extern const CInputTranslator DefaultTranslator;
//////////////////////////////////////////////////////////////////////////

CActionListInputController::CActionListInputController() :
	currentTranslator( &DefaultTranslator )
{
}

void CActionListInputController::OnUserInput( int keyCode, bool isKeyDown )
{
	assert( currentTranslator != nullptr );
	const auto action = currentTranslator->GetAction( keyCode, isKeyDown );
	if( action != nullptr ) {
		action->Invoke();
	}
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

CInputControllerSwitcher::CInputControllerSwitcher( IInputController& newValue ) :
	prevController( GinInternal::GetInputHandler().getInputController() )
{
	GinInternal::GetInputHandler().setInputController( newValue );
}

CInputControllerSwitcher::~CInputControllerSwitcher()
{
	GinInternal::GetInputHandler().setInputController( prevController );
}

IInputController& CInputControllerSwitcher::GetDefaultController()
{
	return GinInternal::GetInputHandler().getDefaultInputController();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
