#pragma once
#include <Gindefs.h>
#include <DrawEnums.h>

namespace Gin {

enum TFramebufferAttachment {
	FA_Color = 0x8CE0,	// gl::COLOR_ATTACHMENT0
	FA_Depth = 0x8D00,	// gl::DEPTH_ATTACHMENT
	FA_Stencil = 0x8D20,	// gl::STENCIL_ATTACHMENT
	FA_DepthStencil = 0x821A	// gl::DEPTH_STENCIL_ATTACHMENT
};

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

// A relation between texture types and framebuffer attachment types.
template <TTextureGlFormat glFormat>
struct CFramebufferAttachementType {
	static const TFramebufferAttachment Result = FA_Color;
};

template <>
struct CFramebufferAttachementType<TGF_Depth> {
	static const TFramebufferAttachment Result = FA_Depth;
};

template <>
struct CFramebufferAttachementType<TGF_Stencil> {
	static const TFramebufferAttachment Result = FA_Stencil;
};

template <>
struct CFramebufferAttachementType<TGF_DepthStencil> {
	static const TFramebufferAttachment Result = FA_DepthStencil;
};

//////////////////////////////////////////////////////////////////////////

// Common framebuffer id operations.
class GINAPI CFramebufferOperations {
public:
	int GetId() const	
		{ return bufferId; }

	template <TTextureGlFormat glFormat>
	void AttachTexture( GinInternal::CConstTextureBase<glFormat, TBT_Texture2> texture, int level = 0 );
	template <TTextureGlFormat glFormat>
	void AttachTexture( GinInternal::CEditTextureBase<glFormat, TBT_Texture2> texture, int level = 0 );
	void AttachTexture( CTypelessTexture<TBT_Texture2> texture, TFramebufferAttachment attachment, int level = 0 );

	template <TTextureGlFormat glFormat>
	void AttachFace( GinInternal::CConstTextureBase<glFormat, TBT_CubeMap> cubeMap, TTextureCubeFace face, int level = 0 );
	template <TTextureGlFormat glFormat>
	void AttachFace( GinInternal::CEditTextureBase<glFormat, TBT_CubeMap> cubeMap, TTextureCubeFace face, int level = 0 );
	void AttachFace( CTypelessTexture<TBT_CubeMap> texture, TTextureCubeFace face, TFramebufferAttachment attachment, int level = 0 );

protected:
	explicit CFramebufferOperations( unsigned id ) : bufferId( id ) {}
	CFramebufferOperations& operator=( const CFramebufferOperations& ) = default;

	void setNewId( unsigned id )
		{ bufferId = id; }

private:
	// Unique framebuffer identifier.
	unsigned bufferId;

	void attachTexture( int textureId, TFramebufferAttachment attachment, int level );
	void attachFace( int textureId, TTextureCubeFace face, TFramebufferAttachment attachment, int level );
};

//////////////////////////////////////////////////////////////////////////

template <TTextureGlFormat glFormat>
void CFramebufferOperations::AttachTexture( GinInternal::CConstTextureBase<glFormat, TBT_Texture2> texture, int level /*= 0 */ )
{
	attachTexture( texture.GetTextureData().GetTextureId(), CFramebufferAttachementType<glFormat>::Result, level );
}

template <TTextureGlFormat glFormat>
void CFramebufferOperations::AttachTexture( GinInternal::CEditTextureBase<glFormat, TBT_Texture2> texture, int level /*= 0 */ )
{
	attachTexture( texture.GetTextureData().GetTextureId(), CFramebufferAttachementType<glFormat>::Result, level );
}

template <TTextureGlFormat glFormat>
void CFramebufferOperations::AttachFace( GinInternal::CConstTextureBase<glFormat, TBT_CubeMap> cubeMap, TTextureCubeFace face, int level /*= 0 */ )
{
	attachFace( cubeMap.GetTextureData().GetTextureId(), face, CFramebufferAttachementType<glFormat>::Result, level );
}

template <TTextureGlFormat glFormat>
void CFramebufferOperations::AttachFace( GinInternal::CEditTextureBase<glFormat, TBT_CubeMap> cubeMap, TTextureCubeFace face, int level /*= 0 */ )
{
	attachFace( cubeMap.GetTextureData().GetTextureId(), face, CFramebufferAttachementType<glFormat>::Result, level );
}

}

//////////////////////////////////////////////////////////////////////////

// Target for an off-screen drawing.
class GINAPI CFramebuffer : public GinInternal::CFramebufferOperations {
public:
	explicit CFramebuffer( unsigned id ) : CFramebufferOperations( id ) {}
};

//////////////////////////////////////////////////////////////////////////

// Framebuffer handler manager.
class GINAPI CFramebufferOwner : public GinInternal::CFramebufferOperations {
public:
	CFramebufferOwner();
	~CFramebufferOwner();

	operator CFramebuffer() const
		{ return CFramebuffer( GetId() ); }

	// Reacquire the framebuffer id. All the attachments are invalidated.
	void Reset();

private:
	static unsigned createBufferId();
	static void deleteBufferId( unsigned id );
};

//////////////////////////////////////////////////////////////////////////

enum TFrambebufferTarget {
	FT_Read = 0x8CA8,	// gl::READ_FRAMEBUFFER
	FT_Draw = 0x8CA9,	// gl::DRAW_FRAMEBUFFER
	FT_ReadDraw = 0x8D40	// gl::FRAMEBUFFER
};

// Class for making framebuffers current.
class GINAPI CFramebufferSwitcher {
public:
	explicit CFramebufferSwitcher( CFramebuffer newBuffer, TFrambebufferTarget target = FT_ReadDraw );
	~CFramebufferSwitcher();

	// Current target ids. Returns 0 if the screen is targeted.
	static int GetDrawTarget();
	static int GetReadTarget();

private:
	TFrambebufferTarget target;
	int prevReadTargetId = 0;
	int prevDrawTargetId = 0;

	static int currentDrawTarget;
	static int currentReadTarget;

	bool isBufferComplete( TFrambebufferTarget target );

	// Copying is prohibited.
	CFramebufferSwitcher( CFramebufferSwitcher& ) = delete;
	void operator=( CFramebufferSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Framebuffer clearing functions.
void GINAPI ClearFramebufferColor();
void GINAPI ClearFramebufferDepth();
void GINAPI ClearFramebufferStencil();
void GINAPI ClearFramebufferColorDepth();
void GINAPI ClearFramebufferAll();

void GINAPI SetFramebufferClearColor( CColor newValue );
void GINAPI SetFramebufferClearDepth( float newValue );

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

