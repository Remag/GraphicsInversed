#pragma once
#include <Gindefs.h>
#include <MainFrame.h>
#include <InputHandler.h>

namespace Gin {

class CEngine;
class CStateManager;
struct CGlWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Application class. Provides necessary initialization routines for an OpenGL application.
class GINAPI CApplication : public CSingleton<CApplication>, public IExternalObject {
public:
	CApplication();
	virtual ~CApplication();

	CMainFrame& MainFrame()
		{ return mainFrame; }
	CStateManager& StateManager()
		{ return *stateManager; }

	CEngine* GetEngine() 
		{ return engine; }
	const CEngine* GetEngine() const
		{ return engine; }

	// Post draw and update actions.
	void AddPostUpdateAction( CMutableActionOwner<void()> action )
		{ postUpdateActions.Add( move( action ) ); }
	void AddPostDrawAction( CMutableActionOwner<void()> action )
		{ postDrawActions.Add( move( action ) ); }
	void AddPostOsQueueAction( CMutableActionOwner<void()> action )
		{ postOsQueueActions.Add( move( action ) ); }
	// Add an additional window to the rendering loop.
	CAdditionalWindowInfo& AddAdditionalWindow( CGlWindow window, CPtrOwner<IRenderMechanism> renderer );
	void DeleteAdditionalWindow( const CAdditionalWindowInfo& info );

	// Initialize the application by creating the first state and running custom onInitialize logic.
	// If the initialization was aborted, false is returned.
	bool Initialize( CUnicodeView commandLine );
	// Initialize, create the mainframe and run the message loop.
	void Run( CUnicodeView commandLine );

	void ReopenMainWindow( CGlWindowSettings settings, HICON icon );
	bool TryCloseMainWindow();
	bool TryQuitOnWindowDestruction( HWND destroyedWindow );

	// Actions on window resize.
	// First parameter is the new window size.
	// Second parameter is true if the window was maximized, false otherwise.
	virtual void OnMainWindowResize( CVector2<int>, bool ) {};
	// Actions on system sleep/restore from task bar.
	virtual void OnSleep() {}
	virtual void OnRestore() {}

protected:
	// Additional actions on initialization. These actions must include:
	// - Setting the Input profile for the handler.
	// - Creating and setting the engine for updating the states.
	// - Calling CMainFrame::Initialize() 
	// - Returning an initial state.
	virtual CPtrOwner<IState> onInitialize( CUnicodeView commandLine ) = 0;
	// Initialize parameters associated with the OpenGL context.
	// This is called during main program initialization as well as any time the GL context is recreated.
	virtual void onInitializeGlContext() {};
	// Actions meant to destroy OpenGL data associated with the current context.
	virtual void cleanupGlContext() {};
	// Additional actions before the application exits.
	virtual void onExit() {}
	// Actions taken when the close command is sent.
	// Returns if the window should actually be closed.
	virtual bool checkMainWindowClose() const
		{ return true; }
	// Additional actions on window close.
	virtual void onMainWindowClose() {}
	// Check if the application should quit after the main window is destroyed.
	virtual bool checkApplicationDestruction() const
		{ return true; }
	// Additional actions on application destruction.
	virtual void onApplicationDestruction() {}

	// Manipulation with the application engine.
	void SetEngine( CPtrOwner<CEngine> newEngine );

	// Clear the windows message queue. Return false if a quit message was found.
	bool runWindowsLoop();
	// Run the default update loop.
	void runUpdateLoop();
	// Cleanup action queues.
	void finalizeApplication();

private:
	// Library initializer.
	CRelibInitializer relibInitializer;
	// Main application window. Created with the application.
	CMainFrame mainFrame;
	CPtrOwner<CStateManager> stateManager;
	// Application engine. Updates current state.
	CPtrOwner<CEngine> engine;
	// List of arbitrary actions to execute after the drawing routine is finished.
	CArray<CMutableActionOwner<void()>> postDrawActions;
	// List of arbitrary actions to execute after the update is finished.
	CArray<CMutableActionOwner<void()>> postUpdateActions;
	// List of arbitrary actions to execute after the OS queue processing is finished.
	CArray<CMutableActionOwner<void()>> postOsQueueActions;
	CArray<CAdditionalWindowInfo> additionalWindows;

	void drawAdditionalWindows( const IState& currentState ) const;

	// Execute the given action queue.
	static void executeActions( CArray<CMutableActionOwner<void()>>& actions );

	// Copying is prohibited.
	CApplication( const CApplication& ) = delete;
	void operator=( const CApplication& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.