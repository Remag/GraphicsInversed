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
	initializeRawInput();
}

void CInputHandler::initializeRawInput()
{
	RAWINPUTDEVICE device;
	device.usUsagePage = 1;
	device.usUsage = 6;
	device.dwFlags = 0;
	device.hwndTarget = nullptr;
	::RegisterRawInputDevices( &device, 1, sizeof( device ) );
}

CInputHandler::~CInputHandler()
{
}

bool CInputHandler::IsKeyPressed( int keyCode ) const
{
	return keyCode < virtualKeyCount && pressedKeys.Has( keyCode );
}

void CInputHandler::OnWindowDestruction( const CGlWindow* targetWindow )
{
	if( mouseHoverWindow == targetWindow ) {
		mouseHoverWindow = nullptr;
		resetMousePosition();
	}
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

void CInputHandler::OnSymbolMessage( int symbolCode )
{
	if( currentTextTranslator != nullptr && !shouldIgnoreSymbol( symbolCode ) ) {
		currentTextTranslator->OnSymbolInput( symbolCode );
	}
}

// Filter out non-character symbols.
bool CInputHandler::shouldIgnoreSymbol( int symbolCode )
{
	// Ignore control characters.
	return symbolCode < 32;
}

void CInputHandler::OnMouseMove( int x, int y, CGlWindow& targetWindow )
{
	if( mouseMoveHandled ) {
		return;
	}
	assert( HasMouseController() );
	// Sometimes the application receives mouse move messages that don't correspond to an actual movement. Filter them out.
	if( mouseHoverWindow == &targetWindow && prevMousePos.X() == x && prevMousePos.Y() == y ) {
		return;
	}
	mouseMoveHandled = true;
	prevMousePos.X() = x;
	prevMousePos.Y() = y;
	mouseHoverWindow = &targetWindow;

	updateMousePosition( x, y );
	currentMouseController->OnMouseMove();
}

void CInputHandler::OnMouseLeave()
{
	assert( HasMouseController() );
	resetMousePosition();
	currentMouseController->OnMouseLeave();
}

IInputController& CInputHandler::getDefaultInputController()
{
	return *defaultController;
}

void CInputHandler::resetMousePosition()
{
	prevMousePos.X() = prevMousePos.Y() = -1;
	mousePixelPos.X() = mousePixelPos.Y() = -1.0f;
}

void CInputHandler::updateMousePosition( int x, int y )
{
	const auto windowSize = mouseHoverWindow->WindowSize();
	mousePixelPos.X() = x + 0.5f;
	mousePixelPos.Y() = windowSize.Y() - y - 0.5f;
}

void CInputHandler::setTextTranslator( ITextTranslator* newValue )
{
	currentTextTranslator = newValue;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
