#include <common.h>
#pragma hdrstop

#include <StateManager.h>
#include <GinGlobals.h>

namespace Gin {
	
//////////////////////////////////////////////////////////////////////////

CStateManager::CStateManager()
{
}

CStateManager::~CStateManager()
{
	assert( stateStack.IsEmpty() );
}

int CStateManager::StateCount() const
{
	return stateStack.Size();
}

const IState& CStateManager::GetCurrentState() const
{
	assert( !stateStack.IsEmpty() );
	return *stateStack.Last();
}

IState& CStateManager::GetCurrentState()
{
	assert( !stateStack.IsEmpty() );
	return *stateStack.Last();
}

const IState* CStateManager::TryGetCurrentState() const
{
	return stateStack.IsEmpty() ? nullptr : stateStack.Last().Ptr();
}

IState* CStateManager::TryGetCurrentState()
{
	return stateStack.IsEmpty() ? nullptr : stateStack.Last().Ptr();
}

void CStateManager::AbortStates()
{
	for( int i = stateStack.Size() - 1; i >= 0; i-- ) {
		stateStack[i]->OnAbort();
		stateStack.DeleteLast();
	}
}

void CStateManager::PopState()
{
	assert( !stateStack.IsEmpty() );
	auto popAction = [this]() { 
		ImmediatePopState();
	};

	ExecutePostUpdate( move( popAction ) );
}

void CStateManager::PushState( CPtrOwner<IState> newState )
{
	auto pushAction = [state = move( newState ), this]() mutable { 
		ImmediatePushState( move( state ) );
	};

	ExecutePostUpdate( move( pushAction ) );
}

void CStateManager::ClearStates()
{
	ExecutePostUpdate( [this]() { 
		ImmediateClearStates();
	} );
}

void CStateManager::ImmediatePopState()
{
	stateStack.DeleteLast();
	if( !stateStack.IsEmpty() ) {
		stateStack.Last()->OnWakeup();
	}
}

void CStateManager::ImmediatePushState( CPtrOwner<IState> state )
{
	if( !stateStack.IsEmpty() ) {
		stateStack.Last()->OnSleep();
	}
	addState( move( state ) );
}

void CStateManager::ImmediateClearStates()
{
	for( int i = stateStack.Size() - 1; i >= 0; i-- ) {
		stateStack.DeleteLast();
	}
}

void CStateManager::addState( CPtrOwner<IState> state )
{
	IState& stateRef = *state;
	stateStack.Add( move( state ) );
	stateRef.OnStart();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

