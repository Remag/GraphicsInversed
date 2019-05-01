#include <common.h>
#pragma hdrstop

#include <TextureBinder.h>
#include <TextureWrappers.h>
#include <GinError.h>

namespace Gin {

GLuint CTextureBinder::currentBindingId = 0;
TTextureBindingTarget CTextureBinder::currentTarget = TBT_Texture1;
//////////////////////////////////////////////////////////////////////////

CTextureBinder::CTextureBinder( TTextureBindingTarget target, GinInternal::CTextureData tex ) :
	prevTarget( currentTarget ),
	prevBindingId( currentBindingId )
{
	gl::BindTexture( target, tex.GetTextureId() );
	currentBindingId = tex.GetTextureId();
	currentTarget = target;
}

CTextureBinder::~CTextureBinder()
{
	const int prevId = prevBindingId;
	gl::BindTexture( prevTarget, prevId );
	currentBindingId = prevBindingId;
	currentTarget = prevTarget;
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
