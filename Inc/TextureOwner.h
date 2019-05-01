#pragma once
#include <TextureOperations.h>
#include <TextureBinder.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Texture wrapper responsible for its creation and deletion.
template <TTextureBindingTarget target, TTextureGlFormat format>
class CTextureOwner : public GinInternal::CEditTextureBase<format, target> {
public:
	// Create a texture id with no data.
	CTextureOwner();
	explicit CTextureOwner( CSamplerObject sampler ) : GinInternal::CEditTextureBase<format, target>( GinInternal::CTextureData::CreateTextureId(), sampler ) {}
	CTextureOwner( CTextureOwner&& other );
	CTextureOwner& operator=( CTextureOwner&& other );

	// Delete an associated id.
	~CTextureOwner();

private:
	// Copying is prohibited.
	CTextureOwner( CTextureOwner& ) = delete;
	void operator=( CTextureOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <TTextureBindingTarget target, TTextureGlFormat format>
CTextureOwner<target, format>::CTextureOwner() :
	GinInternal::CEditTextureBase<format, target>( GinInternal::CTextureData::CreateTextureId() )
{
	CTextureBinder binder( *this );
	this->SetMipmapRange( 0, 0 );
}

template <TTextureBindingTarget target, TTextureGlFormat format>
CTextureOwner<target, format>::CTextureOwner( CTextureOwner<target, format>&& other ) :
	GinInternal::CEditTextureBase<format, target>( other )
{
	other.invalidateTextureData();
}

template <TTextureBindingTarget target, TTextureGlFormat format>
CTextureOwner<target, format>& CTextureOwner<target, format>::operator=( CTextureOwner<target, format>&& other )
{
	GinInternal::CTextureData::DeleteTextureId( this->GetTextureData().GetTextureId() );
	GinInternal::CEditTextureBase<format, target>::operator=( other );
	other.invalidateTextureData();
	return *this;
}

template <TTextureBindingTarget target, TTextureGlFormat format>
CTextureOwner<target, format>::~CTextureOwner()
{
	GinInternal::CTextureData::DeleteTextureId( this->GetTextureData().GetTextureId() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

