#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Action that calls two actions sequentially.
class CCombinedUserInputAction : public TUserAction {
public:
	CCombinedUserInputAction( CPtrOwner<TUserAction> first, CPtrOwner<TUserAction> second ) : firstAction( move( first ) ), secondAction( move( second ) ) {}

	CPtrOwner<TUserAction> DetachFirst()
		{ return move( firstAction ); }
	void SetFirst( CPtrOwner<TUserAction> newValue )
		{ firstAction = move( newValue ); }

	CPtrOwner<TUserAction> DetachSecond()
		{ return move( secondAction ); }
	void SetSecond( CPtrOwner<TUserAction> newValue )
		{ secondAction = move( newValue ); }

	virtual void Invoke() const override final
		{ firstAction->Invoke(); secondAction->Invoke(); }

private:
	CPtrOwner<TUserAction> firstAction;
	CPtrOwner<TUserAction> secondAction;
};

//////////////////////////////////////////////////////////////////////////

// Action that checks for an additional key before executing an action.
class CAdditionalKeyAction : public TUserAction {
public:
	CAdditionalKeyAction( CPtrOwner<TUserAction> _action, int _additionalKey ) : action( move( _action ) ), additionalKey( _additionalKey ) {}

	int GetAdditionalKey() const
		{ return additionalKey; }

	virtual void Invoke() const override final;

private:
	CPtrOwner<TUserAction> action;
	int additionalKey;
};

}	// namespace Gin.

