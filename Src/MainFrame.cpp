#include <common.h>
#pragma hdrstop
#include <MainFrame.h>
#include <InputHandler.h>
#include <Application.h>
#include <GinGlobals.h>

#include <MainWindowDispatcher.h>
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
	renderer = CreateOwner<COpenGlRenderMechanism>( *mainWindow, *glContextManager );
	mainWindow->Show( true );
}

void CMainFrame::InitializeWinGDI( CGlWindowSettings initialSettings, HICON windowIcon )
{
	initializeCommon( initialSettings, windowIcon );
	renderer = CreateOwner<CWinGdiRenderMechanism>( *mainWindow );
	mainWindow->Show( true );
}

CUnicodeView CMainFrame::getMainWindowClassName()
{
	static const wchar_t* className = L"GinMainWindowClass";
	return className;
}

void CMainFrame::initializeCommon( CGlWindowSettings initialSettings, HICON windowIcon )
{
	registerWindowClass( windowIcon );
	mainWindow->Create( getMainWindowClassName(), initialSettings );
	inputHandler->SetNewWindow( mainWindow->Handle() );
	initOpenAL();
}

bool CMainFrame::IsInitialized() const
{
	return mainWindow->IsCreated() && renderer != nullptr
		&& inputHandler != nullptr && inputHandler->HasMouseController();
}

void CMainFrame::registerWindowClass( HICON windowIcon )
{
	assert( windowClass == nullptr );
	CMainWindowDispatcher dispatcher( *this, *mainWindow, GinInternal::GetApplication(), GetStateManager(), *inputHandler );
	windowClass = CreateOwner<CWindowClass<CMainWindowDispatcher>>( getMainWindowClassName(), windowIcon, move( dispatcher ) );
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
	mainWindow->Create( getMainWindowClassName(), settings );
	inputHandler->SetNewWindow( mainWindow->Handle() );
	renderer->ActivateWindowTarget( *mainWindow );
	renderer->OnWindowResize( mainWindow->WindowSize() );
	mainWindow->Show( true );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

