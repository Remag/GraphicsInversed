#pragma once
#include <GinDefs.h>
#include <State.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Manager of program state. Managers state switching and update.
class GINAPI CStateManager {
public:
	explicit CStateManager();
	~CStateManager();

	// Get current number of states in the stack.
	int StateCount() const;

	const IState& GetCurrentState() const;
	IState& GetCurrentState();
	const IState* TryGetCurrentState() const;
	IState* TryGetCurrentState();

	// Call the abort methods of every state.
	void AbortStates();

	// Pop and delete the current state.
	// The actual state change will happen after the update phase is completed.
	void PopState();
	// Create a new state and push it on the stack.
	// The actual state change will happen after the update phase is completed.
	void PushState( CPtrOwner<IState> state );
	// Delete all states.
	// The actual state change will happen after the update phase is completed.
	void ClearStates();

	// Pop the current state immediately on method call.
	void ImmediatePopState();
	// Push a new state immediately on method call.
	void ImmediatePushState( CPtrOwner<IState> state );
	// Delete all states.
	void ImmediateClearStates();

private:
	CArray<CPtrOwner<IState>> stateStack;

	void addState( CPtrOwner<IState> state );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.