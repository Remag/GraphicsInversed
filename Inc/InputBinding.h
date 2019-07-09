#pragma once
#include <InputUtils.h>
#include <BoundUserInputAction.h>

namespace Gin {

namespace GinInternal {
	extern GINAPI CArray<CInputBinding*> ActiveFileKeyBindings;
	extern GINAPI CArray<CSystemInputBinding*> ActiveBasicKeyBindings;
}

//////////////////////////////////////////////////////////////////////////

class CSystemInputBinding {
public:
	// Create a binding that doesn't associate with a input setting file.
	// The provided key combination cannot be changed at runtime.
	template <class Callable>
	CSystemInputBinding( CStringPart _controlSchemeName, CKeyCombination _defaultKey, Callable action, CKeyCombination _defaultAltKey = CKeyCombination() );
	~CSystemInputBinding();

	CStringView GetControlSchemeName() const
		{ return controlSchemeName; }
	
	CKeyCombination GetDefaultKey() const
		{ return defaultKey; }
	CKeyCombination GetCurrentKey() const
		{ return currentKey; }
	CKeyCombination GetDefaultAltKey() const
		{ return defaultAltKey; }
	CKeyCombination GetCurrentAltKey() const
		{ return currentAltKey; }

	CPtrOwner<TUserAction> CreateUserAction() const
		{ return actionCreator(); }

private:
	CString controlSchemeName; 
	CKeyCombination defaultKey;
	CKeyCombination currentKey;
	CKeyCombination defaultAltKey;
	CKeyCombination currentAltKey;

	CActionOwner<CPtrOwner<TUserAction>()> actionCreator;

	template <class Callable>
	void initializeAction( Callable action );
};

//////////////////////////////////////////////////////////////////////////

template<class Callable>
inline CSystemInputBinding::CSystemInputBinding( CStringPart _controlSchemeName, CKeyCombination _defaultKey, Callable action, CKeyCombination _defaultAltKey ) :
	controlSchemeName( _controlSchemeName ),
	defaultKey( _defaultKey ),
	currentKey( _defaultKey ),
	defaultAltKey( _defaultAltKey ), 
	currentAltKey( _defaultAltKey ) 
{
	GinInternal::ActiveBasicKeyBindings.Add( this );
	initializeAction( action );
}

template<class Callable>
void CSystemInputBinding::initializeAction( Callable action )
{
	typedef typename Types::FunctionInfo<Callable>::ArgsTuple TArgsTuple;
	const auto creatorAction = [action](){ return ptr_static_cast<TUserAction>( CreateOwner<CUserInputAction<Callable, TArgsTuple::Size()>>( action ) ); };
	actionCreator = creatorAction;
}

inline CSystemInputBinding::~CSystemInputBinding()
{
	for( int i = GinInternal::ActiveBasicKeyBindings.Size() - 1; i >= 0; i-- ) {
		if( GinInternal::ActiveBasicKeyBindings[i] == this ) {
			GinInternal::ActiveBasicKeyBindings.DeleteAt( i );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

// Class for associating a given action with a key combination and an input setting file that contains key overrides.
class CInputBinding {
public:
	// Create a binding with an associated input file. An external name is created for the given action name.
	template <class Callable>
	CInputBinding( CStringPart _controlSchemeName, CKeyCombination _defaultKey, Callable _action, CStringPart _actionName, CKeyCombination _defaultAltKey = CKeyCombination() );
	~CInputBinding();
	
	CStringView GetControlSchemeName() const
		{ return controlSchemeName; }

	CStringView GetActionName() const
		{ return actionName; }
	CKeyCombination GetDefaultKey() const
		{ return defaultKey; }
	CKeyCombination GetCurrentKey() const
		{ return currentKey; }
	CKeyCombination GetDefaultAltKey() const
		{ return defaultAltKey; }
	CKeyCombination GetCurrentAltKey() const
		{ return currentAltKey; }

	// Key set methods. Actual actions will only be rebound after a call to CommitInputKeyChanges().
	void SetCurrentKey( CKeyCombination newKey )
		{ currentKey = newKey; }
	void SetCurrentAltKey( CKeyCombination newKey )
		{ currentAltKey = newKey; }

private:
	CString controlSchemeName; 
	CString actionName;
	CExternalNameCreator actionCreator;

	CKeyCombination defaultKey;
	CKeyCombination currentKey;
	CKeyCombination defaultAltKey;
	CKeyCombination currentAltKey;

	template <class Callable>
	auto createCreationFunction( Callable action ) const;
};

//////////////////////////////////////////////////////////////////////////

template<class Callable>
CInputBinding::CInputBinding( CStringPart _controlSchemeName, CKeyCombination _defaultKey, Callable _action, CStringPart _actionName, CKeyCombination _defaultAltKey ) :
	controlSchemeName( _controlSchemeName ),
	actionName( _actionName ),
	defaultKey( _defaultKey ),
	currentKey( _defaultKey ),
	defaultAltKey( _defaultAltKey ),
	currentAltKey( _defaultAltKey ),
	actionCreator( UnicodeStr( _actionName ), createCreationFunction( _action ) )
{
	GinInternal::ActiveFileKeyBindings.Add( this );
}

template<class Callable>
auto CInputBinding::createCreationFunction( Callable action ) const
{
	typedef typename Types::FunctionInfo<Callable>::ArgsTuple TArgsTuple;
	return [action](){ return CUserInputAction<Callable, TArgsTuple::Size()>( action ); };
}

inline CInputBinding::~CInputBinding()
{
	for( int i = GinInternal::ActiveFileKeyBindings.Size() - 1; i >= 0; i-- ) {
		if( GinInternal::ActiveFileKeyBindings[i] == this ) {
			GinInternal::ActiveFileKeyBindings.DeleteAt( i );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

