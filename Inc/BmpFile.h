#pragma once
#include <GinDefs.h>

namespace Gin {

struct CBitmapInfoHeader;
//////////////////////////////////////////////////////////////////////////

enum TBmpPixelFormat {
	BPF_Rgb,
	BPF_Rgba,
};

//////////////////////////////////////////////////////////////////////////

// Class that facilitates writing to a BMP file.
class GINAPI CBmpFile {
public:
	CBmpFile() = default;
	explicit CBmpFile( CStringView fileName );

	// Check if the file has been opened.
	bool IsOpen() const
	{ return fileData.IsOpen(); }
	// Open a given file for writing.
	void Open( CStringView fileName );

	// Write image in an uncompressed BMP format.
	// size is given in pixels. 4-byte row alignment is assumed.
	void WriteImage( const void* data, CVector2<int> size, TBmpPixelFormat format );
	
private:
	// File with the image data.
	CDynamicFile fileData;

	void writeGeneralHeader( BYTE* dest, int totalSize, int dataOffset );
	void fillBitmapInfo( CBitmapInfoHeader& header, int headerSize, CVector2<int> size, int compressionMethod, int byteSize, int bpp );
	void writeBmpInfoHeader( CVector2<int> size, int byteSize, int bpp );
	void writeBmpV4Header( CVector2<int> size, int byteSize );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

