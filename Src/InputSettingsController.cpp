#include <common.h>
#pragma hdrstop

#include <InputSettingsController.h>
#include <InputBinding.h>

namespace Gin {

namespace GinInternal {
	extern GINAPI CArray<CInputBinding*> ActiveFileKeyBindings;
	extern GINAPI CArray<CSystemInputBinding*> ActiveBasicKeyBindings;
}
//////////////////////////////////////////////////////////////////////////

CInputSettingsController::CInputSettingsController( CUnicodeView settingsFileName ) :
	settingsFile( settingsFileName )
{
	for( auto binding : GinInternal::ActiveFileKeyBindings ) {
		const auto translatorName = binding->GetControlSchemeName();
		const auto newKey = settingsFile.RegisterPrimaryFileAction( binding->GetCurrentKey(), binding->GetActionName(), translatorName );
		binding->SetCurrentKey( newKey );

		const auto altKey = binding->GetCurrentAltKey();
		const auto newAltKey = settingsFile.RegisterSecondaryFileAction( altKey, binding->GetActionName(), translatorName );
		binding->SetCurrentAltKey( newAltKey );
	}

	for( auto binding : GinInternal::ActiveBasicKeyBindings ) {
		settingsFile.AddBasicAction( binding->GetCurrentKey(), binding->CreateUserAction(), binding->GetControlSchemeName() );
	}
	settingsFile.Save();
}

const CInputTranslator& CInputSettingsController::GetInputTranslator( CStringPart translatorName ) const
{
	return settingsFile.GetInputTranslator( translatorName );
}

void CInputSettingsController::ResetInputSettings( CStringPart translatorName )
{
	settingsFile.Empty( translatorName );

	for( auto binding : GinInternal::ActiveFileKeyBindings ) {
		if( binding->GetControlSchemeName() == translatorName ) {
			settingsFile.RegisterPrimaryFileAction( binding->GetCurrentKey(), binding->GetActionName(), translatorName );
			const auto altKey = binding->GetCurrentAltKey();
			if( altKey.MainKey != GVK_Null ) {
				settingsFile.RegisterSecondaryFileAction( altKey, binding->GetActionName(), translatorName );
			}
		}
	}

	for( auto binding : GinInternal::ActiveBasicKeyBindings ) {
		if( binding->GetControlSchemeName() == translatorName ) {
			settingsFile.AddBasicAction( binding->GetCurrentKey(), binding->CreateUserAction(), translatorName );
		}
	}
	settingsFile.Save();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

