#pragma once
// Support classes for input handling.
#include <GinDefs.h>

namespace Gin {

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

	static int getActionIndex( int keyCode, bool isDown )
		{ return ( keyCode << 1 ) | static_cast<int>( isDown ); }

	// Copying is prohibited.
	CInputTranslator( CInputTranslator& ) = delete;
	void operator=( CInputTranslator& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

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
	virtual void OnSymbolInput( int utf32 ) = 0;
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
	// SettingsSectionName is a name of the section in a application input settings file.
	CInputSwitcher( CStringPart settingsSectionName, IMouseMoveController* newController );
	~CInputSwitcher();
	
private:
	CInputTranslatorSwitcher transSwt;
	CMouseMoveSwitcher moveSwt;

	static const CInputTranslator& getTranslator( CStringPart name );

	// Copying is prohibited.
	CInputSwitcher( CInputSwitcher& ) = delete;
	void operator=( CInputSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// List of available virtual key codes.
enum TGinVirtualKey {
	GVK_Undefined = -1,
	GVK_Null = 0,
	GVK_LMouse = VK_LBUTTON,
	GVK_RMouse = VK_RBUTTON,
	GVK_MMouse = VK_MBUTTON,
	GVK_Mouse4 = VK_XBUTTON1,
	GVK_Mouse5 = VK_XBUTTON2,
	GVK_LDClick = 0x3A,
	GVK_RDClick = 0x3B,
	GVK_MDClick = 0x3C,
	GVK_WheelDown = 0x0E,
	GVK_WheelUp = 0x0F,

	GVK_Escape = VK_ESCAPE,
	GVK_Space = VK_SPACE,
	GVK_LShift = VK_LSHIFT,
	GVK_RShift = VK_RSHIFT,
	GVK_AnyShift = VK_SHIFT,
	GVK_LCtrl = VK_LCONTROL,
	GVK_RCtrl = VK_RCONTROL,
	GVK_AnyCtrl = VK_CONTROL,
	GVK_LAlt = VK_LMENU,
	GVK_RAlt = VK_RMENU,
	GVK_Enter = VK_RETURN,
	GVK_Backspace = VK_BACK,
	GVK_Delete = VK_DELETE,
	GVK_Home = VK_HOME,
	GVK_End = VK_END,
	GVK_Insert = VK_INSERT,
	GVK_Tab = VK_TAB,

	GVK_Left = VK_LEFT,
	GVK_Up = VK_UP,
	GVK_Right = VK_RIGHT,
	GVK_Down = VK_DOWN,

	GVK_Num0 = VK_NUMPAD0,
	GVK_Num1 = VK_NUMPAD1,
	GVK_Num2 = VK_NUMPAD2,
	GVK_Num3 = VK_NUMPAD3,
	GVK_Num4 = VK_NUMPAD4,
	GVK_Num5 = VK_NUMPAD5,
	GVK_Num6 = VK_NUMPAD6,
	GVK_Num7 = VK_NUMPAD7,
	GVK_Num8 = VK_NUMPAD8,
	GVK_Num9 = VK_NUMPAD9,

	GVK_F1 = VK_F1,
	GVK_F2 = VK_F2,
	GVK_F3 = VK_F3,
	GVK_F4 = VK_F4,
	GVK_F5 = VK_F5,
	GVK_F6 = VK_F6,
	GVK_F7 = VK_F7,
	GVK_F8 = VK_F8,
	GVK_F9 = VK_F9,
	GVK_F10 = VK_F10,
	GVK_F11 = VK_F11,
	GVK_F12 = VK_F12,

	GVK_A = 0x41,
	GVK_B = 0x42,
	GVK_C = 0x43,
	GVK_D = 0x44,
	GVK_E = 0x45,
	GVK_F = 0x46,
	GVK_G = 0x47,
	GVK_H = 0x48,
	GVK_I = 0x49,
	GVK_J = 0x4A,
	GVK_K = 0x4B,
	GVK_L = 0x4C,
	GVK_M = 0x4D,
	GVK_N = 0x4E,
	GVK_O = 0x4F,
	GVK_P = 0x50,
	GVK_Q = 0x51,
	GVK_R = 0x52,
	GVK_S = 0x53,
	GVK_T = 0x54,
	GVK_U = 0x55,
	GVK_V = 0x56,
	GVK_W = 0x57,
	GVK_X = 0x58,
	GVK_Y = 0x59,
	GVK_Z = 0x5A,

	GVK_0 = 0x30,
	GVK_1 = 0x31,
	GVK_2 = 0x32,
	GVK_3 = 0x33,
	GVK_4 = 0x34,
	GVK_5 = 0x35,
	GVK_6 = 0x36,
	GVK_7 = 0x37,
	GVK_8 = 0x38,
	GVK_9 = 0x39,

	GVK_Tilde = VK_OEM_3,
	GVK_Minus = VK_OEM_MINUS,
	GVK_Equal = VK_OEM_PLUS,
	GVK_BackSlash = VK_OEM_5,
	GVK_SingleQuote = VK_OEM_7,
	GVK_Semicolon = VK_OEM_1,
	GVK_OpenBracket = VK_OEM_4,
	GVK_CloseBracket = VK_OEM_6,
	GVK_Slash = VK_OEM_2,
	GVK_Period = VK_OEM_PERIOD,
	GVK_Comma = VK_OEM_COMMA
};

enum TKeyModifierType {
	KMT_Undefined,
	KMT_Press,
	KMT_Release,
	KMT_EnumCount
};

// A set of pressed or released keys.
struct CKeyCombination {
	// Main key virtual code.
	TGinVirtualKey MainKey = GVK_Null;
	// Optional key that is required to be pressed when the main key code is triggered.
	TGinVirtualKey AdditionalKey = GVK_Null;
	// Explicit key modifier if preset.
	TKeyModifierType KeyModifier = KMT_Undefined;

	CKeyCombination() = default;
	CKeyCombination( TGinVirtualKey mainKey, TKeyModifierType keyModifier = KMT_Press, TGinVirtualKey additionalKey = GVK_Null ) : 
		MainKey( mainKey ), AdditionalKey( additionalKey ), KeyModifier( keyModifier ) {}

	int HashKey() const
		{ return CombineHashKey( CombineHashKey( MainKey, AdditionalKey ), KeyModifier ); }
	bool operator==( CKeyCombination other ) const
		{ return MainKey == other.MainKey && AdditionalKey == other.AdditionalKey && KeyModifier == other.KeyModifier; }
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
