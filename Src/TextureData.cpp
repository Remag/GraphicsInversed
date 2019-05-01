#include <common.h>
#pragma hdrstop

#include <TextureData.h>
#include <GinError.h>
#include <GlBuffer.h>
#include <TextureBinder.h>

namespace Gin {
//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

void CTextureData::SetSamplerObject( CSamplerObject object )
{
	sampler = object;
}

void CTextureData::ConnectBindingPoint( TTextureBindingTarget target, int bindingPoint ) const
{
	assert( bindingPoint > 0 );
	// Bind the texture to the binding point.
	gl::ActiveTexture( gl::TEXTURE0 + bindingPoint );
	gl::BindTexture( target, GetTextureId() );
	CheckGlError();
	gl::ActiveTexture( gl::TEXTURE0 );

	gl::BindSampler( bindingPoint, sampler.GetId() );
}

// Set texture's base level and max level of mipmaps. Texture must be bound to its target.
void CTextureData::SetTextureMipmapRange( TTextureBindingTarget target, int baseLevel, int maxLevel )
{
	assert( CTextureBinder::GetCurrentBindingId() == GetTextureId() );
	assert( baseLevel >= 0 && maxLevel >= 0 );
	assert( maxLevel >= baseLevel );
	gl::TexParameteri( target, gl::TEXTURE_BASE_LEVEL, baseLevel );
	gl::TexParameteri( target, gl::TEXTURE_MAX_LEVEL, maxLevel );
	CheckGlError();
}

void CTextureData::GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer, TTextureBindingTarget target ) const
{
	assert( CTextureBinder::GetCurrentBindingId() == GetTextureId() );
	CBufferObjectBinder binder( pixelBuffer );
	gl::GetTexImage( target, level, cFormat, type, 0 );
	CheckGlError();
}

unsigned CTextureData::CreateTextureId()
{
	assert( GetGlContextManager().HasContext() );
	GLuint result;
	gl::GenTextures( 1, &result );
	CheckGlError();
	return result;
}

void CTextureData::DeleteTextureId( int textureId )
{
	if( textureId != 0 ) {
		assert( GetGlContextManager().HasContext() );
		const unsigned id = textureId;
		gl::DeleteTextures( 1, &id );
	}
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
