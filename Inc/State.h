#pragma once

namespace Gin {

typedef float TTime;
class IRenderParameters;
//////////////////////////////////////////////////////////////////////////

// Program state. Owned by the state manager.
class IState : public IExternalObject {
public:
	IState() = default;

	// Action taken right after the state is added to the state stack.
	virtual void OnStart() = 0;
	// Action taken immediately before the state is popped from the stack.
	virtual void OnFinish() = 0;

	// Actions on every update.
	virtual void Update( TTime secondsPassed ) = 0;
	// Draw call for the state.
	virtual void Draw( const IRenderParameters& params ) const = 0;

	// Actions taken when the state is no longer current and is pushed down further into the stack.
	virtual void OnSleep() = 0;
	// Actions taken when the state is current again.
	virtual void OnWakeup() = 0;
	// Actions taken when the top level exception handler is triggered and the application is about to exit.
	virtual void OnAbort() = 0;

private:
	// Copying is prohibited.
	IState( IState& ) = delete;
	void operator=( IState& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

