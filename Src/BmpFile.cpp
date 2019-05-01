#include <common.h>
#pragma hdrstop

#include <BmpFile.h>
#include <Gindefs.h>

namespace Gin {

struct CBitmapInfoHeader {
	// The size of this header (40 bytes).
	int HeaderSize;
	// The bitmap width in pixels (signed integer).
	int PixelWidth;
	// The bitmap height in pixels (signed integer).
	int PixelHeight;
	// The number of color planes, must be 1.
	short ColorPlanesCount;
	// The number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
	short Bpp;
	// The compression method being used. See the next table for a list of possible values.
	int CompressionMethod;
	// The image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
	int ImageSize;
	// The horizontal resolution of the image. ( pixel per meter, signed integer ).
	int ResolutionX;
	// The vertical resolution of the image. ( pixel per meter, signed integer ).
	int ResolutionY;
	// The number of colors in the color palette, or 0 to default to 2^n.
	int ColorCount;
	// The number of important colors used, or 0 when every color is important; generally ignored.
	int ImportantColorCount;
};

struct CBitmapV4Header {
	CBitmapInfoHeader BitmapInfo;
	unsigned RedMask;
	unsigned GreenMask;
	unsigned BlueMask;
	unsigned AlphaMask;
	unsigned CSType;
	CIEXYZTRIPLE bV4Endpoints;
	unsigned RedGamma;
	unsigned GreenGamma;
	unsigned BlueGamma;
};

//////////////////////////////////////////////////////////////////////////

CBmpFile::CBmpFile( CUnicodeView fileName )
{
	Open( fileName );
}

void CBmpFile::Open( CUnicodeView fileName )
{
	assert( !IsOpen() );
	fileData.Open( fileName, FRWM_Write, FCM_CreateAlways, FSM_DenyNone );
}

void CBmpFile::WriteImage( const void* data, CVector2<int> size, TBmpPixelFormat format )
{
	const int bytesPerPixel = format == BPF_Rgb ? 3 : 4;
	const int bitsPerPixel = 8 * bytesPerPixel;
	const int rowSize = CeilTo( bytesPerPixel * size.X(), 4 );
	const int dataSize = rowSize * size.Y();

	if( format == BPF_Rgb ) {
		writeBmpInfoHeader( size, dataSize, bitsPerPixel );
	} else {
		writeBmpV4Header( size, dataSize );
	}
	fileData.Write( data, dataSize );
}

static const int generalHeaderSize = 14;
void CBmpFile::writeBmpInfoHeader( CVector2<int> size, int imageByteSize, int bitsPerPixel )
{
	BYTE headerData[generalHeaderSize + sizeof( CBitmapInfoHeader )];
	const int dataOffset = 54;
	writeGeneralHeader( headerData, dataOffset + imageByteSize, dataOffset );

	// Bitmap specific data.
	CBitmapInfoHeader header;
	fillBitmapInfo( header, sizeof( header ), size, 0, imageByteSize, bitsPerPixel );

	memcpy( headerData + generalHeaderSize, &header, sizeof( header ) );
	fileData.Write( headerData, sizeof( headerData ) );
}

void CBmpFile::writeGeneralHeader( BYTE* dest, int totalSize, int dataOffset )
{
	// Magic number "BM".
	dest[0] = 0x42;
	dest[1] = 0x4D;
	// Total file size.
	memcpy( dest + 2, &totalSize, sizeof( totalSize ) );
	// Offset of the data.
	memcpy( dest + 10, &dataOffset, sizeof( dataOffset ) );
}

void CBmpFile::fillBitmapInfo( CBitmapInfoHeader& header, int headerSize, CVector2<int> size, int compressionMethod, int imageByteSize, int bpp )
{
	header.HeaderSize = headerSize;
	header.PixelWidth = size.X();
	header.PixelHeight = size.Y();
	header.ColorPlanesCount = 1;
	header.Bpp = numeric_cast<short>( bpp );
	header.CompressionMethod = compressionMethod;
	header.ImageSize = imageByteSize;
	header.ResolutionX = 2835;
	header.ResolutionY = 2835;
	header.ColorCount = 0;
	header.ImportantColorCount = 0;
}

void CBmpFile::writeBmpV4Header( CVector2<int> size, int imageByteSize )
{
	BYTE headerData[generalHeaderSize + sizeof( CBitmapV4Header )];
	const int dataOffset = 122;
	writeGeneralHeader( headerData, dataOffset + imageByteSize, dataOffset );

	CBitmapV4Header header;
	fillBitmapInfo( header.BitmapInfo, sizeof( header ), size, BI_BITFIELDS, imageByteSize, 32 );
	header.RedMask = 0x00FF0000;
	header.GreenMask = 0x0000FF00;
	header.BlueMask = 0x000000FF;
	header.AlphaMask = 0xFF000000;
	memset( &header.CSType, 0, sizeof( header ) - sizeof( header.BitmapInfo ) - 4 * sizeof( unsigned ) );

	memcpy( headerData + generalHeaderSize, &header, sizeof( header ) );
	fileData.Write( headerData, sizeof( headerData ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
