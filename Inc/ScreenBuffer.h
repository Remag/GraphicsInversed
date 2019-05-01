#pragma once
#include <DrawEnums.h>
#include <Framebuffer.h>
#include <TextureWrappers.h>
#include <TextureOwner.h>
#include <GlWindowUtils.h>
#include <DrawMaskSwitchers.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// A framebuffer with an attached texture.
template <TTextureGlFormat screenFormat>
class CScreenBufferBase {
public:
	CVector2<int> GetSize() const
		{ return screenSize; }
		
	// Framebuffer conversion.
	operator CFramebuffer() const
		{ return CFramebuffer( screenBuffer ); }
	
	// Texture conversion.
	operator CTexture<TBT_Texture2, screenFormat>() const
		{ return CTexture<TBT_Texture2, screenFormat>( screenImage ); }
	operator CTypelessTexture<TBT_Texture2>() const
		{ return CTypelessTexture<TBT_Texture2>( screenImage ); }

protected:
	explicit CScreenBufferBase( CVector2<int> _screenSize ) : screenSize( _screenSize ) {}

	void initFramebufferColor();
	
	CVector2<int> screenSize;
	CFramebufferOwner screenBuffer;
	CTextureOwner<TBT_Texture2, screenFormat> screenImage;
};

//////////////////////////////////////////////////////////////////////////

template <TTextureGlFormat screenFormat>
void CScreenBufferBase<screenFormat>::initFramebufferColor()
{
	CTextureBinder binder( screenImage );
	screenImage.SetBuffer( screenSize );
	screenBuffer.AttachTexture( screenImage );
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// A screen buffer without depth.
template <TTextureGlFormat format>
class CScreenBuffer : public GinInternal::CScreenBufferBase<format> {
public:
	explicit CScreenBuffer( CVector2<int> size );

	// Screen resize. Changing the size invalidates the screen image.
	void SetSize( CVector2<int> newValue );
};

//////////////////////////////////////////////////////////////////////////

template <TTextureGlFormat format>
CScreenBuffer<format>::CScreenBuffer( CVector2<int> size ) :
	GinInternal::CScreenBufferBase<format>( size )
{
	this->initFramebufferColor();
}

template <TTextureGlFormat format>
void CScreenBuffer<format>::SetSize( CVector2<int> newValue )
{
	this->screenBuffer.Reset();
	this->screenSize = newValue;
	this->initFramebufferColor();
}

//////////////////////////////////////////////////////////////////////////

// A screen buffer with depth.
template <TTextureGlFormat format>
class CDepthScreenBuffer : public GinInternal::CScreenBufferBase<format> {
public:
	CDepthScreenBuffer( CVector2<int> size );

	// Screen resize. Changing the size invalidates the screen image.
	void SetSize( CVector2<int> newValue );

	CTexture<TBT_Texture2, TGF_Depth> GetDepth() const
		{ return screenDepth; }

private:
	CTextureOwner<TBT_Texture2, TGF_Depth> screenDepth;

	void initFramebufferDepth();
};

//////////////////////////////////////////////////////////////////////////

template <TTextureGlFormat format>
CDepthScreenBuffer<format>::CDepthScreenBuffer( CVector2<int> size ) : 
	GinInternal::CScreenBufferBase<format>( size )
{
	this->initFramebufferColor();
	initFramebufferDepth();
}

template <TTextureGlFormat format>
void CDepthScreenBuffer<format>::SetSize( CVector2<int> newValue )
{
	this->screenBuffer.Reset();
	this->screenSize = newValue;
	this->initFramebufferColor();
	initFramebufferDepth();
}

template <TTextureGlFormat format>
void CDepthScreenBuffer<format>::initFramebufferDepth()
{
	CTextureBinder binder( screenDepth );
	screenDepth.SetBuffer( this->screenSize );
	this->screenBuffer.AttachTexture( screenDepth );
}

//////////////////////////////////////////////////////////////////////////

// Switcher for screen size and framebuffer.
class CScreenSwitcher {
public:
	template <TTextureGlFormat format>
	explicit CScreenSwitcher( const CScreenBuffer<format>& screen ) :
		bufferSwt( screen ), viewportSwt( screen.GetSize() ), depthSwt( false ) { ClearFramebufferColor(); }
	template <TTextureGlFormat format>
	explicit CScreenSwitcher( const CDepthScreenBuffer<format>& screen ) : 
		bufferSwt( screen ), viewportSwt( screen.GetSize() ), depthSwt( true ) { ClearFramebufferColorDepth(); }

private:
	CFramebufferSwitcher bufferSwt;
	CViewportSwitcher viewportSwt;
	CDepthMaskSwitcher depthSwt;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

