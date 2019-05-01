#pragma once
#include <DrawEnums.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Wrapper around the texture data.
class GINAPI CImageData {
public:
	// Constructor for compressed textures.
	CImageData( TTextureType type, int width, int height, int depth, TTextureCompressionType compressionType, int mipmapCount, int arrayCount );
	// Constructor for uncompressed textures.
	CImageData( TTextureType type, int width, int height, int depth, TTexelFormat _imageFormat, TTexelDataType _pixelFormat, int mipmapCount, int arrayCount );
	// Common constructor for compressed and uncompressed textures.
	CImageData( TTextureType type, int width, int height, int depth, TTexelFormat _imageFormat, TTexelDataType _pixelFormat, TTextureCompressionType compressionType, 
		int mipmapCount, int arrayCount );
	CImageData( CImageData&& other );

	TTextureType GetType() const
		{ return type; }
	TTextureCompressionType GetCompressionType() const
		{ return compressionType; }

	int Width() const
		{ return width; }
	int Height() const
		{ return height; }
	int Depth() const
		{ return depth; }

	// Pixel format details of the image.
	TTexelFormat GetTexelFormat() const
		{ return texelFormat; }
	TTexelDataType GetTexelDataType() const
		{ return texelDataType; }

	// Get data or size in bytes for the single image in a texture.
	int GetImageDataSize( int mipmapLevel ) const;
	const BYTE* GetImageData( int mipmapLevel, int arrayIndex = 0, int cubeFace = 0 ) const;
	// Set singe image's data. Data is assumed to have a top-left corner as its origin.
	// If shouldFlip is set to true, data is flipped vertically before copying.
	// Exactly GetImageDataSize( mipmapLevel ) bytes will be copied from data.
	void SetImageData( int mipmapLevel, int arrayIndex, int cubeFace, const BYTE* data, bool shouldFlip );
	// Set image data with a 32-bit BGRA pixel data.
	void SetImageData( int mipmapLevel, int arrayIndex, int cubeFace, const CColor* data, bool shouldFlip );

	// Get data for the whole mipmap level.
	// In simple textures the returned value is the same as in GetImageData.
	const CArray<BYTE>& GetMipmapData( int mimmapLevel ) const
		{ return textureData[mimmapLevel].Data; }
	CArray<BYTE>& GetMipmapData( int mipmapLevel ) 
		{ return textureData[mipmapLevel].Data; }

	// Get/Set maximum mipmap level.
	int GetMipmapCount() const
		{ return textureData.Size(); }
	int GetArrayCount() const
		{ return arrayCount; }
	int GetCubeFaceCount() const;

private:
	struct CSingleImage {
		// Data for all the images in the array/cube.
		CArray<BYTE> Data;
		// Size of a single image.
		int ImageSize;
		
		CSingleImage( CSingleImage&& other ) : Data( move( other.Data ) ), ImageSize( move( other.ImageSize ) ) {}
		CSingleImage() = default;

		// Copying is prohibited.
		CSingleImage( CSingleImage& ) = delete;
		void operator=( CSingleImage& ) = delete;
	};

	// Number of textures on each mipmap level.
	int arrayCount;
	// Texture data for all the mipmap levels.
	// Size of this array is the number of mipmap levels of the texture.
	CArray<CSingleImage> textureData;
	// Size of the texture.
	int width;
	int height;
	int depth;
	// Texture type and compression type.
	TTextureType type;
	TTextureCompressionType compressionType;

	TTexelFormat texelFormat;
	TTexelDataType texelDataType;

	// Byte block size for a DXT1 compression format.
	static const int dxtSmallBlockSize = 8;
	// Byte block size for DXT3 and DXT5 compression formats.
	static const int dxtBigBlockSize = 16;
	// Pixel block size for DXT compressionFormats.
	static const int dxtPixelBlockSize = 4;

	void initData( int mipmapCount );

	void fillSizes();

	int findUnpackedBytesPerPixel( int elemSize ) const;
	int findImageSize( int width, int height, int depth, int bytesPerPixel ) const;
	int findLineSize( int width, int bytesPerPixel ) const;

	static void adjustDimensions( int& w, int& h, int& d );

	void allocateData();

	void copyDataFlipped( BYTE* dest, const BYTE* src, int byteSize, int mipmapLevel ) const;
	void copyUncompressedDataFlipped( BYTE* dest, const BYTE* src, int byteSize, int mipmapLevel ) const;
	void getDimensions( int mipmapLevel, int& width, int& height, int& depth ) const;

	static void flipDxt1Block( BYTE* dest, const BYTE* src );
	static void flipDxt3Block( BYTE* dest, const BYTE* src );
	static void flipCompressedAlphaBlock( BYTE* dest, const BYTE* src );
	static void flipDxt5Block( BYTE* dest, const BYTE* src );
	
	template<class FlippingFunction>
	void copyDxtDataFlipped( BYTE* dest, const BYTE* src, int imageSize, int mipmapLevel, int blockSize, FlippingFunction flipBlocks ) const;

	// Copying is prohibited.
	CImageData( CImageData& ) = delete;
	void operator=( CImageData& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template<class FlippingFunction>
void CImageData::copyDxtDataFlipped( BYTE* dest, const BYTE* src, int imageSize, int mipmapLevel, int blockSize, FlippingFunction flipBlocks ) const
{
	int currentWidth, currentHeight, currentDepth;
	getDimensions( mipmapLevel, currentWidth, currentHeight, currentDepth );

	// Height in blocks.
	const int blockHeight = Ceil( currentHeight, dxtPixelBlockSize );
	// Size of a texture slice.
	const int sliceSize = imageSize / currentDepth;
	// Size of a block row in bytes.
	const int blockRowSize = sliceSize / blockHeight;
	// Size of a block row in blocks.
	const int blockRowCount = Ceil( currentWidth, dxtPixelBlockSize );

	int destPos = 0;
	int srcPos = sliceSize - blockRowSize;
	for( int i = 0; i < currentDepth; i++ ) {
		for( int j = 0; j < blockHeight; j++ ) {
			for( int k = 0; k < blockRowCount; k++ ) {
				flipBlocks( dest + destPos, src + srcPos );
				destPos += blockSize;
				srcPos += blockSize;
			}
			// Go to the next row.
			srcPos -= 2 * blockRowSize;
		}

		// Go to the next slice.
		srcPos += 2 * sliceSize;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

