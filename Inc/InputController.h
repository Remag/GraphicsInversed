#pragma once
#include <GinDefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class that reacts to user input.
class GINAPI IInputController {
public:
	IInputController() = default;
	virtual ~IInputController() = 0 {}

	// Actions on user input given the virtual key code and the flag indicating the new key state.
	virtual void OnUserInput( int keyCode, bool isKeyDown ) = 0;

private:
	// Copying is prohibited.
	IInputController( IInputController& ) = delete;
	void operator=( IInputController& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// Default input handler controller. Uses the current input translator to fire an appropriate action if present.
class GINAPI CActionListInputController : public IInputController {
public:
	CActionListInputController();

	const CInputTranslator* GetCurrentTranslator() const
		{ return currentTranslator; }
	void SetCurrentTranslator( const CInputTranslator* newValue )
		{ currentTranslator = newValue; }

	virtual void OnUserInput( int keyCode, bool isKeyDown ) override final;

private:
	const CInputTranslator* currentTranslator;
};

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

class GINAPI CInputControllerSwitcher {
public:
	explicit CInputControllerSwitcher( IInputController& newValue );
	~CInputControllerSwitcher();

private:
	IInputController& prevController;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

