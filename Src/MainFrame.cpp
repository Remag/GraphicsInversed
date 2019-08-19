#include <common.h>
#pragma hdrstop
#include <MainFrame.h>
#include <InputHandler.h>
#include <Application.h>
#include <GinGlobals.h>

#include <StandardWindowDispatcher.h>
#include <WindowClass.h>
#include <GlWindow.h>
#include <State.h>
#include <InputHandler.h>
#include <AlContextManager.h>
#include <StateManager.h>
#include <OpenGlRenderMechanism.h>
#include <WinGdiRenderMechanism.h>
#include <GlContextManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CMainFrame::CMainFrame() :
	mainWindow( CreateOwner<CGlWindow>() ),
	inputHandler( CreateOwner<CInputHandler>() )
{
#ifndef GIN_NO_AUDIO
	alContextManager = new Audio::CAlContextManager();
#endif
}

CMainFrame::~CMainFrame()
{
#ifndef GIN_NO_AUDIO
	delete alContextManager;
#endif
}

void CMainFrame::InitializeOpenGL( CGlWindowSettings initialSettings, COpenGlVersion requestedGlVersion, HICON windowIcon )
{
	initializeCommon( initialSettings, windowIcon );
	glContextManager = CreateOwner<CGlContextManager>( requestedGlVersion );
	glContextManager->CreateContext( mainWindow->GetDeviceContext() );
	renderer = CreateOwner<COpenGlRenderMechanism>( *glContextManager );
	renderer->AttachNewWindow( *mainWindow );
	mainWindow->setRenderMechanism( *renderer );
	mainWindow->Show( true );
}

void CMainFrame::InitializeWinGDI( CGlWindowSettings initialSettings, HICON windowIcon )
{
	initializeCommon( initialSettings, windowIcon );
	renderer = CreateOwner<CWinGdiRenderMechanism>();
	renderer->AttachNewWindow( *mainWindow );
	mainWindow->setRenderMechanism( *renderer );
	mainWindow->Show( true );
}

CUnicodeView CMainFrame::getMainWindowClassName()
{
	static const wchar_t* className = L"GinMainWindowClass";
	return className;
}

void CMainFrame::initializeCommon( CGlWindowSettings initialSettings, HICON windowIcon )
{
	registerWindowClass( windowIcon, initialSettings.TrackMouseLeave );
	mainWindow->Create( *windowClass, initialSettings );
	initOpenAL();
}

bool CMainFrame::IsInitialized() const
{
	return mainWindow->IsCreated() && renderer != nullptr
		&& inputHandler != nullptr && inputHandler->HasMouseController();
}

void CMainFrame::registerWindowClass( HICON windowIcon, bool trackMouseLeave )
{
	assert( windowClass == nullptr );
	CStandardWindowDispatcher dispatcher( trackMouseLeave );
	windowClass = CreateOwner<CWindowClass<CStandardWindowDispatcher>>( getMainWindowClassName(), windowIcon, move( dispatcher ) );
}

void CMainFrame::initOpenAL()
{
#ifndef GIN_NO_AUDIO
	alContextManager->Initialize();
#endif
}

void CMainFrame::Cleanup()
{
	renderer = nullptr;
#ifndef GIN_NO_AUDIO
	alContextManager->Cleanup();
#endif
}

void CMainFrame::ReopenMainWindow( CGlWindowSettings settings, HICON )
{
	// TODO: window icon change.
	mainWindow = CreateOwner<CGlWindow>();
	mainWindow->Create( *windowClass, settings );
	renderer->AttachNewWindow( *mainWindow );
	renderer->ActivateWindowTarget();
	mainWindow->Show( true );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

