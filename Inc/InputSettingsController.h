#pragma once
#include <InputSettings.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mechanism for initializing the given file with globally defined input keys.
class CInputSettingsController {
public:
	explicit CInputSettingsController( CStringPart settingsFileName );

	const CInputTranslator& GetInputTranslator( CStringPart translatorName ) const;

	// Refill the settings file with current input binding values.
	void ResetInputSettings( CStringPart translatorName );

private:
	CInputSettings settingsFile;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

