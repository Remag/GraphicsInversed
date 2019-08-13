#pragma once
#include <Gindefs.h>

namespace Gin {

class IState;
class IRenderMechanism;
class CDefaultSamplerContainer;
class CStandardWindowDispatcher;
struct CGlWindowSettings;
struct COpenGlVersion;
//////////////////////////////////////////////////////////////////////////

// Main window class. Handles window class initialization. Reroutes window initialization and draw calls.
class GINAPI CMainFrame {
public:
	CMainFrame();
	~CMainFrame();

	// Accessors to the context manager and window class.
	CGlWindow& GetMainGlWindow()
		{ return *mainWindow; }
	const CGlWindow& GetMainGlWindow() const
		{ return *mainWindow; }

	IRenderMechanism& GetRenderer()
		{ return *renderer; }
	const IRenderMechanism& GetRenderer() const
		{ return *renderer; }
	Audio::CAlContextManager& AlContextManager()
		{ return *alContextManager; }
	CGlContextManager& GetGlContextManager()
		{ return *glContextManager; }

	CInputHandler& InputHandler()
		{ return *inputHandler; }

	// Create a window and an OpenGL context.
	void InitializeOpenGL( CGlWindowSettings initialSettings, COpenGlVersion requestedGlVersion, HCURSOR cursor, HICON windowIcon );
	void InitializeWinGDI( CGlWindowSettings initialSettings, HCURSOR cursor, HICON windowIcon );
	bool IsInitialized() const;
	// Cleanup the library and the OpenGL context.
	void Cleanup();

	// Open a new main window and recreate the renderer.
	// Previous window must be closed and all the context resources must be freed.
	void ReopenMainWindow( CGlWindowSettings settings, HICON windowIcon );

private:
	// Application window.
	CPtrOwner<CGlWindow> mainWindow;
	// Active renderer.
	CPtrOwner<IRenderMechanism> renderer;
	// OpenGL rendering context. Created when OpenGL is initialized, null otherwise.
	CPtrOwner<CGlContextManager> glContextManager;
	// Manager for audio context. Created and deleted using raw pointers to support GIN_NO_AUDIO.
	Audio::CAlContextManager* alContextManager;
	// Input handler.
	CPtrOwner<CInputHandler> inputHandler;
	// Main window class.
	CPtrOwner<CWindowClass<CStandardWindowDispatcher>> windowClass;

	static CUnicodeView getMainWindowClassName();
	
	void initializeCommon( CGlWindowSettings initialSettings, HCURSOR cursor, HICON windowIcon );
	void registerWindowClass( HCURSOR cursor, HICON windowIcon, bool trackMouseLeave );
	void initOpenAL();

	// Copying is prohibited.
	CMainFrame( const CMainFrame& ) = delete;
	void operator=( const CMainFrame& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.