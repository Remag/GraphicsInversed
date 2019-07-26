#pragma once
#include <Gindefs.h>

//////////////////////////////////////////////////////////////////////////

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Exception that occurs while trying to extract data from a PNG file.
class GINAPI CPngException : public CFileWrapperException {
public:
	CPngException( CUnicodePart fileName, CUnicodePart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}

	virtual CUnicodeString GetMessageTemplate() const override
		{ return UnicodeStr( generalPngFileError ); }
	
private:
	static const CUnicodeView generalPngFileError;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for PNG file compression and decompression.
class GINAPI CPngFile {
public:
	explicit CPngFile( CUnicodePart fileName );

	// Read the contents of the PNG file and decompress them as a 32bit RGBA image.
	// Image pixel dimensions are returned in resultSize.
	void Read( CArray<CColor>& result, CVector2<int>& resultSize ) const;
	// Compress the given result into a 32bit RGBA PNG file.
	// imageSize specifies the size of the given file data in pixels.
	void Write( CArrayView<CColor> file, CVector2<int> imageSize );
	// Write from a custom color data. rowStride indicates the size in bytes between image rows. Negative row stride indicates a bottom-up image.
	void Write( CArrayView<BYTE> file, TTexelFormat format, int rowStride, CVector2<int> imageSize );

	// Read directly from the provided array.
	static void ReadRawData( CArrayView<BYTE> pngData, CArray<CColor>& result, CVector2<int>& resultSize );

private:
	CUnicodeString fileName;

	unsigned findLibPngFormat( TTexelFormat format ) const;
	void doWrite( const void* fileData, unsigned pngLibFormat, int rowStride, CVector2<int> imageSize );
	void writeCompressedData( CArray<BYTE>& compressedData, int dataSize ) const;

	static void doReadRawData( CUnicodePart fileName, CArrayView<BYTE> pngData, CArray<CColor>& result, CVector2<int>& resultSize );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

