#include <common.h>
#pragma hdrstop

#include <InputHandler.h>
#include <GinGlobals.h>
#include <GlWindow.h>
#include <InputSettings.h>
#include <InputController.h>
#include <NullMouseController.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CInputHandler::CInputHandler() :
	defaultController( CreateOwner<GinInternal::CActionListInputController>() ),
	defaultMouseController( CreateOwner<CNullMouseMoveController>() ),
	currentMouseController( defaultMouseController ),
	currentInputController( defaultController )
{
}

void CInputHandler::initializeNonTextInput( HWND handle )
{
	initializeRawInput( handle, RIDEV_NOLEGACY );
}

void CInputHandler::initializeTextInput( HWND handle )
{
	initializeRawInput( handle, 0 );
}

void CInputHandler::initializeRawInput( HWND handle, DWORD kbFlags )
{
	RAWINPUTDEVICE device;
	device.usUsagePage = 1;
	device.usUsage = 6;
	device.dwFlags = kbFlags;
	device.hwndTarget = handle;
	::RegisterRawInputDevices( &device, 1, sizeof( device ) );
}

CInputHandler::~CInputHandler()
{
}

void CInputHandler::SetNewWindow( HWND newHandle )
{
	windowHandle = newHandle;
	initializeNonTextInput( windowHandle );
}

bool CInputHandler::IsKeyPressed( int keyCode ) const
{
	return keyCode < virtualKeyCount && pressedKeys.Has( keyCode );
}

void CInputHandler::OnFrameEnd()
{
	mouseMoveHandled = false;
}

void CInputHandler::OnKeyboardPress( int keyCode, bool isDown )
{
	assert( currentInputController != nullptr );
	if( isDown && pressedKeys.Has( keyCode ) && !IsInTextMode() ) {
		// Ignore consecutive key presses.
		return;
	}
	pressedKeys.Set( keyCode, isDown );
	return currentInputController->OnUserInput( keyCode, isDown );
}

void CInputHandler::OnMousePress( int keyCode, bool isDown )
{
	assert( currentInputController != nullptr );
	return currentInputController->OnUserInput( keyCode, isDown );
}

void CInputHandler::OnHotkeyPress( int keyCode )
{
	assert( currentInputController != nullptr );
	return currentInputController->OnUserHotkey( keyCode );
}

void CInputHandler::OnSymbolMessage( int symbolCode )
{
	if( currentTextTranslator != 0 && !shouldIgnoreSymbol( symbolCode ) ) {
		currentTextTranslator->OnSymbolInput( static_cast<wchar_t>( symbolCode ) );
	}
}

// Filter out non-character symbols.
bool CInputHandler::shouldIgnoreSymbol( int symbolCode )
{
	// Ignore control characters.
	return symbolCode < 32;
}

void CInputHandler::OnMouseMove( int x, int y )
{
	if( mouseMoveHandled ) {
		return;
	}
	assert( HasMouseController() );
	// Sometimes the application receives mouse move messages that don't correspond to an actual movement. Filter them out.
	if( prevMousePos.X() == x && prevMousePos.Y() == y ) {
		return;
	}
	mouseMoveHandled = true;
	prevMousePos.X() = x;
	prevMousePos.Y() = y;

	updateMousePosition( x, y );
	currentMouseController->OnMouseMove();
}

void CInputHandler::OnMouseLeave()
{
	assert( HasMouseController() );
	prevMousePos.X() = prevMousePos.Y() = -1;
	mousePixelPos.X() = mousePixelPos.Y() = -1.0f;
	currentMouseController->OnMouseLeave();
}

void CInputHandler::updateMousePosition( int x, int y )
{
	const CVector2<int> windowSize = GetMainWindow().WindowSize();

	mousePixelPos.X() = x + 0.5f;
	mousePixelPos.Y() = windowSize.Y() - y - 0.5f;
}

void CInputHandler::setTextTranslator( ITextTranslator* newValue )
{
	if( currentTextTranslator == nullptr ) {
		initializeTextInput( windowHandle );
	}
	if( newValue == nullptr ) {
		initializeNonTextInput( windowHandle );
	}

	currentTextTranslator = newValue;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
