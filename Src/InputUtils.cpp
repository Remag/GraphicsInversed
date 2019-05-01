#include <common.h>
#pragma hdrstop

#include <InputUtils.h>
#include <InputSettings.h>
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

CPtrOwner<TUserAction> CInputTranslator::SetHotkeyAction( int keyCode, CPtrOwner<TUserAction> action )
{
	auto result = DetachHotkeyAction( keyCode );
	hotkeyActions.Add( keyCode, move( action ) );
	return result;
}

CPtrOwner<TUserAction> CInputTranslator::DetachHotkeyAction( int keyCode )
{
	const int hotkeyCount = hotkeyActions.Size();
	for( int i = 0; i < hotkeyCount; i++ ) {
		auto& hotkey = hotkeyActions[i];
		if( hotkey.KeyCode == keyCode ) {
			auto result = move( hotkey.Action );
			hotkeyActions.DeleteAt( i );
			return result;
		}
	}
	
	return nullptr;
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

CInputSettings* CInputSwitcher::currentSettings = 0;

CInputSwitcher::CInputSwitcher( CInputTranslator* newTranslator, IMouseMoveController* newController ) :
	transSwt( newTranslator ),
	moveSwt( newController )
{
}

CInputSwitcher::CInputSwitcher( CUnicodePart _settingsSectionName, IMouseMoveController* newController ) :
	transSwt( getTranslator( currentSettings, _settingsSectionName ) ),
	moveSwt( newController )
{
}

const CInputTranslator* CInputSwitcher::getTranslator( const CInputSettings* settings, CUnicodePart name )
{
	assert( settings != nullptr );
	return &settings->GetTranslator( name );
}

CInputSwitcher::~CInputSwitcher()
{
}

void CInputSwitcher::SetInputSettings( CInputSettings& newValue )
{
	currentSettings = &newValue;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
