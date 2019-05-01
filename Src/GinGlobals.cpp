#include <common.h>
#pragma hdrstop

#include <GinGlobals.h>
#include <Application.h>
#include <GlContextManager.h>
#include <GlWindow.h>
#include <RenderMechanism.h>

namespace Gin {

CStateManager& GetStateManager()
{
	return CApplication::GetInstance()->StateManager();
}

CGlWindow& GetMainWindow()
{
	return GinInternal::GetMainFrame().GetMainGlWindow();
}

CGlContextManager& GetGlContextManager()
{
	return *CGlContextManager::GetInstance();
}

CEngine& GetEngine()
{
	return *GinInternal::GetApplication().GetEngine();
}

CPixelVector MousePixelPos()
{
	return GinInternal::GetInputHandler().GetMousePixelPos();
}

IRenderMechanism& GetRenderMechanism()
{
	return GinInternal::GetMainFrame().GetRenderer();
}

bool IsVirtualKeyDown( int virtualCode )
{
	assert( virtualCode >= 0 && virtualCode < 256 );
	return GinInternal::GetInputHandler().GetVirtualKeySet().Has( virtualCode );
}

void ExecutePostUpdate( CMutableActionOwner<void()> action )
{
	CApplication::GetInstance()->AddPostUpdateAction( move( action ) );
}

void ExecutePostDraw( CMutableActionOwner<void()> action )
{
	CApplication::GetInstance()->AddPostDrawAction( move( action ) );
}

void ExecutePostOsQueue( CMutableActionOwner<void()> action )
{
	CApplication::GetInstance()->AddPostOsQueueAction( move( action ) );
}

void ReopenMainWindow( CGlWindowSettings windowSettings, HICON windowIcon )
{
	const auto prevMainHandle = GetMainWindow().Handle();
	GinInternal::GetApplication().ReopenMainWindow( windowSettings, windowIcon );
	::DestroyWindow( prevMainHandle );
}

CAdditionalWindowInfo& CreateAdditionalWindow( CGlWindow newWindow, CPtrOwner<IRenderMechanism> renderer )
{
	return GinInternal::GetApplication().AddAdditionalWindow( move( newWindow ), move( renderer ) );
}

namespace GinInternal {

CApplication& GetApplication()
{
	return *CApplication::GetInstance();
}

CInputHandler& GetInputHandler()
{
	return GetApplication().MainFrame().InputHandler();
}

CActionListInputController& GetDefaultInputController()
{
	return GetInputHandler().GetDefaultInputController();
}

CMainFrame& GetMainFrame()
{
	return GetApplication().MainFrame();
}

}	// namespace GinInternal.

}	// namespace Gin.