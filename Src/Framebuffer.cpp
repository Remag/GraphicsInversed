#include <common.h>
#pragma hdrstop

#include <Framebuffer.h>
#include <TextureWrappers.h>
#include <GinError.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

void CFramebufferOperations::AttachTexture( CTypelessTexture<TBT_Texture2> texture, TFramebufferAttachment attachment, int level /*= 0 */ )
{
	attachTexture( texture.GetTextureId(), attachment, level );
}

void CFramebufferOperations::AttachFace( CTypelessTexture<TBT_CubeMap> texture, TTextureCubeFace face, TFramebufferAttachment attachment, int level /*= 0 */ )
{
	attachFace( texture.GetTextureId(), face, attachment, level );
}

void CFramebufferOperations::attachTexture( int textureId, TFramebufferAttachment type, int level )
{
	const int prevDrawBufferId = CFramebufferSwitcher::GetDrawTarget();
	gl::BindFramebuffer( FT_Draw, bufferId );
	gl::FramebufferTexture2D( FT_ReadDraw, type, TBT_Texture2, textureId, level );

	gl::BindFramebuffer( FT_Draw, prevDrawBufferId );
	CheckGlError();
}

void CFramebufferOperations::attachFace( int textureId, TTextureCubeFace face, TFramebufferAttachment type, int level )
{
	gl::FramebufferTexture2D( FT_ReadDraw, type, face, textureId, level );
	CheckGlError();
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

CFramebufferOwner::CFramebufferOwner() :
	CFramebufferOperations( createBufferId() )
{
}

unsigned CFramebufferOwner::createBufferId()
{
	unsigned result;
	gl::GenFramebuffers( 1, &result );
	CheckGlError();
	return result;
}

void CFramebufferOwner::deleteBufferId( unsigned id )
{
	gl::DeleteFramebuffers( 1, &id );
	CheckGlError();
}

CFramebufferOwner::~CFramebufferOwner()
{
	deleteBufferId( GetId() );
}

void CFramebufferOwner::Reset()
{
	deleteBufferId( GetId() );
	setNewId( createBufferId() );
}

//////////////////////////////////////////////////////////////////////////

int CFramebufferSwitcher::currentReadTarget = 0;
int CFramebufferSwitcher::currentDrawTarget = 0;

CFramebufferSwitcher::CFramebufferSwitcher( CFramebuffer newBuffer, TFrambebufferTarget _target /*= FT_ReadDraw*/ ) :
	target( _target )
{
	gl::BindFramebuffer( target, newBuffer.GetId() );
	CheckGlError();

	switch( target ) {
		case FT_Draw:
			prevDrawTargetId = currentDrawTarget;
			currentDrawTarget = newBuffer.GetId();
			break;
		case FT_Read:
			prevReadTargetId = currentReadTarget;
			currentReadTarget = newBuffer.GetId();
			break;
		case FT_ReadDraw:
			prevReadTargetId = currentReadTarget;
			prevDrawTargetId = currentDrawTarget;
			currentDrawTarget = newBuffer.GetId();
			currentReadTarget = newBuffer.GetId();
			break;
		default:
			assert( false );
			break;
	}
}

bool CFramebufferSwitcher::isBufferComplete( TFrambebufferTarget targetBuffer )
{
	const GLenum status = gl::CheckFramebufferStatus( targetBuffer );
	return status == gl::FRAMEBUFFER_COMPLETE;
}

CFramebufferSwitcher::~CFramebufferSwitcher()
{
	switch( target ) {
		case FT_Draw:
			gl::BindFramebuffer( FT_Draw, prevDrawTargetId );
			currentDrawTarget = prevDrawTargetId;
			break;
		case FT_Read:
			gl::BindFramebuffer( FT_Read, prevReadTargetId );
			currentReadTarget = prevReadTargetId;
			break;
		case FT_ReadDraw:
			if( prevDrawTargetId == prevReadTargetId ) {
				gl::BindFramebuffer( FT_ReadDraw, prevDrawTargetId );
			} else {
				gl::BindFramebuffer( FT_Draw, prevDrawTargetId );
				gl::BindFramebuffer( FT_Read, prevReadTargetId );
			}
			currentReadTarget = prevReadTargetId;
			currentDrawTarget = prevDrawTargetId;
			break;
		default:
			assert( false );
			break;
	}
}

int CFramebufferSwitcher::GetDrawTarget()
{
	return currentDrawTarget;
}

int CFramebufferSwitcher::GetReadTarget()
{
	return currentReadTarget;
}

//////////////////////////////////////////////////////////////////////////

void ClearFramebufferColor()
{	 
	gl::Clear( gl::COLOR_BUFFER_BIT );
}	 
	 
void ClearFramebufferDepth()
{	 
	gl::Clear( gl::DEPTH_BUFFER_BIT );
}	 
	 
void ClearFramebufferStencil()
{	 
	gl::Clear( gl::STENCIL_BUFFER_BIT );
}	 
	 
void ClearFramebufferColorDepth()
{	 
	gl::Clear( gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT );
}	 
	 
void ClearFramebufferAll()
{
	gl::Clear( gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT );
}

void SetFramebufferClearColor( CColor newValue )
{
	gl::ClearColor( newValue.GetRed(), newValue.GetGreen(), newValue.GetBlue(), newValue.GetAlpha() );
}

void SetFramebufferClearDepth( float newValue )
{
	gl::ClearDepth( newValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
