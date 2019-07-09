#pragma once
#include <InputUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class representing a set of controls with associated bindings.
// Creates correct actions for key combinations and merges actions with the same key together.
class GINAPI CControlScheme {
public:
	const CInputTranslator& GetTranslator() const
		{ return translator; }

	void Empty();
	void AddAction( CKeyCombination keyCombination, CPtrOwner<TUserAction> action );

private:
	CInputTranslator translator;

	CPtrOwner<TUserAction> createFullAction( CPtrOwner<TUserAction> action, CKeyCombination keyCombination ) const;
	CPtrOwner<TUserAction> mergeAction( CPtrOwner<TUserAction> oldAction, CPtrOwner<TUserAction> newAction ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

