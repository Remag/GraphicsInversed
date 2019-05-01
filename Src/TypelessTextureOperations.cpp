#include <common.h>
#pragma hdrstop

#include <TypelessTextureOperations.h>
#include <ImageData.h>
#include <GinError.h>
#include <TextureUtils.h>
#include <GlBuffer.h>
#include <TextureBinder.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

void CTypelessTextureOperations<TBT_Texture1>::SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	assert( data.GetType() == TT_Texture1D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );

	const int mipmapCount = data.GetMipmapCount();
	textureData.SetTextureMipmapRange( TBT_Texture1, 0, mipmapCount - 1 );

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setImageNormalData( textureData, data, internalFormat );
	} else {
		setImageCompressedData( data );
	}
}

// Set the texture uncompressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_Texture1>::setImageNormalData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	const int mipmapCount = data.GetMipmapCount();
	int width = data.Width();
	assert( width > 0 );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		SetData( textureData, data.GetMipmapData( i ).Ptr(), width, i, internalFormat, format, dataType );
		// Next mipmap level is twice as small ( rounded down ).
		if( width > 1 ) {
			width /= 2;
		}
	}
}

// Set the texture compressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_Texture1>::setImageCompressedData( const CImageData& data )
{
	const int mipmapCount = data.GetMipmapCount();
	int width = data.Width();
	assert( width > 0 );
	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexImage1D( TBT_Texture1, i, data.GetCompressionType(), width, 0, 
			data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( width > 1 ) {
			width /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_Texture1>::SetData( CTextureData textureData, const void* data, int size, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( size >= 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	gl::TexImage1D( gl::TEXTURE_1D, level, glFormat, size, 0, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_Texture1>::SetBuffer( CTextureData textureData, int size, int level, TTextureGlFormat glFormat )
{
	SetData( textureData, nullptr, size, level, glFormat, TF_Red, TDT_UnsignedByte );
}

void CTypelessTextureOperations<TBT_Texture1>::SetSubData( CTextureData textureData, int offset, const void* data, int size, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( size >= 0 );
	assert( offset >= 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;
	gl::TexSubImage1D( TBT_Texture1, level, offset, size, cFormat, type, data );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

void CTypelessTextureOperations<TBT_Texture2>::SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	assert( data.GetType() == TT_Texture2D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	
	const int mipmapCount = data.GetMipmapCount();
	assert( mipmapCount > 0 );
	textureData.SetTextureMipmapRange( TBT_Texture2, 0, mipmapCount - 1 );

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setImageNormalData( data, internalFormat );
	} else {
		setImageCompressedData( data );
	}
}

// Set the texture uncompressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_Texture2>::setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat )
{
	const int mipmapCount = data.GetMipmapCount();
	CVector2<int> mipmapSize{ data.Width(), data.Height() };
	assert( mipmapSize.X() > 0 && mipmapSize.Y() > 0 );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		setDataCheckErrors( mipmapSize, i, internalFormat, format, dataType, data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
		if( mipmapSize.Y() > 1 ) {
			mipmapSize.Y() /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_Texture2>::setDataCheckErrors( CVector2<int> size, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type, const void* data )
{
	gl::TexImage2D( TBT_Texture2, level, glFormat, size.X(), size.Y(), 0, cFormat, type, data );
	CheckGlError();
}

// Set the texture compressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_Texture2>::setImageCompressedData( const CImageData& data )
{
	const int mipmapCount = data.GetMipmapCount();
	int mipmapWidth = data.Width();
	int mipmapHeight = data.Height();
	assert( mipmapWidth > 0 && mipmapHeight > 0 );
	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexImage2D( TBT_Texture2, i, data.GetCompressionType(), mipmapWidth, mipmapHeight, 0, 
			data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
		if( mipmapHeight > 1 ) {
			mipmapHeight /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_Texture2>::SetBuffer( CTextureData textureData, CVector2<int> size, int level, TTextureGlFormat glFormat )
{
	// Initialize compatible parameters for format types.
	// Even though these parameters are not used, they still need to correspond to glFormat to not produce errors.
	TTexelFormat texelFormat;
	TTexelDataType texelDataType;
	if( glFormat == TGF_Depth ) {
		texelFormat = TF_Depth;
		texelDataType = TDT_Float;
	} else {
		texelFormat = TF_Red;
		texelDataType = TDT_UnsignedByte;
	}

	SetData( textureData, nullptr, size, level, glFormat, texelFormat, texelDataType );
}

void CTypelessTextureOperations<TBT_Texture2>::GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );

	if( prevSize.IsNull() ) {
		SetBuffer( textureData, newSize, level, glFormat );
		return;
	}

	const int newBufferSizeX = max( prevSize.X(), newSize.X() );
	const int newBufferSizeY = max( prevSize.Y(), newSize.Y() );

	const int bpp = TextureUtils::FindBytesPerPixel( type, cFormat );
	const int rowSize = prevSize.X() * bpp;

	CGlBufferOwner<BT_PixelPack, BYTE> pixelBuffer;
	pixelBuffer.ReserveBuffer( rowSize * prevSize.Y(), BUH_DynamicCopy );

	// Copy an existing image to the pixel buffer.
	CBufferObjectBinder binder( pixelBuffer );
	gl::GetTexImage( TBT_Texture2, level, cFormat, type, 0 );
	// Reserve new memory for the texture.
	gl::TexImage2D( TBT_Texture2, level, glFormat, newBufferSizeX, newBufferSizeY, 0, cFormat, type, 0 );
	// Copy the image memory back to the texture.
	CBufferObjectBinder binder2( BT_PixelUnpack, pixelBuffer.GetId() );
	gl::TexSubImage2D( TBT_Texture2, level, 0, 0, prevSize.X(), prevSize.Y(), cFormat, type, 0 );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_Texture2>::SetData( CTextureData textureData, const void* data, CVector2<int> size, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( size.Y() >= 0 && size.X() >= 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	setDataCheckErrors( size, level, glFormat, cFormat, type, data );
}

void CTypelessTextureOperations<TBT_Texture2>::SetSubData( CTextureData textureData, CVector2<int> offset, const void* data, CVector2<int> size, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( offset.X() >= 0 && offset.Y() >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;
	setSubDataCheckErrors( size, level, offset, cFormat, type, data );
}

void CTypelessTextureOperations<TBT_Texture2>::setSubDataCheckErrors( CVector2<int> size, int level, CVector2<int> offset, TTexelFormat cFormat, TTexelDataType type, const void* data )
{
	gl::TexSubImage2D( TBT_Texture2, level, offset.X(), offset.Y(), size.X(), size.Y(), cFormat, type, data );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

void CTypelessTextureOperations<TBT_TextureArray1>::SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	assert( data.GetType() == TT_Texture2D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	
	const int mipmapCount = data.GetMipmapCount();
	assert( mipmapCount > 0 );
	textureData.SetTextureMipmapRange( TBT_TextureArray1, 0, mipmapCount - 1 );

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setImageNormalData( data, internalFormat );
	} else {
		setImageCompressedData( data );
	}
}

// Set the texture uncompressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_TextureArray1>::setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat )
{
	const int mipmapCount = data.GetMipmapCount();
	CVector2<int> mipmapSize( data.Width(), data.GetArrayCount() );

	assert( mipmapSize.X() > 0 && mipmapSize.Y() > 0 );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		setDataCheckErrors( data.GetMipmapData( i ).Ptr(), mipmapSize, i, internalFormat, format, dataType );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
	}
}

// Set the texture compressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_TextureArray1>::setImageCompressedData( const CImageData& data )
{
	const int mipmapCount = data.GetMipmapCount();
	const int arrayCount = data.GetArrayCount();
	int mipmapWidth = data.Width();
	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexImage2D( TBT_TextureArray1, i, data.GetCompressionType(), mipmapWidth, arrayCount, 0, 
			data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray1>::SetSubImageData( CTextureData textureData, const CImageData& data, int index )
{
	assert( data.GetType() == TT_Texture1D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setSubImageNormalData( data, index );
	} else {
		setSubImageCompressedData( data, index );
	}
}

void CTypelessTextureOperations<TBT_TextureArray1>::setSubImageNormalData( const CImageData& data, int index )
{
	const int mipmapCount = data.GetMipmapCount();
	CVector2<int> mipmapSize( data.Width(), data.GetArrayCount() );

	const CVector2<int> dataOffset( 0, index );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		setSubDataCheckErrors( dataOffset, data.GetMipmapData( i ).Ptr(), mipmapSize, i, format, dataType );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray1>::setSubImageCompressedData( const CImageData& data, int index )
{
	const int mipmapCount = data.GetMipmapCount();
	int mipmapWidth = data.Width();

	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexSubImage2D( TBT_TextureArray1, i, 0, index, mipmapWidth, 1, data.GetCompressionType(), data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray1>::SetBuffer( CTextureData textureData, CVector2<int> newSize, int level, TTextureGlFormat glFormat )
{
	SetData( textureData, nullptr, newSize, level, glFormat, TF_Red, TDT_UnsignedByte );
}

void CTypelessTextureOperations<TBT_TextureArray1>::GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, 
	TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );

	if( prevSize.IsNull() ) {
		SetBuffer( textureData, newSize, level, glFormat );
		return;
	}

	const int newBufferSizeX = max( prevSize.X(), newSize.X() );
	const int newBufferSizeY = max( prevSize.Y(), newSize.Y() );
	const CVector2<int> newBufferSize( newBufferSizeX, newBufferSizeY );

	const int bpp = TextureUtils::FindBytesPerPixel( type, cFormat );
	const int rowSize = prevSize.X() * bpp;

	CGlBufferOwner<BT_PixelPack, BYTE> pixelBuffer;
	pixelBuffer.ReserveBuffer( rowSize * prevSize.Y(), BUH_DynamicCopy );

	// Copy an existing image to the pixel buffer.
	CBufferObjectBinder binder( pixelBuffer );
	gl::GetTexImage( TBT_TextureArray1, level, cFormat, type, 0 );
	// Reserve new memory for the texture.
	gl::TexImage2D( TBT_TextureArray1, level, glFormat, newBufferSizeX, newBufferSizeY, 0, cFormat, type, 0 );
	// Copy the image memory back to the texture.
	CBufferObjectBinder binder2( BT_PixelUnpack, pixelBuffer.GetId() );
	gl::TexSubImage2D( TBT_TextureArray1, level, 0, 0, prevSize.X(), prevSize.Y(), cFormat, type, 0 );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray1>::getDataCheckErrors( BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
{
	gl::GetTexImage( TBT_TextureArray1, level, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray1>::SetData( CTextureData textureData, const BYTE* data, CVector2<int> newSize, int level, TTextureGlFormat glFormat,
	TTexelFormat cFormat, TTexelDataType type )
{
	assert( newSize.X() > 0 && newSize.Y() > 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	setDataCheckErrors( data, newSize, level, glFormat, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray1>::setDataCheckErrors( const BYTE* data, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexImage2D( TBT_TextureArray1, level, glFormat, newSize.X(), newSize.Y(), 0, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray1>::SetSliceData( CTextureData textureData, int sliceIndex, int sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	const CVector2<int> sliceOffset{ 0, sliceIndex };
	const CVector2<int> sliceSize2D{ sliceSize, 1 };
	setSubDataCheckErrors( sliceOffset, data, sliceSize2D, level, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray1>::SetSubData( CTextureData textureData, CVector2<int> offset, const BYTE* data, CVector2<int> newSize, int level,
	TTexelFormat cFormat, TTexelDataType type )
{
	assert( offset.X() >= 0 && offset.Y() >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;
	setSubDataCheckErrors( offset, data, newSize, level, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray1>::setSubDataCheckErrors( CVector2<int> offset, const BYTE* data, CVector2<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexSubImage2D( TBT_TextureArray1, level, offset.X(), offset.Y(), newSize.X(), newSize.Y(), cFormat, type, data );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

void CTypelessTextureOperations<TBT_TextureArray2>::SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	assert( data.GetType() == TT_Texture2D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	
	const int mipmapCount = data.GetMipmapCount();
	assert( mipmapCount > 0 );
	textureData.SetTextureMipmapRange( TBT_TextureArray2, 0, mipmapCount - 1 );

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setImageNormalData( data, internalFormat );
	} else {
		setImageCompressedData( data );
	}
}

// Set the texture uncompressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_TextureArray2>::setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat )
{
	const int mipmapCount = data.GetMipmapCount();
	CVector3<int> mipmapSize( data.Width(), data.Height(), data.GetArrayCount() );

	assert( mipmapSize.X() > 0 && mipmapSize.Y() > 0 && mipmapSize.Z() > 0 );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		setDataCheckErrors( data.GetMipmapData( i ).Ptr(), mipmapSize, i, internalFormat, format, dataType );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
		if( mipmapSize.Y() > 1 ) {
			mipmapSize.Y() /= 2;
		}
	}
}

// Set the texture compressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_TextureArray2>::setImageCompressedData( const CImageData& data )
{
	const int mipmapCount = data.GetMipmapCount();
	const int arrayCount = data.GetArrayCount();
	int mipmapWidth = data.Width();
	int mipmapHeight = data.Height();
	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexImage3D( TBT_TextureArray2, i, data.GetCompressionType(), mipmapWidth, mipmapHeight, arrayCount, 0, 
			data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
		if( mipmapHeight > 1 ) {
			mipmapHeight /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray2>::SetSubImageData( CTextureData textureData, const CImageData& data, int index )
{
	assert( data.GetType() == TT_Texture2D );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	assert( data.GetArrayCount() == 1 );
	textureData;

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setSubImageNormalData( data, index );
	} else {
		setSubImageCompressedData( data, index );
	}
}

void CTypelessTextureOperations<TBT_TextureArray2>::setSubImageNormalData( const CImageData& data, int index )
{
	const int mipmapCount = data.GetMipmapCount();
	CVector3<int> mipmapSize( data.Width(), data.Height(), data.GetArrayCount() );

	const CVector3<int> dataOffset( 0, 0, index );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		setSubDataCheckErrors( dataOffset, data.GetMipmapData( i ).Ptr(), mipmapSize, i, format, dataType );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
		if( mipmapSize.Y() > 1 ) {
			mipmapSize.Y() /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray2>::setSubImageCompressedData( const CImageData& data, int index )
{
	const int mipmapCount = data.GetMipmapCount();
	const CVector3<int> dataOffset( 0, 0, index );
	int mipmapWidth = data.Width();
	int mipmapHeight = data.Height();

	for( int i = 0; i < mipmapCount; i++ ) {
		gl::CompressedTexSubImage3D( TBT_TextureArray2, i, 0, 0, index, mipmapWidth, mipmapHeight, 1, data.GetCompressionType(), data.GetMipmapData( i ).Size(), data.GetMipmapData( i ).Ptr() );
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
		if( mipmapHeight > 1 ) {
			mipmapHeight /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_TextureArray2>::SetBuffer( CTextureData textureData, CVector3<int> newSize, int level, TTextureGlFormat glFormat )
{
	SetData( textureData, nullptr, newSize, level, glFormat, TF_Red, TDT_UnsignedByte );
}

void CTypelessTextureOperations<TBT_TextureArray2>::GrowBuffer( CTextureData textureData, CVector3<int> prevSize, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );

	if( prevSize.IsNull() ) {
		SetBuffer( textureData, newSize, level, glFormat );
		return;
	}

	const int newBufferSizeX = max( prevSize.X(), newSize.X() );
	const int newBufferSizeY = max( prevSize.Y(), newSize.Y() );
	const int newBufferSizeZ = max( prevSize.Z(), newSize.Z() );
	const CVector3<int> newBufferSize( newBufferSizeX, newBufferSizeY, newBufferSizeZ );

	const int bpp = TextureUtils::FindBytesPerPixel( type, cFormat );
	const int rowSize = prevSize.X() * bpp;

	CGlBufferOwner<BT_PixelPack, BYTE> pixelBuffer;
	pixelBuffer.ReserveBuffer( rowSize * prevSize.Y() * prevSize.Z(), BUH_DynamicCopy );

	// Copy an existing image to the pixel buffer.
	CBufferObjectBinder binder( pixelBuffer );
	gl::GetTexImage( TBT_TextureArray2, level, cFormat, type, 0 );
	// Reserve new memory for the texture.
	gl::TexImage3D( TBT_TextureArray2, level, glFormat, newBufferSizeX, newBufferSizeY, newBufferSizeZ, 0, cFormat, type, 0 );
	// Copy the image memory back to the texture.
	CBufferObjectBinder binder2( BT_PixelUnpack, pixelBuffer.GetId() );
	gl::TexSubImage3D( TBT_TextureArray2, level, 0, 0, 0, prevSize.X(), prevSize.Y(), prevSize.Z(), cFormat, type, 0 );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray2>::getDataCheckErrors( BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
{
	gl::GetTexImage( TBT_TextureArray2, level, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray2>::SetData( CTextureData textureData, const BYTE* data, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( newSize.X() > 0 && newSize.Y() > 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	setDataCheckErrors( data, newSize, level, glFormat, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray2>::setDataCheckErrors( const BYTE* data, CVector3<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexImage3D( TBT_TextureArray2, level, glFormat, newSize.X(), newSize.Y(), newSize.Z(), 0, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_TextureArray2>::SetSliceData( CTextureData textureData, int sliceIndex, CVector2<int> sliceSize, const BYTE* data, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	const CVector3<int> sliceOffset{ 0, 0, sliceIndex };
	const CVector3<int> sliceSize3D{ sliceSize, 1 };
	setSubDataCheckErrors( sliceOffset, data, sliceSize3D, level, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray2>::SetSubData( CTextureData textureData, CVector3<int> offset, const BYTE* data, CVector3<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( offset.X() >= 0 && offset.Y() >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;
	setSubDataCheckErrors( offset, data, newSize, level, cFormat, type );
}

void CTypelessTextureOperations<TBT_TextureArray2>::setSubDataCheckErrors( CVector3<int> offset, const BYTE* data, CVector3<int> newSize, int level, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexSubImage3D( TBT_TextureArray2, level, offset.X(), offset.Y(), offset.Z(), newSize.X(), newSize.Y(), newSize.Z(), cFormat, type, data );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

void CTypelessTextureOperations<TBT_CubeMap>::SetImageData( CTextureData textureData, const CImageData& data, TTextureGlFormat internalFormat )
{
	assert( data.GetType() == TT_TextureCubeMap );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	
	const int mipmapCount = data.GetMipmapCount();
	assert( mipmapCount > 0 );
	textureData.SetTextureMipmapRange( TBT_CubeMap, 0, mipmapCount - 1 );

	if( data.GetCompressionType() == TCT_Uncompressed ) {
		setImageNormalData( data, internalFormat );
	} else {
		setImageCompressedData( data );
	}
}

// Set the texture uncompressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_CubeMap>::setImageNormalData( const CImageData& data, TTextureGlFormat internalFormat )
{
	const int mipmapCount = data.GetMipmapCount();
	const int faceCount = data.GetCubeFaceCount();
	CVector2<int> mipmapSize( data.Width(), data.Height() );

	assert( mipmapSize.X() > 0 && mipmapSize.Y() > 0 );
	const TTexelFormat format = data.GetTexelFormat();
	const TTexelDataType dataType = data.GetTexelDataType();

	for( int i = 0; i < mipmapCount; i++ ) {
		for( int facePos = 0; facePos < faceCount; facePos++ ) {
			setDataCheckErrors( data.GetImageData( i, 0, facePos ), mipmapSize, getCubeFace( facePos ), i, internalFormat, format, dataType );
		}
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapSize.X() > 1 ) {
			mipmapSize.X() /= 2;
		}
		if( mipmapSize.Y() > 1 ) {
			mipmapSize.Y() /= 2;
		}
	}
}

TTextureCubeFace CTypelessTextureOperations<TBT_CubeMap>::getCubeFace( int faceIndex )
{
	static CStackArray<TTextureCubeFace, 6> faces {
		TCF_NegativeX,
		TCF_PositiveX,
		TCF_NegativeY,
		TCF_PositiveY,
		TCF_NegativeZ,
		TCF_PositiveZ 
	};
	return faces[faceIndex];
}

// Set the texture compressed data for all the mipmap levels.
void CTypelessTextureOperations<TBT_CubeMap>::setImageCompressedData( const CImageData& data )
{
	const int mipmapCount = data.GetMipmapCount();
	const int faceCount = data.GetCubeFaceCount();
	int mipmapWidth = data.Width();
	int mipmapHeight = data.Height();
	for( int i = 0; i < mipmapCount; i++ ) {
		for( int facePos = 0; facePos < faceCount; facePos++ ) {
			gl::CompressedTexImage2D( getCubeFace( facePos ), i, data.GetCompressionType(), mipmapWidth, mipmapHeight, 0, 
				data.GetImageDataSize( i ), data.GetImageData( i, 0, facePos ) );
		}
		
		// Next mipmap level is twice as small ( rounded down ).
		if( mipmapWidth > 1 ) {
			mipmapWidth /= 2;
		}
		if( mipmapHeight > 1 ) {
			mipmapHeight /= 2;
		}
	}
}

void CTypelessTextureOperations<TBT_CubeMap>::SetBuffer( CTextureData textureData, CVector2<int> newSize, int level, TTextureGlFormat glFormat )
{
	for( int facePos = 0; facePos < 6; facePos++ ) {
		SetData( textureData, nullptr, newSize, getCubeFace( facePos ), level, glFormat, TF_Red, TDT_UnsignedByte );
	}
}

void CTypelessTextureOperations<TBT_CubeMap>::GrowBuffer( CTextureData textureData, CVector2<int> prevSize, CVector2<int> newSize, int level, TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );

	if( prevSize.IsNull() ) {
		SetBuffer( textureData, newSize, level, glFormat );
		return;
	}

	const int newBufferSizeX = max( prevSize.X(), newSize.X() );
	const int newBufferSizeY = max( prevSize.Y(), newSize.Y() );
	const CVector2<int> newBufferSize( newBufferSizeX, newBufferSizeY );

	const int bpp = TextureUtils::FindBytesPerPixel( type, cFormat );
	const int rowSize = prevSize.X() * bpp;

	CGlBufferOwner<BT_PixelPack, BYTE> pixelBuffer;
	pixelBuffer.ReserveBuffer( rowSize * prevSize.Y(), BUH_DynamicCopy );

	// Copy an existing image to the pixel buffer.
	CBufferObjectBinder binder( pixelBuffer );
	CBufferObjectBinder binder2( BT_PixelUnpack, pixelBuffer.GetId() );

	for( int facePos = 0; facePos < 6; facePos++ ) {
		gl::GetTexImage( getCubeFace( facePos ), level, cFormat, type, 0 );
		// Reserve new memory for the texture.
		gl::TexImage2D( getCubeFace( facePos ), level, glFormat, newBufferSizeX, newBufferSizeY, 0, cFormat, type, 0 );
		// Copy the image memory back to the texture.
		gl::TexSubImage2D( getCubeFace( facePos ), level, 0, 0, prevSize.X(), prevSize.Y(), cFormat, type, 0 );
		CheckGlError();
	}
}

void CTypelessTextureOperations<TBT_CubeMap>::SetData( CTextureData textureData, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTextureGlFormat glFormat, 
	TTexelFormat cFormat, TTexelDataType type )
{
	assert( newSize.X() > 0 && newSize.Y() > 0 );
	assert( level >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;

	setDataCheckErrors( data, newSize, face, level, glFormat, cFormat, type );
}

void CTypelessTextureOperations<TBT_CubeMap>::setDataCheckErrors( const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level,
	TTextureGlFormat glFormat, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexImage2D( face, level, glFormat, newSize.X(), newSize.Y(), 0, cFormat, type, data );
	CheckGlError();
}

void CTypelessTextureOperations<TBT_CubeMap>::SetSubData( CTextureData textureData, CVector2<int> offset, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type )
{
	assert( offset.X() >= 0 && offset.Y() >= 0 );
	assert( CTextureBinder::GetCurrentBindingId() == textureData.GetTextureId() );
	textureData;
	setSubDataCheckErrors( offset, data, newSize, face, level, cFormat, type );
}

void CTypelessTextureOperations<TBT_CubeMap>::setSubDataCheckErrors( CVector2<int> offset, const BYTE* data, CVector2<int> newSize, TTextureCubeFace face, int level, TTexelFormat cFormat, TTexelDataType type )
{
	gl::TexSubImage2D( face, level, offset.X(), offset.Y(), newSize.X(), newSize.Y(), cFormat, type, data );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

}	// namespace Gin.
