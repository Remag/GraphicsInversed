#pragma once
// Accessors to global variables.

#include <Gindefs.h>

namespace Gin {

class IRenderMechanism;
class CPixelVector;
class CApplication;
class CInputHandler;
class CInputSettings;
class CMainFrame;
class CStateManager;
class CGlWindow;
class CGlContextManager;
class CStandardWindowDispatcher;
class CEngine;
struct CGlWindowSettings;

// Get the state manager. The class can be used to access the state stack.
GINAPI CStateManager& GetStateManager();
// Main window. Can be used to manage window size/resolution.
GINAPI CGlWindow& GetMainWindow();
// Dispatcher of the main window class.
GINAPI CStandardWindowDispatcher& GetMainWindowDispatcher();
// OpenGL rendering context manager.
GINAPI CGlContextManager& GetGlContextManager();
// Update engine.
GINAPI CEngine& GetEngine();
GINAPI CPixelVector MousePixelPos();
GINAPI const CGlWindow* GetMouseHoverWindow();
GINAPI IRenderMechanism& GetRenderMechanism();

// Check if a given virtual key is being held by the user.
GINAPI bool IsVirtualKeyDown( int virtualCode );

// Action execution functions. 

// Execute a given action after an update phase of the current frame finishes.
GINAPI void ExecutePostUpdate( CMutableActionOwner<void()> action );
// Execute a given action after an drawing phase of the current frame finishes.
// Adding a post draw action during an update phase is prohibited since the post-draw execution has already happened.
GINAPI void ExecutePostDraw( CMutableActionOwner<void()> action );
// Execute a given action after the OS message processing is completed.
GINAPI void ExecutePostOsQueue( CMutableActionOwner<void()> action );

// Create a new main window with the given settings. Previous main window is destroyed.
GINAPI void ReopenMainWindow( CGlWindowSettings windowSettings, HICON windowIcon );

// Save all the changes made to key bindings in the input settings file.
GINAPI void CommitInputKeyChanges( CStringPart controlSchemeName );

// Add the given window to an application rendering procedure.
GINAPI void AttachAdditionalWindow( CPtrOwner<CGlWindow> newWindow, TWindowRendererType rendererType );
// Find an existing additional window with the given class name.
// Returns null if no window was found.
GINAPI CGlWindow* FindAdditionalWindow( CUnicodePart className );
// Find an existing additional window with the given dispatcher.
// Returns null if no window was found.
template <class Dispatcher>
CGlWindow* FindAdditionalWindow()
{
	return FindAdditionalWindow( CWindowClass<Dispatcher>::GetInstance()->GetClassName() );
}

//////////////////////////////////////////////////////////////////////////


// Part of the library has nothing interesting to the user.
namespace GinInternal {
	
CApplication& GetApplication();
CMainFrame& GetMainFrame();
CInputHandler& GetInputHandler();
CActionListInputController& GetDefaultInputController();
const CInputTranslator& GetInputTranslator( CStringPart translatorName );

}

}	// namespace Gin.

