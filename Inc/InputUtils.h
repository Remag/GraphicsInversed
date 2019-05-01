#pragma once
// Support classes for input handling.
#include <GinDefs.h>

namespace Gin {

class CInputSettings;
//////////////////////////////////////////////////////////////////////////

// Class that translates user input into application defined action ids.
class GINAPI CInputTranslator {
public:
	CInputTranslator() = default;

	void Empty()
		{ actions.Empty(); }
	
	// Get full action list.
	CArrayView<CPtrOwner<TUserAction>> GetAllActions() const
		{ return actions; }

	CPtrOwner<TUserAction> SetHotkeyAction( int keyCode, CPtrOwner<TUserAction> action );
	CPtrOwner<TUserAction> DetachHotkeyAction( int keyCode );
	const TUserAction* GetHotkeyAction( int keyCode ) const;

	// Get/Set the action corresponding to the given keystroke.
	const TUserAction* GetAction( int keyCode, bool isDown ) const;
	CPtrOwner<TUserAction> SetAction( int keyCode, bool isDown, CPtrOwner<TUserAction> action );
	CPtrOwner<TUserAction> DetachAction( int keyCode, bool isDown );

private:
	// Array of possible actions.
	// Virtual key and pressed bit serve as an index.
	CArray<CPtrOwner<TUserAction>> actions;

	struct CHotkeyData {
		int KeyCode;
		CPtrOwner<TUserAction> Action;
	};
	// List of registered hotkeys. Unsorted.
	CArray<CHotkeyData> hotkeyActions;

	static int getActionIndex( int keyCode, bool isDown )
		{ return ( keyCode << 1 ) | static_cast<int>( isDown ); }

	// Copying is prohibited.
	CInputTranslator( CInputTranslator& ) = delete;
	void operator=( CInputTranslator& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

inline const TUserAction* CInputTranslator::GetHotkeyAction( int keyCode ) const
{
	for( const auto& hotkey : hotkeyActions ) {
		if( hotkey.KeyCode == keyCode ) {
			return hotkey.Action;
		}
	}

	return nullptr;
}

inline const TUserAction* CInputTranslator::GetAction( int keyCode, bool isDown ) const
{
	const int pos = getActionIndex( keyCode, isDown );
	return pos >= actions.Size() ? nullptr : actions[pos].Ptr();
}

//////////////////////////////////////////////////////////////////////////

// Mouse movement controller. Responds to cursor movements.
class IMouseMoveController {
public:
	virtual ~IMouseMoveController() {}

	// Response to mouse movement.
	virtual void OnMouseMove() = 0;
	// Response to mouse leaving the main window. This callback is not invoked unless TrackMouseEvent is called.
	virtual void OnMouseLeave() = 0;
};

// Base mouse controller that doesn't have a mouse leave implementation.
class CBaseMouseMoveController : public IMouseMoveController {
public:
	virtual void OnMouseLeave() override final {}
};

//////////////////////////////////////////////////////////////////////////

// Input text translator. Works with translated unicode symbols.
class ITextTranslator {
public:
	virtual ~ITextTranslator() {}

	// Actions on a symbol input.
	virtual void OnSymbolInput( wchar_t symbolCode ) = 0;
};

//////////////////////////////////////////////////////////////////////////

// Class for switching input translators.
class GINAPI CInputTranslatorSwitcher {
public:
	explicit CInputTranslatorSwitcher( const CInputTranslator* newTranslator );
	~CInputTranslatorSwitcher();

private:
	const CInputTranslator* prevTranslator;
	const CInputTranslator* newTranslator;

	// Copying is prohibited.
	CInputTranslatorSwitcher( const CInputTranslatorSwitcher& ) = delete;
	void operator=( const CInputTranslatorSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Class for switching mouse movement controllers.
class GINAPI CMouseMoveSwitcher {
public:
	explicit CMouseMoveSwitcher( IMouseMoveController* newController );
	~CMouseMoveSwitcher();

private:
	IMouseMoveController* prevController;
	IMouseMoveController* newController;

	// Copying is prohibited.
	CMouseMoveSwitcher( CMouseMoveSwitcher& ) = delete;
	void operator=( CMouseMoveSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Switcher for text translators.
class GINAPI CTextTranslatorSwitcher {
public:
	explicit CTextTranslatorSwitcher( ITextTranslator& newValue );
	~CTextTranslatorSwitcher();

	static bool IsInTextMode();

private:
	ITextTranslator* prevTranslator;

	// Copying is prohibited.
	CTextTranslatorSwitcher( CTextTranslatorSwitcher& ) = delete;
	void operator=( CTextTranslatorSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Input switcher for both the translator and move controller.
class GINAPI CInputSwitcher {
public:
	CInputSwitcher( CInputTranslator* newTranslator, IMouseMoveController* newController );
	// SettingsSectionName is a name of the section in a current input settings file.
	CInputSwitcher( CUnicodePart settingsSectionName, IMouseMoveController* newController );
	~CInputSwitcher();

	static CInputSettings& GetInputSettings()
		{ assert( currentSettings != nullptr ); return *currentSettings; }
	static void SetInputSettings( CInputSettings& newValue );
	
private:
	CInputTranslatorSwitcher transSwt;
	CMouseMoveSwitcher moveSwt;

	// Current file that has the input settings.
	static CInputSettings* currentSettings;

	static const CInputTranslator* getTranslator( const CInputSettings* settings, CUnicodePart name );

	// Copying is prohibited.
	CInputSwitcher( CInputSwitcher& ) = delete;
	void operator=( CInputSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
