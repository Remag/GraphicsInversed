#pragma once
#include <TextureData.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

template <TTextureBindingTarget target>
class CTypelessTextureOperations {};

//////////////////////////////////////////////////////////////////////////

template<>
class GINAPI CTypelessTextureOperations<TBT_Texture1> {
public:
	static void SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void SetData( CTextureData textureData, const void* data, int sizeX, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetBuffer( CTextureData textureData, int size, int level, TTextureGlFormat glFormat );
	static void SetSubData( CTextureData textureData, int offset, const void* data, int size, int level, TTexelFormat cFormat, TTexelDataType type );

private:
	static void setImageNormalData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void setImageCompressedData( const CImageData& data );
};

//////////////////////////////////////////////////////////////////////////

template<>
class GINAPI CTypelessTextureOperations<TBT_Texture2> {
public:
	static void SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void SetBuffer( CTextureData textureData, CVector2<int> size, int level, TTextureGlFormat glFormat );
	static void GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetData( CTextureData textureData, const void* data, CVector2<int> size, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetSubData( CTextureData textureData, CVector2<int> offset, const void* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type );

private:
	static void setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat );
	static void setImageCompressedData( const CImageData& data );
	static void setDataCheckErrors( CVector2<int> size, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type, const void* data );
	static void setSubDataCheckErrors( CVector2<int> size, int level, CVector2<int> offset, TTexelFormat cFormat, TTexelDataType type, const void* data );
};

//////////////////////////////////////////////////////////////////////////

template<>
class GINAPI CTypelessTextureOperations<TBT_TextureArray1> {
public:
	static void SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void SetSubImageData( CTextureData textureData, const CImageData& data, int index );
	static void SetBuffer( CTextureData textureData, CVector2<int> newSize, int level, TTextureGlFormat glFormat );
	static void GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetData( CTextureData textureData, const BYTE* data, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetSliceData( CTextureData textureData, int sliceIndex, int sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type );
	static void SetSubData( CTextureData textureData, CVector2<int> offset, const BYTE* data, CVector2<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type );

private:
	static void setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat );
	static void setImageCompressedData( const CImageData& data );
	static void setSubImageNormalData( const CImageData& data, int index );
	static void setSubImageCompressedData( const CImageData& data, int index );
	static void getDataCheckErrors( BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type );
	static void setDataCheckErrors( const BYTE* data, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void setSubDataCheckErrors( CVector2<int> offset, const BYTE* data, CVector2<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type );
};

//////////////////////////////////////////////////////////////////////////

template<>
class GINAPI CTypelessTextureOperations<TBT_TextureArray2> {
public:
	static void SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void SetSubImageData( CTextureData textureData, const CImageData& data, int index );
	static void SetBuffer( CTextureData textureData, CVector3<int> newSize, int level, TTextureGlFormat glFormat );
	static void GrowBuffer( CTextureData textureData, CVector3<int> prevSize, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetData( CTextureData textureData, const BYTE* data, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetSliceData( CTextureData textureData, int sliceIndex, CVector2<int> sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type );
	static void SetSubData( CTextureData textureData, CVector3<int> offset, const BYTE* data, CVector3<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type );

private:
	static void setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat );
	static void setImageCompressedData( const CImageData& data );
	static void setSubImageNormalData( const CImageData& data, int index );
	static void setSubImageCompressedData( const CImageData& data, int index );
	static void getDataCheckErrors( BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type );
	static void setDataCheckErrors( const BYTE* data, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void setSubDataCheckErrors( CVector3<int> offset, const BYTE* data, CVector3<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type );
};

//////////////////////////////////////////////////////////////////////////

template<>
class GINAPI CTypelessTextureOperations<TBT_CubeMap> {
public:
	static void SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat );
	static void SetBuffer( CTextureData textureData, CVector2<int> newSize, int level, TTextureGlFormat glFormat );
	static void GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetData( CTextureData textureData, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void SetSubData( CTextureData textureData, CVector2<int> offset, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type );

private:
	static void setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat );
	static void setImageCompressedData( const CImageData& data );
	static void setDataCheckErrors( const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type );
	static void setSubDataCheckErrors( CVector2<int> offset, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type );
	static TTextureCubeFace getCubeFace( int faceIndex );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

}	// namespace Gin.

