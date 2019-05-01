#pragma once
#include <GinDefs.h>
#include <DrawEnums.h>
#include <TypelessTextureOperations.h>
#include <DefaultSamplerContainer.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

// Common operations on the texture object.
template <TTextureGlFormat format, TTextureBindingTarget bindingTarget>
class CConstTextureBase {};
template <TTextureGlFormat format, TTextureBindingTarget bindingTarget>
class CEditTextureBase {};

//////////////////////////////////////////////////////////////////////////

// Operations on the one dimensional texture.
template <TTextureGlFormat format>
class CConstTextureBase<format, TBT_Texture1> {
public:
	operator CTexture<TBT_Texture1, format>() const
		{ return CTexture<TBT_Texture1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_Texture1>() const;

	CTextureData GetTextureData() const
		{ return textureData; }

protected:
	CConstTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CConstTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CConstTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}

private:
	CTextureData textureData;
};

template <TTextureGlFormat format>
class CEditTextureBase<format, TBT_Texture1> {
public:
	operator CTexture<TBT_Texture1, format>() const
		{ return CTexture<TBT_Texture1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CEditableTexture<TBT_Texture1, format>()
		{ return CEditableTexture<TBT_Texture1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_Texture1>() const;
	
	CTextureData GetTextureData() const
		{ return textureData; }

	void SetSamplerObject( CSamplerObject newValue )
		{ textureData.SetSamplerObject( newValue ); }

	// Set the image data from the designated container.
	// Internal format is a recommendation for a type of storage to use.
	// It is ignored if data is compressed.
	void SetImageData( const CImageData& data, TTextureGlFormat internalFormat = TGF_RGBA )
		{ return CTypelessTextureOperations<TBT_Texture1>::SetImageData( textureData, data, internalFormat ); }

	// Set the range of mipmap levels used by the texture.
	// The default values are both 0.
	void SetMipmapRange( int baseValue, int maxValue )
		{ textureData.SetTextureMipmapRange( TBT_Texture1, baseValue, maxValue ); }

	// Reserve memory for the first level of the texture.
	void SetBuffer( int size )
		{ setBuffer( size, 0, format ); }

	// Load the texture data directly.
	// data - the array to load.
	// size - size of the texture in texels.
	// level - the mipmap level to load.
	// cFormat - the data format.
	// type - type used by data.
	void SetData( const void* data, int size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_Texture1>::SetData( textureData, data, size, level, format, cFormat, type ); }
	void SetSubData( int offset, const void* data, int size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_Texture1>::SetSubData( textureData, offset, data, size, level, cFormat, type ); }

protected:
	CEditTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CEditTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CEditTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
	void invalidateTextureData()
		{ textureData = CTextureData{ 0, textureData.GetSampler() }; }

private:
	CTextureData textureData;
};

//////////////////////////////////////////////////////////////////////////

// Operations on the two dimensional texture.
template <TTextureGlFormat format>
class CConstTextureBase<format, TBT_Texture2> {
public:
	operator CTexture<TBT_Texture2, format>() const
		{ return CTexture<TBT_Texture2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_Texture2>() const;
	
	CTextureData GetTextureData() const
		{ return textureData; }
	
	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_Texture2 ); }

protected:
	CConstTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CConstTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CConstTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}

private:
	CTextureData textureData;
};

template <TTextureGlFormat format>
class CEditTextureBase<format, TBT_Texture2> {
public:
	operator CTexture<TBT_Texture2, format>() const
		{ return CTexture<TBT_Texture2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_Texture2>() const;
	operator CEditableTexture<TBT_Texture2, format>()
		{ return CEditableTexture<TBT_Texture2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
		
	CTextureData GetTextureData() const
		{ return textureData; }
		
	void SetSamplerObject( CSamplerObject newValue )
		{ textureData.SetSamplerObject( newValue ); }

	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_Texture2 ); }

	// Set the image data from the designated container.
	// Internal format is a recommendation for a type of storage to use.
	// It is ignored if data is compressed.
	void SetImageData( const CImageData& data, TTextureGlFormat internalFormat = TGF_RGBA )
		{ CTypelessTextureOperations<TBT_Texture2>::SetImageData( textureData, data, internalFormat ); }

	// Set the range of mipmap levels used by the texture.
	// The default values are both 0.
	void SetMipmapRange( int baseValue, int maxValue )
		{ textureData.SetTextureMipmapRange( TBT_Texture2, baseValue, maxValue ); }

	// Reserve memory for the first level of the texture.
	void SetBuffer( CVector2<int> size )
		{ CTypelessTextureOperations<TBT_Texture2>::SetBuffer( textureData, size, 0, format ); }
	// Increase buffer size without of the first level without destroying the texture contents.
	void GrowBuffer( CVector2<int> prevSize, CVector2<int> newSize, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_Texture2>::GrowBuffer( textureData, prevSize, newSize, 0, format, cFormat, type ); }
	// Load the texture data directly.
	// data - the array to load.
	// size - size of the texture in texels.
	// level - the mipmap level to load.
	// cFormat - the data format.
	// type - type used by data.
	void SetData( const void* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_Texture2>::SetData( textureData, data, size, level, format, cFormat, type ); }
	// Load part of the data at a given offset.
	void SetSubData( CVector2<int> offset, const void* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_Texture2>::SetSubData( textureData, offset, data, size, level, cFormat, type ); }

protected:
	CEditTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CEditTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CEditTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
	void invalidateTextureData()
		{ textureData = CTextureData{ 0, textureData.GetSampler() }; }

private:
	CTextureData textureData;
};

//////////////////////////////////////////////////////////////////////////
// Operations on texture arrays.

template <TTextureGlFormat format>
class CConstTextureBase<format, TBT_TextureArray1> {
public:
	operator CTexture<TBT_TextureArray1, format>() const
		{ return CTexture<TBT_TextureArray1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_TextureArray1>() const;
	
	CTextureData GetTextureData() const
		{ return textureData; }
	
	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_TextureArray1 ); }

protected:
	CConstTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CConstTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CConstTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}

private:
	CTextureData textureData;
};

template <TTextureGlFormat format>
class CEditTextureBase<format, TBT_TextureArray1> {
public:
	operator CTexture<TBT_TextureArray1, format>() const
		{ return CTexture<TBT_TextureArray1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_TextureArray1>() const;
	operator CEditableTexture<TBT_TextureArray1, format>()
		{ return CEditableTexture<TBT_TextureArray1, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
		
	CTextureData GetTextureData() const
		{ return textureData; }

	void SetSamplerObject( CSamplerObject newValue )
		{ textureData.SetSamplerObject( newValue ); }

	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_TextureArray1 ); }

	// Set the image data from the designated container.
	// Internal format is a recommendation for a type of storage to use.
	// It is ignored if data is compressed.
	void SetImageData( const CImageData& data, TTextureGlFormat internalFormat = TGF_RGBA )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetImageData( textureData, data, internalFormat ); }

	// Set the data for the given array index.
	// Storage for the texture must be preallocated.
	void SetSubImageData( const CImageData& data, int index )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetSubImageData( textureData, data, index ); }

	// Set the range of mipmap levels used by the texture.
	// The default values are both 0.
	void SetMipmapRange( int baseValue, int maxValue )
		{ textureData.SetTextureMipmapRange( TBT_TextureArray1, baseValue, maxValue ); }

	// Reserve memory for the first level of the texture.
	void SetBuffer( CVector2<int> size )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetBuffer( textureData, size, 0, format ); }
	// Increase buffer size without of the first level without destroying the texture contents.
	void GrowBuffer( CVector2<int> prevSize, CVector2<int> newSize, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray1>::GrowBuffer( textureData, prevSize, newSize, 0, format, cFormat, type ); }
	// Load the texture data directly.
	// data - the array to load.
	// size - size of the texture in texels.
	// level - the mipmap level to load.
	// glFormat - the OpenGL format of the sampler.
	// cFormat - the data format.
	// type - type used by data.
	void SetData( const BYTE* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetData( textureData, data, size, level, format, cFormat, type ); }
	// Set a single slice of the array. Texture data must be allocated beforehand.
	void SetSliceData( int sliceIndex, CVector2<int> sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetSliceData( textureData, sliceIndex, sliceSize, data, level, cFormat, type ); }
	// Load part of the data at a given offset.
	void SetSubData( CVector3<int> offset, const BYTE* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray1>::SetSubData( textureData, offset, data, size, level, cFormat, type ); }

protected:
	CEditTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CEditTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CEditTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
	void invalidateTextureData()
		{ textureData = CTextureData{ 0, textureData.GetSampler() }; }

private:
	CTextureData textureData;
};

//////////////////////////////////////////////////////////////////////////

// Operations on 2D texture arrays.
template <TTextureGlFormat format>
class CConstTextureBase<format, TBT_TextureArray2> {
public:
	operator CTexture<TBT_TextureArray2, format>() const
		{ return CTexture<TBT_TextureArray2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_TextureArray2>() const;
	
	CTextureData GetTextureData() const
		{ return textureData; }
	
	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_TextureArray2 ); }

protected:
	CConstTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CConstTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CConstTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}

private:
	CTextureData textureData;
};

template <TTextureGlFormat format>
class CEditTextureBase<format, TBT_TextureArray2> {
public:
	operator CTexture<TBT_TextureArray2, format>() const
		{ return CTexture<TBT_TextureArray2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_TextureArray2>() const;
	operator CEditableTexture<TBT_TextureArray2, format>()
		{ return CEditableTexture<TBT_TextureArray2, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
		
	CTextureData GetTextureData() const
		{ return textureData; }

	void SetSamplerObject( CSamplerObject newValue )
		{ textureData.SetSamplerObject( newValue ); }

	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, cFormat, type, pixelBuffer, TBT_TextureArray2 ); }

	// Set the image data from the designated container.
	// Internal format is a recommendation for a type of storage to use.
	// It is ignored if data is compressed.
	void SetImageData( const CImageData& data, TTextureGlFormat internalFormat = TGF_RGBA )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetImageData( textureData, data, internalFormat ); }

	// Set the data for the given array index.
	// Storage for the texture must be preallocated.
	void SetSubImageData( const CImageData& data, int index )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetSubImageData( textureData, data, index ); }

	// Set the range of mipmap levels used by the texture.
	// The default values are both 0.
	void SetMipmapRange( int baseValue, int maxValue )
		{ textureData.SetTextureMipmapRange( TBT_TextureArray2, baseValue, maxValue ); }

	// Reserve memory for the first level of the texture.
	void SetBuffer( CVector3<int> size )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetBuffer( textureData, size, 0, format ); }
	// Increase buffer size without of the first level without destroying the texture contents.
	void GrowBuffer( CVector3<int> prevSize, CVector3<int> newSize, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray2>::GrowBuffer( textureData, prevSize, newSize, 0, format, cFormat, type ); }
	// Load the texture data directly.
	// data - the array to load.
	// size - size of the texture in texels.
	// level - the mipmap level to load.
	// glFormat - the OpenGL format of the sampler.
	// cFormat - the data format.
	// type - type used by data.
	void SetData( const BYTE* data, CVector3<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetData( textureData, data, size, level, format, cFormat, type ); }
	// Set a single slice of the array. Texture data must be allocated beforehand.
	void SetSliceData( int sliceIndex, CVector2<int> sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetSliceData( textureData, sliceIndex, sliceSize, data, level, cFormat, type ); }
	// Load part of the data at a given offset.
	void SetSubData( CVector3<int> offset, const BYTE* data, CVector3<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_TextureArray2>::SetSubData( textureData, offset, data, size, level, cFormat, type ); }

protected:
	CEditTextureBase() : textureData( GetDefaultSampler() ) {}
	explicit CEditTextureBase( unsigned id ) : textureData( id, GetDefaultSampler() ) {}
	CEditTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
	void invalidateTextureData()
		{ textureData = CTextureData{ 0, textureData.GetSampler() }; }

private:
	CTextureData textureData;
};

//////////////////////////////////////////////////////////////////////////

// Operations on cube maps.
template <TTextureGlFormat format>
class CConstTextureBase<format, TBT_CubeMap> {
public:
	operator CTexture<TBT_CubeMap, format>() const
		{ return CTexture<TBT_CubeMap, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_CubeMap>() const;
	
	CTextureData GetTextureData() const
		{ return textureData; }

	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTextureCubeFace face, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, face, cFormat, type, pixelBuffer, TBT_CubeMap ); }

protected:
	CConstTextureBase() : textureData( CTextureData::DefaultCubeMapSampler() ) {}
	explicit CConstTextureBase( unsigned id ) : textureData( id, CTextureData::DefaultCubeMapSampler() ) {}
	CConstTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
private:
	CTextureData textureData;
};

template <TTextureGlFormat format>
class CEditTextureBase<format, TBT_CubeMap> {
public:
	operator CTexture<TBT_CubeMap, format>() const
		{ return CTexture<TBT_CubeMap, format>( textureData.GetTextureId(), textureData.GetSampler() ); }
	operator CTypelessTexture<TBT_CubeMap>() const;
	
	operator CEditableTexture<TBT_CubeMap, format>()
		{ return CEditableTexture<TBT_CubeMap, format>( this->GetTextureId(), this->GetSampler() ); }
		
	CTextureData GetTextureData() const
		{ return textureData; }

	void SetSamplerObject( CSamplerObject newValue )
		{ textureData.SetSamplerObject( newValue ); }
		
	// Get the data from a given level.
	// Data is filled into a pixel buffer that must be big enough to contain it.
	void GetData( int level, TTextureCubeFace face, TTexelFormat cFormat, TTexelDataType type, CRawGlBuffer<BT_PixelPack> pixelBuffer ) const
		{ textureData.GetData( level, face, cFormat, type, pixelBuffer, TBT_CubeMap ); }

	// Set the image data from the designated container.
	// Internal format is a recommendation for a type of storage to use.
	// It is ignored if data is compressed.
	void SetImageData( const CImageData& data, TTextureGlFormat internalFormat = TGF_RGBA )
		{ CTypelessTextureOperations<TBT_CubeMap>::SetImageData( textureData, data, internalFormat ); }

	// Set the range of mipmap levels used by the texture.
	// The default values are both 0.
	void SetMipmapRange( int baseValue, int maxValue )
		{ textureData.SetTextureMipmapRange( TBT_CubeMap, baseValue, maxValue ); }

	// Reserve memory for the first level of the texture.
	void SetBuffer( CVector2<int> size )
		{ CTypelessTextureOperations<TBT_CubeMap>::SetBuffer( textureData, size, 0, format ); }
	// Increase buffer size without of the first level without destroying the texture contents.
	void GrowBuffer( CVector2<int> prevSize, CVector2<int> newSize, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_CubeMap>::GrowBuffer( textureData, prevSize, newSize, 0, format, cFormat, type ); }
	// Load the texture data directly.
	// data - the array to load.
	// size - size of the texture in texels.
	// level - the mipmap level to load.
	// glFormat - the OpenGL format of the sampler.
	// cFormat - the data format.
	// type - type used by data.
	void SetData( const BYTE* data, CVector2<int> size, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_CubeMap>::SetData( textureData, data, size, face, level, format, cFormat, type ); }
	// Load part of the data at a given offset.
	void SetSubData( CVector2<int> offset, const BYTE* data, CVector2<int> size, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type )
		{ CTypelessTextureOperations<TBT_CubeMap>::SetSubData( textureData, offset, data, size, face, level, cFormat, type ); }

protected:
	CEditTextureBase() : textureData( CTextureData::DefaultCubeMapSampler() ) {}
	explicit CEditTextureBase( unsigned id ) : textureData( id, CTextureData::DefaultCubeMapSampler() ) {}
	CEditTextureBase( unsigned id, CSamplerObject sampler ) : textureData( id, sampler ) {}
	
	void invalidateTextureData()
		{ textureData = CTextureData{ 0, textureData.GetSampler() }; }
	
private:
	CTextureData textureData;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

}	// namespace Gin.

