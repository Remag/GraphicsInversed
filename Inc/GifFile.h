#pragma once
#include <Gindefs.h>

//////////////////////////////////////////////////////////////////////////

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Exception that occurs while trying to extract data from a GIF file.
class GINAPI CGifException : public CFileWrapperException {
public:
	CGifException( CUnicodePart fileName, CUnicodePart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}

	virtual CUnicodeString GetMessageTemplate() const override
		{ return UnicodeStr( generalGifFileError ); }

private:
	static const CUnicodeView generalGifFileError;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for GIF file decompression.
class GINAPI CGifFile {
public:
	explicit CGifFile( CUnicodePart fileName );

	// Read the contents of the GIF file and decompress them as a 32bit RGBA image.
	// Image pixel dimensions are returned in resultSize.
	void Read( CArray<CColor>& result, CVector2<int>& resultSize ) const;

	// Read directly from the provided array.
	static void ReadRawData( CArrayView<BYTE> pngData, CArray<CColor>& result, CVector2<int>& resultSize );

private:
	CUnicodeString fileName;

	static void doReadRawData( CUnicodePart fileName, CArrayView<BYTE> pngData, CArray<CColor>& result, CVector2<int>& resultSize );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

