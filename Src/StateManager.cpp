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
	stateStack.Last()->OnFinish();
	auto lastState = move( stateStack.Last() );
	stateStack.DeleteLast();
	if( !stateStack.IsEmpty() ) {
		stateStack.Last()->OnWakeup();
	}

	auto destroyStateAction = [state = move( lastState )]() {};
	ExecutePostUpdate( move( destroyStateAction ) );
}

void CStateManager::PushState( CPtrOwner<IState> newState )
{
	if( !stateStack.IsEmpty() ) {
		stateStack.Last()->OnSleep();
	}
	addState( move( newState ) );
}

void CStateManager::ClearStates()
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

