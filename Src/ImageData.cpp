#include <common.h>
#pragma hdrstop

#include <ImageData.h>
#include <TextureUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CImageData::CImageData( TTextureType _type, int _width, int _height, int _depth, TTextureCompressionType _compressionType, int _mipmapCount, int _arrayCount ) :
	CImageData( _type, _width, _height, _depth, TF_Compressed, TDT_Compressed, _compressionType, _mipmapCount, _arrayCount )
{
	assert( compressionType != TCT_Uncompressed );
}

CImageData::CImageData( TTextureType _type, int _width, int _height, int _depth, TTexelFormat _imageFormat, TTexelDataType _pixelFormat, int _mipmapCount, int _arrayCount ) :
	CImageData( _type, _width, _height, _depth, _imageFormat, _pixelFormat, TCT_Uncompressed, _mipmapCount, _arrayCount )
{
	assert( texelFormat != TF_Compressed && texelDataType != TDT_Compressed );
}

CImageData::CImageData( TTextureType _type, int _width, int _height, int _depth, TTexelFormat _imageFormat, TTexelDataType _pixelFormat,
		TTextureCompressionType _compressionType, int _mipmapCount, int _arrayCount ) :
	width( _width ),
	height( _height ),
	depth( _depth ),
	type( _type ),
	compressionType( _compressionType ),
	arrayCount( _arrayCount ),
	texelFormat( _imageFormat ),
	texelDataType( _pixelFormat )
{
	initData( _mipmapCount );
}

CImageData::CImageData( CImageData&& other ) :
	arrayCount( move( other.arrayCount ) ),
	textureData( move( other.textureData ) ),
	width( move( other.width ) ),
	height( move( other.height ) ),
	depth( move( other.depth ) ),
	type( move( other.type ) ),
	compressionType( move( other.compressionType ) ),
	texelFormat( move( other.texelFormat ) ),
	texelDataType( move( other.texelDataType ) )
{

}

void CImageData::initData( int mipmapCount )
{
	assert( width > 0 && height > 0 && depth > 0 );
	assert( arrayCount > 0 );

	textureData.IncreaseSize( mipmapCount );
	fillSizes();
	allocateData();
}

int CImageData::GetCubeFaceCount() const
{
	return GetType() == TT_TextureCubeMap ? 6 : 1;
}

void CImageData::fillSizes()
{
	int mipmapWidth = width;
	int mipmapHeight = height;
	int mipmapDepth = depth;
	
	const int bytesPerPixel = ( compressionType == TCT_Uncompressed )
		? TextureUtils::FindBytesPerPixel( texelDataType, texelFormat )
		: NotFound;
	for( int i = 0; i < GetMipmapCount(); i++ ) {
		textureData[i].ImageSize = findImageSize( mipmapWidth, mipmapHeight, mipmapDepth, bytesPerPixel );
		adjustDimensions( mipmapWidth, mipmapHeight, mipmapDepth );
	}
}

// Calculate the size of an image based on the used compression.
int CImageData::findImageSize( int imageWidth, int imageHeight, int imageDepth, int bytesPerPixel ) const
{
	// BytesPerPixel can and must be calculated only for uncompressed textures.
	assert( compressionType != TCT_Uncompressed || bytesPerPixel > 0 );

	const int lineSize = findLineSize( imageWidth, bytesPerPixel );
	// Height is calculated differently for compressed and uncompressed types.
	const int effectiveHeight = compressionType == TCT_Uncompressed ? imageHeight : Ceil( imageHeight, dxtPixelBlockSize );
	// Block compression is assumed to compressed every slice of 3D textures separately.
	// Depth is always the same.
	return lineSize * effectiveHeight * imageDepth;
}

// Get the size of a texture line in bytes.
// In uncompressed textures the line consists of pixels.
// In compressed textures the line consists of blocks of varying size.
int CImageData::findLineSize( int lineWidth, int bytesPerPixel ) const
{
	switch( compressionType ) {
	case TCT_Uncompressed:
		return lineWidth * bytesPerPixel;
	case TCT_Dxt1_RGB:
		return Ceil( lineWidth, dxtPixelBlockSize ) * dxtSmallBlockSize;
	case TCT_Dxt3:
	case TCT_Dxt5:
		return Ceil( lineWidth, dxtPixelBlockSize ) * dxtBigBlockSize;
	default:
		assert( false );
		return NotFound;
	}
}

void CImageData::adjustDimensions( int& w, int& h, int& d )
{
	if( w > 1 ) {
		w /= 2;
	}
	if( h > 1 ) {
		h /= 2;
	}
	if( d > 1 ) {
		d /= 2;
	}
}

void CImageData::allocateData()
{
	const int cubeFaceCount = GetCubeFaceCount();
	for( int i = 0; i < GetMipmapCount(); i++ ) {
		textureData[i].Data.IncreaseSize( textureData[i].ImageSize * cubeFaceCount * arrayCount );
	}
}

int CImageData::GetImageDataSize( int mipmapLevel ) const
{
	assert( mipmapLevel >= 0 && mipmapLevel < GetMipmapCount() );

	return textureData[mipmapLevel].ImageSize;
}

const BYTE* CImageData::GetImageData( int mipmapLevel, int arrayIndex /*= 0*/, int cubeFace /*= 0 */ ) const
{
	assert( mipmapLevel >= 0 && mipmapLevel < GetMipmapCount() );
	assert( arrayIndex >= 0 && arrayIndex < GetArrayCount() );
	assert( cubeFace >= 0 && cubeFace < GetCubeFaceCount() );

	const int dataOffset = ( arrayIndex * GetCubeFaceCount() + cubeFace ) * textureData[mipmapLevel].ImageSize;
	return textureData[mipmapLevel].Data.Ptr() + dataOffset;
}

void CImageData::SetImageData( int mipmapLevel, int arrayIndex, int cubeFace, const BYTE* data, bool shouldFlip )
{
	assert( mipmapLevel >= 0 && mipmapLevel < GetMipmapCount() );
	assert( arrayIndex >= 0 && arrayIndex < GetArrayCount() );
	assert( cubeFace >= 0 && cubeFace < GetCubeFaceCount() );

	const int dataOffset = ( arrayIndex * GetCubeFaceCount() + cubeFace ) * textureData[mipmapLevel].ImageSize;
	if( shouldFlip ) {
		copyDataFlipped( textureData[mipmapLevel].Data.Ptr() + dataOffset, data, textureData[mipmapLevel].ImageSize, mipmapLevel );
	} else {
		memcpy( textureData[mipmapLevel].Data.Ptr() + dataOffset, data, textureData[mipmapLevel].ImageSize );
	}
}

void CImageData::SetImageData( int mipmapLevel, int arrayIndex, int cubeFace, const CColor* data, bool shouldFlip )
{
	SetImageData( mipmapLevel, arrayIndex, cubeFace, reinterpret_cast<const BYTE*>( data ), shouldFlip );
}

extern const CError Err_InvalidDxtImageHeight;
void CImageData::copyDataFlipped( BYTE* dest, const BYTE* src, int byteSize, int mipmapLevel ) const
{
	switch( compressionType ) {
	case TCT_Uncompressed:
		copyUncompressedDataFlipped( dest, src, byteSize, mipmapLevel );
		return;
	case TCT_Dxt1_RGB:
	case TCT_Dxt1_sRGB:
		copyDxtDataFlipped( dest, src, byteSize, mipmapLevel, dxtSmallBlockSize, flipDxt1Block );
		break;
	case TCT_Dxt3:
	case TCT_Dxt3_sRGBA:
		copyDxtDataFlipped( dest, src, byteSize, mipmapLevel, dxtBigBlockSize, flipDxt3Block );
		break;
	case TCT_Dxt5:
	case TCT_Dxt5_sRGBA:
		copyDxtDataFlipped( dest, src, byteSize, mipmapLevel, dxtBigBlockSize, flipDxt5Block );
		break;
	default:
		assert( false );
	}
	check( height % dxtPixelBlockSize == 0, Err_InvalidDxtImageHeight );
}

void CImageData::copyUncompressedDataFlipped( BYTE* dest, const BYTE* src, int imageSize, int mipmapLevel ) const
{
	int currentWidth, currentHeight, currentDepth;
	getDimensions( mipmapLevel, currentWidth, currentHeight, currentDepth );
	const int sliceSize = imageSize / currentDepth;
	const int lineSize = sliceSize / currentHeight;
	int destPos = 0;
	int srcPos = sliceSize - lineSize;
	// Go through the image slice by slice.
	for( int i = 0; i < currentDepth; i++ ) {
		// Flip each slice vertically line by line.
		for( int j = 0; j < currentHeight; j++ ) {
			// Copy the line.
			memcpy( dest + destPos, src + srcPos, lineSize );
			srcPos -= lineSize;
			destPos += lineSize;
		}
		// Move to the next slice.
		srcPos += 2 * sliceSize - lineSize;
	}
}

// Get the dimensions of the image on the current mipmap level.
void CImageData::getDimensions( int mipmapLevel, int& mipmapWidth, int& mipmapHeight, int& mipmapDepth ) const
{
	// Bit shifts by more than this amount are undefined.
	assert( mipmapLevel < sizeof( int ) * CHAR_BIT );

	mipmapWidth = width >> mipmapLevel;
	if( mipmapWidth < 1 ) {
		mipmapWidth = 1;
	}
	mipmapHeight = height >> mipmapLevel;
	if( mipmapHeight < 1 ) {
		mipmapHeight = 1;
	}
	mipmapDepth = depth >> mipmapLevel;
	if( mipmapDepth < 1 ) {
		mipmapDepth = 1;
	}
}

void CImageData::flipDxt1Block( BYTE* dest, const BYTE* src )
{
	// First four bytes contain color information.
	memcpy( dest, src, 4 );

	// Next four bytes contain 4x4 pixel block information, flip them.
	// Each pixel is represented as 2 bits, copy every byte in reverse order.
	dest[4] = src[7];
	dest[5] = src[6];
	dest[6] = src[5];
	dest[7] = src[4];
}

void CImageData::flipDxt3Block( BYTE* dest, const BYTE* src )
{
	// First eight bytes contain 4x4 pixel block data, flip them.
	// Each pixel is represented as 4 bits, copy every two bytes in reverse order.
	dest[0] = src[6];
	dest[1] = src[7];
	dest[2] = src[4];
	dest[3] = src[5];
	dest[4] = src[2];
	dest[5] = src[3];
	dest[6] = src[0];
	dest[7] = src[1];
	
	// Next eight bytes are DXT1 compressed.
	flipDxt1Block( dest + 8, src + 8 );
}

void CImageData::flipCompressedAlphaBlock( BYTE* dest, const BYTE* src )
{
	// First two bytes contain alpha values.
	memcpy( dest, src, 2 );

	// Next six bytes contain 4x4 pixel block data, flip them.
	// See compression description for the logic behind the flipping:
	// http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
	dest[2] = ( src[7] & 0x0F ) << 4;
	dest[2] |= ( src[6] & 0xF0 ) >> 4;

	dest[3] = ( src[5] & 0x0F ) << 4;
	dest[3] |= ( src[7] & 0xF0 ) >> 4;

	dest[4] = ( src[6] & 0x0F ) << 4;
	dest[4] |= ( src[5] & 0xF0 ) >> 4;

	dest[5] = ( src[4] & 0x0F ) << 4;
	dest[5] |= ( src[3] & 0xF0 ) >> 4;

	dest[6] = ( src[2] & 0x0F ) << 4;
	dest[6] |= ( src[4] & 0xF0 ) >> 4;

	dest[7] = ( src[3] & 0x0F ) << 4;
	dest[7] |= ( src[2] & 0xF0 ) >> 4;
}

void CImageData::flipDxt5Block( BYTE* dest, const BYTE* src )
{
	// First eight bytes are alpha compressed.
	flipCompressedAlphaBlock( dest, src );
	// Next eight bytes are DXT1 compressed.
	flipDxt1Block( dest + 8, src + 8 );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
