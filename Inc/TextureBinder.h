#pragma once
#include <DrawEnums.h>
#include <TextureOperations.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Switcher for texture bindings.
class GINAPI CTextureBinder {
public:
	template <TTextureGlFormat format, TTextureBindingTarget target>
	explicit CTextureBinder( GinInternal::CConstTextureBase<format, target> tex ) : CTextureBinder( target, tex.GetTextureData() ) {}
	template <TTextureGlFormat format, TTextureBindingTarget target>
	explicit CTextureBinder( GinInternal::CEditTextureBase<format, target> tex ) : CTextureBinder( target, tex.GetTextureData() ) {}

	~CTextureBinder();

	static const unsigned GetCurrentBindingId()
		{ return currentBindingId; }

private:
	// Previous texture binding.
	unsigned prevBindingId;
	TTextureBindingTarget prevTarget; 

	// Current texture bindings.
	static unsigned currentBindingId;
	static TTextureBindingTarget currentTarget;

	CTextureBinder( TTextureBindingTarget target, GinInternal::CTextureData text );
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

