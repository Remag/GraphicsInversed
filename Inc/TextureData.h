#pragma once
#include <DrawEnums.h>
#include <SamplerObject.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// Texture data wrapper. Handles the texture samplers and binding points.
class GINAPI CTextureData {
public:
	explicit CTextureData( CSamplerObject _sampler ) : sampler( _sampler ) {}
	CTextureData( int id, CSamplerObject _sampler ) : textureId( id ), sampler( _sampler ) {}

	bool IsLoaded() const
		{ return textureId != 0; }
	unsigned GetTextureId() const
		{ return textureId; }
	CSamplerObject GetSampler() const
		{ return sampler; }

	// Save the texture's id at the given binding point.
	// The binding point is written to sampler uniforms to associate them with the texture.
	void ConnectBindingPoint( TTextureBindingTarget target, int bindingPoint ) const;

	void SetTextureMipmapRange( TTextureBindingTarget target, int baseLevel, int maxLevel );

	static unsigned CreateTextureId();
	static void DeleteTextureId( int id );

	// Bind the given sampler object to the texture.
	void SetSamplerObject( CSamplerObject object );

	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer, TTextureBindingTarget target ) const;

private:
	// Unique identifier of the texture.
	int textureId = 0;
	// Sampler object associated with the texture.
	CSamplerObject sampler;
};

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

