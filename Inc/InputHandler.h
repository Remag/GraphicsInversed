#pragma once
#include <InputUtils.h>
#include <PixelVector.h>
#include <ClipVector.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Handler of user's input. Redirects actions to the current input profile.
class GINAPI CInputHandler {
public:
	explicit CInputHandler();
	~CInputHandler();

	GinInternal::CActionListInputController& GetDefaultInputController()
		{ return *defaultController; }

	void SetNewWindow( HWND newHandle );

	// Delete pressed key information.
	void ClearPressedKeys()
		{ pressedKeys.Empty(); }

	// Check if a given key is being held right now.
	bool IsKeyPressed( int keyCode ) const;

	// Get cached current mouse position in pixel coordinates. It's updated on every move message.
	CPixelVector GetMousePixelPos() const
		{ return mousePixelPos; }

	// Actions on the end of every frame. Gets called after the update phase has been finished and the buffers have been swapped.
	void OnFrameEnd();

	// Translatable actions.
	void OnKeyboardPress( int keyCode, bool isDown );
	void OnMousePress( int keyCode, bool isDown );
	void OnHotkeyPress( int keyCode );
	void OnSymbolMessage( int symbolCode );
	void OnMouseMove( int x, int y );
	void OnMouseLeave();

	bool HasMouseController() const
		{ return currentMouseController != nullptr; }	
	bool IsInTextMode() const
		{ return currentTextTranslator != nullptr; }

	// Get the bitset with all the pressed virtual keys.
	auto GetVirtualKeySet() const
		{ return pressedKeys; }

	// Switcher classes need access to the translator to change it.
	friend class CInputSwitcher;
	friend class CInputControllerSwitcher;
	friend class CTextTranslatorSwitcher;
	friend class CMouseMoveSwitcher;
	friend class CActionDataSwitcher;

private:
	// Window that sends the input.
	HWND windowHandle = nullptr;
	
	// Default input controller. Translates input into actions.
	CPtrOwner<GinInternal::CActionListInputController> defaultController;
	// Current Input controller. User actions are redirected there.
	IInputController* currentInputController = nullptr;

	// Translator for user text input. Works with translated unicode symbols.
	ITextTranslator* currentTextTranslator = nullptr;
	// Default mouse movement controller.
	CPtrOwner<IMouseMoveController> defaultMouseController;
	// Mouse movement controller. 
	IMouseMoveController* currentMouseController = nullptr;
	// Cached mouse position.
	CPixelVector mousePixelPos;
	// Previous mouse position in windows native coordinates.
	CVector2<int> prevMousePos;

	static const int virtualKeyCount = 256;
	// Table of pressed virtual keys.
	CBitSet<virtualKeyCount> pressedKeys;
	// Flag indicating that mouse movement has been handled during this frame transition.
	// Only a single mouse movement is handled during each transition.
	bool mouseMoveHandled = false;

	void initializeNonTextInput( HWND window );
	void initializeTextInput( HWND window );
	void initializeRawInput( HWND window, DWORD kbFlags );

	ITextTranslator* getTextTranslator()
		{ return currentTextTranslator; }
	void setTextTranslator( ITextTranslator* newValue );

	IMouseMoveController* getMouseController()
		{ return currentMouseController; }
	void setMouseController( IMouseMoveController* controller )
		{ currentMouseController = controller; }

	IInputController& getInputController()
		{ assert( currentInputController != nullptr ); return *currentInputController; }
	void setInputController( IInputController& newValue )
		{ currentInputController = &newValue; }

	void updateMousePosition( int x, int y );
	static bool shouldIgnoreSymbol( int symbolCode );

	// Copying is prohibited.
	CInputHandler( const CInputHandler& ) = delete;
	void operator=( const CInputHandler& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
