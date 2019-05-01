#include <common.h>
#pragma hdrstop

#include <SamplerObject.h>
#include <GinGlobals.h>
#include <GlContextManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CSamplerOwner::CSamplerOwner() :
	samplerId( createSampler() )
{
	SetMinFilter( TF_Nearest );
}

GLuint CSamplerOwner::createSampler()
{
	GLuint result;
	assert( GetGlContextManager().HasContext() );
	gl::GenSamplers( 1, &result );
	return result;
}

CSamplerOwner::CSamplerOwner( CSamplerOwner&& other ) :
	samplerId( other.GetId() )
{
	other.samplerId = 0;
}

CSamplerOwner& CSamplerOwner::operator=( CSamplerOwner&& other )
{
	samplerId = other.GetId();
	other.samplerId = 0;
	return *this;
}

CSamplerOwner::~CSamplerOwner()
{
	DeleteSampler();
}

void CSamplerOwner::SetMinFilter( TTextureFilter value )
{
	gl::SamplerParameteri( GetId(), gl::TEXTURE_MIN_FILTER, value );
}

void CSamplerOwner::SetMagFilter( TTextureFilter value )
{
	assert( value == TF_Nearest || value == TF_Linear );
	gl::SamplerParameteri( GetId(), gl::TEXTURE_MAG_FILTER, value );
}

void CSamplerOwner::SetTextureWrap( TTextureWrapDim dim, TTextureWrapMode mode )
{
	gl::SamplerParameteri( GetId(), dim, mode );
}

void CSamplerOwner::DeleteSampler()
{
	if( IsValid() ) {
		const GLuint id = GetId();
		gl::DeleteSamplers( 1, &id );
		samplerId = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

