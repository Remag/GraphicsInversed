#pragma once
#include <TextureOperations.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// An OpenGl texture wrapper of unspecified texture format.
template <TTextureBindingTarget target>
class CTypelessTexture : public GinInternal::CTextureData {
public:
	explicit CTypelessTexture( CSamplerObject sampler ) : CTextureData( sampler ) {}
	CTypelessTexture( unsigned id, CSamplerObject sampler ) : CTextureData( id, sampler ) {}
};

// An OpenGL texture wrapper.
template <TTextureBindingTarget target, TTextureGlFormat format>
class CTexture : public GinInternal::CConstTextureBase<format, target> {
public:
	CTexture() = default;
	CTexture( unsigned id, CSamplerObject sampler ) : GinInternal::CConstTextureBase<format, target>( id, sampler ) {}
};

// An edit interface for the texture.
template <TTextureBindingTarget target, TTextureGlFormat format>
class CEditableTexture : public GinInternal::CEditTextureBase<format, target> {
public:
	CEditableTexture() = default;
	CEditableTexture( unsigned id, CSamplerObject sampler ) : GinInternal::CEditTextureBase<format, target>( id, sampler ) {}
};

//////////////////////////////////////////////////////////////////////////

// Conversion operators that require wrapper definitions.
namespace GinInternal {

template <TTextureGlFormat format>
CConstTextureBase<format, TBT_Texture1>::operator CTypelessTexture<TBT_Texture1>() const
{
	return CTypelessTexture<TBT_Texture1>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CConstTextureBase<format, TBT_Texture2>::operator CTypelessTexture<TBT_Texture2>() const
{
	return CTypelessTexture<TBT_Texture2>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CConstTextureBase<format, TBT_TextureArray1>::operator CTypelessTexture<TBT_TextureArray1>() const
{
	return CTypelessTexture<TBT_TextureArray1>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CConstTextureBase<format, TBT_TextureArray2>::operator CTypelessTexture<TBT_TextureArray2>() const
{
	return CTypelessTexture<TBT_TextureArray2>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CConstTextureBase<format, TBT_CubeMap>::operator CTypelessTexture<TBT_CubeMap>() const
{
	return CTypelessTexture<TBT_CubeMap>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CEditTextureBase<format, TBT_Texture1>::operator CTypelessTexture<TBT_Texture1>() const
{
	return CTypelessTexture<TBT_Texture1>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CEditTextureBase<format, TBT_Texture2>::operator CTypelessTexture<TBT_Texture2>() const
{
	return CTypelessTexture<TBT_Texture2>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CEditTextureBase<format, TBT_TextureArray2>::operator CTypelessTexture<TBT_TextureArray2>() const
{
	return CTypelessTexture<TBT_TextureArray2>( textureData.GetTextureId(), textureData.GetSampler() );
}

template <TTextureGlFormat format>
CEditTextureBase<format, TBT_CubeMap>::operator CTypelessTexture<TBT_CubeMap>() const
{
	return CTypelessTexture<TBT_CubeMap>( textureData.GetTextureId(), textureData.GetSampler() );
}

}	// namespace GinInternal. 

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

