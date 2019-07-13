#include <common.h>
#pragma hdrstop

#include <GinGlobals.h>
#include <Application.h>
#include <GlContextManager.h>
#include <GlWindow.h>
#include <RenderMechanism.h>
#include <InputSettingsController.h>

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

const CGlWindow* GetMouseHoverWindow()
{
	return GinInternal::GetInputHandler().GetHoverWindow();
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

void CommitInputKeyChanges( CStringPart controlSchemeName )
{
	return GinInternal::GetApplication().CommitInputKeyChanges( controlSchemeName );
}

void AttachAdditionalWindow( CPtrOwner<CGlWindow> newWindow, TWindowRendererType rendererType )
{
	return GinInternal::GetApplication().AddAdditionalWindow( move( newWindow ), rendererType );
}

CGlWindow* FindAdditionalWindow( CUnicodePart className )
{
	return GinInternal::GetApplication().FindAdditionalWindow( className );
}

//////////////////////////////////////////////////////////////////////////

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

const CInputTranslator& GetInputTranslator( CStringPart translatorName )
{
	return GetApplication().GetInputSettingsController().GetInputTranslator( translatorName );
}

}	// namespace GinInternal.

}	// namespace Gin.