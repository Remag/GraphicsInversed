#include <common.h>
#pragma hdrstop

#include <Application.h>
#include <Engine.h>
#include <StateManager.h>
#include <GlWindow.h>
#include <AlContextManager.h>
#include <RenderMechanism.h>
#include <MainFrame.h>
#include <AdditionalWindowInfo.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CApplication::CApplication()
{

}

CApplication::~CApplication()
{
	assert( postDrawActions.IsEmpty() );
	assert( postUpdateActions.IsEmpty() );
	assert( postOsQueueActions.IsEmpty() );
}

void CApplication::SetEngine( CPtrOwner<CEngine> newEngine )
{
	assert( newEngine != nullptr );
	engine = move( newEngine );
}

void CApplication::ReopenMainWindow( CGlWindowSettings settings, HICON icon )
{
	cleanupGlContext();
	mainFrame.ReopenMainWindow( settings, icon );
	onInitializeGlContext();
}

bool CApplication::TryCloseMainWindow()
{
	if( checkMainWindowClose() ) {
		onMainWindowClose();
		return true;
	}
	return false;
}

bool CApplication::TryQuitOnWindowDestruction( HWND destroyedWindow )
{
	const auto currentMainWindow = mainFrame.GetMainGlWindow().Handle();
	if( currentMainWindow == destroyedWindow && checkApplicationDestruction() ) {
		onApplicationDestruction();
		stateManager->ClearStates();
		return true;
	}
	return false;
}

CAdditionalWindowInfo& CApplication::AddAdditionalWindow( CGlWindow window, CPtrOwner<IRenderMechanism> renderer )
{
	return additionalWindows.Add( move( window ), move( renderer ) );
}

void CApplication::DeleteAdditionalWindow( const CAdditionalWindowInfo& info )
{
	for( int i = additionalWindows.Size() - 1; i >= 0; i-- ) {
		if( &additionalWindows[i] == &info ) {
			additionalWindows.DeleteAt( i );
			return;
		}
	}
	assert( false );
}

bool CApplication::Initialize( CUnicodeView commandLine )
{
	stateManager = CreateOwner<CStateManager>();
	auto firstState = onInitialize( commandLine );
	if( firstState == nullptr ) {
		return false;
	}
	onInitializeGlContext();
	// Engine must be present.
	assert( engine != nullptr );
	// Mainframe must be initialized.
	assert( mainFrame.IsInitialized() );

	stateManager->ImmediatePushState( move( firstState ) );
	return true;
}

void CApplication::Run( CUnicodeView commandLine )
{
	if( !Initialize( commandLine ) ) {
		return;
	}
	try {
		while( runWindowsLoop() ) {
			runUpdateLoop();
		}
		
		finalizeApplication();

	} catch( const CException& e ) {
		Log::Exception( e );
		stateManager->AbortStates();
		postDrawActions.Empty();
		postUpdateActions.Empty();
		postOsQueueActions.Empty();
	}
#ifndef GIN_NO_AUDIO
	mainFrame.AlContextManager().StopAllRecords();
#endif
	onExit();
	mainFrame.Cleanup();
}

bool CApplication::runWindowsLoop()
{
	MSG msg;
	while( ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) != 0 ) {
		if( msg.message == WM_QUIT ) {
			return false;
		}
		if( mainFrame.InputHandler().IsInTextMode() ) {
			::TranslateMessage( &msg );
		}
		::DispatchMessage( &msg );
	}
	return true;
}

void CApplication::runUpdateLoop()
{
	executeActions( postOsQueueActions );
	const auto frameInfo = engine->AdvanceFrame();

	if( frameInfo.RunUpdate ) {
		stateManager->GetCurrentState().Update( frameInfo.Step );
		executeActions( postUpdateActions );
		mainFrame.InputHandler().OnFrameEnd();
	}

	// Check if a draw is still possible after an update.
	if( stateManager->StateCount() <= 0 ) {
		return;
	}

	if( frameInfo.RunDraw ) {
		const auto& renderer = mainFrame.GetRenderer();
		const auto& currentState = stateManager->GetCurrentState();
		renderer.OnDraw( currentState );
		executeActions( postDrawActions );
		renderer.OnPostDraw( mainFrame.GetMainGlWindow() );
		drawAdditionalWindows( currentState );
		// This assert most likely fires if a post-update action is added during the draw phase.
		assert( postUpdateActions.IsEmpty() );
	}
}

void CApplication::drawAdditionalWindows( const IState& currentState ) const
{
	for( const auto& window : additionalWindows ) {
		auto& renderer = const_cast<IRenderMechanism&>( *window.Renderer );
		renderer.ActivateWindowTarget( window.Window );
		renderer.OnDraw( currentState );
		renderer.OnPostDraw( window.Window );
	}

	if( !additionalWindows.IsEmpty() ) {
		auto& renderer = const_cast<IRenderMechanism&>( mainFrame.GetRenderer() );
		renderer.ActivateWindowTarget( mainFrame.GetMainGlWindow() );
	}
}

void CApplication::finalizeApplication()
{
	executeActions( postOsQueueActions );
	executeActions( postDrawActions );
	executeActions( postUpdateActions );
}

void CApplication::executeActions( CArray<CMutableActionOwner<void()>>& actions )
{
	// Action execution can lead to new actions being added, we need to reload the size after each iteration.
	for( int i = 0; i< actions.Size(); i++ ) {
		actions[i].Invoke();
	}
	actions.Empty();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.