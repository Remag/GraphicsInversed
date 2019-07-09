#include <common.h>
#pragma hdrstop

#include <InputUtils.h>
#include <InputHandler.h>
#include <InputController.h>
#include <GinGlobals.h>
#include <NullMouseController.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CPtrOwner<TUserAction> CInputTranslator::SetAction( int keyCode, bool isDown, CPtrOwner<TUserAction> action )
{
	const int pos = getActionIndex( keyCode, isDown );
	if( actions.Size() <= pos ) {
		actions.IncreaseSize( pos + 1 );
	}
	swap( actions[pos], action );
	return action;
}

CPtrOwner<TUserAction> CInputTranslator::DetachAction( int keyCode, bool isDown )
{
	const int pos = getActionIndex( keyCode, isDown );
	return pos < actions.Size() ? move( actions[pos] ) : nullptr;
}

//////////////////////////////////////////////////////////////////////////

CInputTranslatorSwitcher::CInputTranslatorSwitcher( const CInputTranslator* _newTranslator ) :
	prevTranslator( GinInternal::GetDefaultInputController().GetCurrentTranslator() ),
	newTranslator( _newTranslator )
{
	GinInternal::GetDefaultInputController().SetCurrentTranslator( newTranslator );
}

CInputTranslatorSwitcher::~CInputTranslatorSwitcher()
{
	assert( GinInternal::GetDefaultInputController().GetCurrentTranslator() == newTranslator );
	GinInternal::GetDefaultInputController().SetCurrentTranslator( prevTranslator );
}

//////////////////////////////////////////////////////////////////////////

CMouseMoveSwitcher::CMouseMoveSwitcher( IMouseMoveController* _newController ) :
	prevController( GinInternal::GetInputHandler().getMouseController() ),
	newController( _newController )
{
	if( newController == nullptr ) {
		newController = CNullMouseMoveController::GetInstance();
	}
	GinInternal::GetInputHandler().setMouseController( newController );
}

CMouseMoveSwitcher::~CMouseMoveSwitcher()
{
	assert( GinInternal::GetInputHandler().getMouseController() == newController );
	GinInternal::GetInputHandler().setMouseController( prevController );
}

//////////////////////////////////////////////////////////////////////////

CTextTranslatorSwitcher::CTextTranslatorSwitcher( ITextTranslator& newValue ) :
	prevTranslator( GinInternal::GetInputHandler().getTextTranslator() )
{
	GinInternal::GetInputHandler().setTextTranslator( &newValue );
}

CTextTranslatorSwitcher::~CTextTranslatorSwitcher()
{
	GinInternal::GetInputHandler().setTextTranslator( prevTranslator );
}

bool CTextTranslatorSwitcher::IsInTextMode()
{
	return GinInternal::GetInputHandler().IsInTextMode();
}

//////////////////////////////////////////////////////////////////////////

CInputSwitcher::CInputSwitcher( CInputTranslator* newTranslator, IMouseMoveController* newController ) :
	transSwt( newTranslator ),
	moveSwt( newController )
{
}

CInputSwitcher::CInputSwitcher( CStringPart _settingsSectionName, IMouseMoveController* newController ) :
	transSwt( &getTranslator( _settingsSectionName ) ),
	moveSwt( newController )
{
}

const CInputTranslator& CInputSwitcher::getTranslator( CStringPart name )
{
	return GinInternal::GetInputTranslator( name );
}

CInputSwitcher::~CInputSwitcher()
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
