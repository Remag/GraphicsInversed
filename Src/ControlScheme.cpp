#include <common.h>
#pragma hdrstop

#include <ControlScheme.h>
#include <UtilityUserInputActions.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CControlScheme::Empty()
{
	translator.Empty();
}

void CControlScheme::AddAction( CKeyCombination keyCombination, CPtrOwner<TUserAction> action )
{
	const auto isDown = keyCombination.KeyModifier != KMT_Release;
	auto fullAction = createFullAction( move( action ), keyCombination );
	auto oldAction = translator.DetachAction( keyCombination.MainKey, isDown );
	translator.SetAction( keyCombination.MainKey, isDown, mergeAction( move( oldAction ), move( fullAction ) ) );
}

CPtrOwner<TUserAction> CControlScheme::createFullAction( CPtrOwner<TUserAction> action, CKeyCombination key ) const
{
	return key.AdditionalKey == 0 
		? move( action ) 
		: ptr_static_cast<TUserAction>( CreateOwner<CAdditionalKeyAction>( move( action ), key.AdditionalKey ) );
}

CPtrOwner<TUserAction> CControlScheme::mergeAction( CPtrOwner<TUserAction> oldAction, CPtrOwner<TUserAction> newAction ) const
{
	assert( newAction != nullptr );
	return oldAction == nullptr 
		? move( newAction ) 
		: ptr_static_cast<TUserAction>( CreateOwner<CCombinedUserInputAction>( move( oldAction ), move( newAction ) ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

