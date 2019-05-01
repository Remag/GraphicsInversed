#pragma once

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

// Class that maintains user input action targets.
class GINAPI CActionTargetController {
public:
	static int GetOrCreateTargetIndex( const type_info& info );

	static void SetActionTarget( void* target, int index )
		{ actionTargets[index] = target; }
	static void* GetActionTarget( int index )
		{ return actionTargets[index]; }

private:
	static CMap<const type_info*, int> targetTypeToIndex;
	static CArray<void*> actionTargets;
};

//////////////////////////////////////////////////////////////////////////

inline int CActionTargetController::GetOrCreateTargetIndex( const type_info& info )
{
	const int targetSize = actionTargets.Size();
	const int result = targetTypeToIndex.GetOrCreate( &info, targetSize ).Value();
	if( result == targetSize ) {
		actionTargets.Add( nullptr );
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

// Action that calls a specified callable with the correct bound arguments type.
template <class CallableType, int argCount>
class CUserInputAction : public TUserAction {
public:
	typedef typename Types::FunctionInfo<CallableType>::ArgsTuple TArgsTuple;
	staticAssert( argCount == TArgsTuple::Size() );

	explicit CUserInputAction( CallableType _callable ) : 
		callable( move( _callable ) ) { registerTargetIndexes<0>( argTargetIndexes.Ptr(), Types::BoolType<TArgsTuple::Size() == 0>() ); }

	virtual void Invoke() const override final;

private:
	CallableType callable;
	CStackArray<int, TArgsTuple::Size()> argTargetIndexes;

	template <int argPos, int argCount>
	void registerTargetIndexes( int ( &indexes )[argCount], Types::FalseType endMarker );
	template <int argPos, int argCount>
	void registerTargetIndexes( int ( &indexes )[argCount], Types::TrueType endMarker );

	template <int argPos, int argCount, class... Args>
	void invokeCallableWithActionTargets( const int ( &indexes )[argCount], Types::FalseType endMarker, Args&... args ) const;
	template <int argPos, int argCount, class... Args>
	void invokeCallableWithActionTargets( const int ( &indexes )[argCount], Types::TrueType endMarker, Args&... args ) const;
};

// Input action specialization for callables with no arguments.
template <class CallableType>
class CUserInputAction<CallableType, 0> : public TUserAction {
public:
	explicit CUserInputAction( CallableType _callable ) : callable( _callable ) {}

	virtual void Invoke() const override final
		{ callable(); }

private:
	CallableType callable;
};

//////////////////////////////////////////////////////////////////////////

// Input action creation function.
template <class CallableType>
auto CreateUserInputAction( CallableType&& callable )
{
	typedef typename Types::FunctionInfo<CallableType>::ArgsTuple TArgsTuple;
	return CreateOwner<CUserInputAction<CallableType, TArgsTuple::Size()>>( forward<CallableType>( callable ) );
}

//////////////////////////////////////////////////////////////////////////

template <class CallableType, int args>
template <int argPos, int argCount>
void CUserInputAction<CallableType, args>::registerTargetIndexes( int( &indexes )[argCount], Types::FalseType )
{
	staticAssert( argPos < argCount );
	typedef	TArgsTuple::template Elem<argPos> TArg;
	indexes[argPos] = GinInternal::CActionTargetController::GetOrCreateTargetIndex( typeid( TArg ) );
	registerTargetIndexes<argPos + 1>( indexes, Types::BoolType<argPos + 1 == argCount>() );
}

template <class CallableType, int args>
template <int argPos, int argCount>
void CUserInputAction<CallableType, args>::registerTargetIndexes( int( & )[argCount], Types::TrueType )
{
}

template <class CallableType, int args>
void CUserInputAction<CallableType, args>::Invoke() const
{
	invokeCallableWithActionTargets<0>( argTargetIndexes.Ptr(), Types::BoolType<TArgsTuple::Size() == 0>() );
}

template <class CallableType, int args>
template <int argPos, int argCount, class... Args>
void CUserInputAction<CallableType, args>::invokeCallableWithActionTargets( const int( &indexes )[argCount], Types::FalseType, Args&... args ) const
{
	typedef	TArgsTuple::template Elem<argPos> TArg;
	const int argIndex = indexes[argPos];
	void* argPtr = GinInternal::CActionTargetController::GetActionTarget( argIndex );
	if( argPtr == nullptr ) {
		return;
	}
	auto& arg = *static_cast<typename Types::PureType<TArg>::Result*>( argPtr );
	invokeCallableWithActionTargets<argPos + 1>( indexes, Types::BoolType<argPos + 1 == argCount>(), args..., arg );
}

template <class CallableType, int args>
template <int argPos, int argCount, class... Args>
void CUserInputAction<CallableType, args>::invokeCallableWithActionTargets( const int( & )[argCount], Types::TrueType, Args&... args ) const
{
	Relib::Invoke( callable, args... );
}

//////////////////////////////////////////////////////////////////////////

// Utility class for creating input actions from a given callable.
class CUserActionCreator {
public:
	// Register an action with the given callable.
	template <class Callable>
	CUserActionCreator( CUnicodeView actionName, Callable callable );

	// Register two action with given callables.
	// The first action adds a prefix of '+' to actionName.
	// The second action adds a prefix of '-' to actionName.
	template <class StartCallable, class ReleaseCallable>
	CUserActionCreator( CUnicodeView actionName, StartCallable onStart, ReleaseCallable onRelease );

private:
	CExternalNameCreator nameCreator;
	COptional<CExternalNameCreator> releaseNameCreator;
	
	template <class CallableType>
	static auto getUserActionCreator( CallableType callable );
};

//////////////////////////////////////////////////////////////////////////

template <class CallableType>
auto CUserActionCreator::getUserActionCreator( CallableType callable )
{
	typedef typename Types::FunctionInfo<CallableType>::ArgsTuple TArgsTuple;
	return [callable](){ return CUserInputAction<CallableType, TArgsTuple::Size()>( callable ); };
}

template <class CallableType>
CUserActionCreator::CUserActionCreator( CUnicodeView actionName, CallableType callable ) :
	nameCreator( actionName, getUserActionCreator( callable ) )
{
}

template <class StartCallable, class ReleaseCallable>
CUserActionCreator::CUserActionCreator( CUnicodeView actionName, StartCallable onStart, ReleaseCallable onRelease ) :
	nameCreator( L'+' + actionName, getUserActionCreator( onStart ) ),
	releaseNameCreator( L'-' + actionName, getUserActionCreator( onRelease ) )
{
}

//////////////////////////////////////////////////////////////////////////

// Switcher for current action targets.
class CActionTargetSwitcher {
public:
	template <class Target>
	explicit CActionTargetSwitcher( Target* target );
	~CActionTargetSwitcher();

private:
	int targetIndex;
	void* prevTarget;
};

//////////////////////////////////////////////////////////////////////////

template <class Target>
CActionTargetSwitcher::CActionTargetSwitcher( Target* target ) :
	targetIndex( GinInternal::CActionTargetController::GetOrCreateTargetIndex( typeid( Target ) ) ),
	prevTarget( GinInternal::CActionTargetController::GetActionTarget( targetIndex ) )
{
	GinInternal::CActionTargetController::SetActionTarget( target, targetIndex );
}

inline CActionTargetSwitcher::~CActionTargetSwitcher()
{
	GinInternal::CActionTargetController::SetActionTarget( prevTarget, targetIndex );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

