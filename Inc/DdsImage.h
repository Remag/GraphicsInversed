#pragma once
#include <DrawEnums.h>
#include <Gindefs.h>
#include <DdsUtils.h>

namespace Gin {

class CImageData;
//////////////////////////////////////////////////////////////////////////

// Exception that occurs while trying to extract data from a DDS file.
class GINAPI CDdsException : public CFileWrapperException {
public:
	CDdsException( CStringPart fileName, CStringPart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}

	virtual CString GetMessageTemplate() const override
		{ return Str( generalDdsFileError ); }
	
private:
	static const CStringView generalDdsFileError;
};

//////////////////////////////////////////////////////////////////////////

// Image in a DDS format.
class GINAPI CDdsImage {
public:
	explicit CDdsImage( CStringPart fileName );

	CStringView GetName() const
		{ return textureFileName; }

	// Get all the data associated with the texture.
	// topLeftOrigin indicates that the source image should be flipped vertically to match the bottom left origin in OpenGL.
	CImageData CreateImageData( bool topLeftOrigin = true );
	// Write an existing texture data to the file.
	void WriteImageData( const CImageData& data );
	
private:
	// Name of the file with the texture data.
	CString textureFileName;

	CArray<BYTE> readFileBuffer();
	void parseMagicNumber( CArray<BYTE>& data );
	void parseDdsHeader(  CArray<BYTE>& data, DDS::CDdsHeader& result );
	static bool hasDxtHeader( const DDS::CDdsHeader& header );
	void getOrCreateDxtHeader( CArray<BYTE>& data, int& pos, const DDS::CDdsHeader& header, DDS::CDxt10Header& result );
	void createDxtHeader( const DDS::CDdsHeader& header, DDS::CDxt10Header& result );

	CImageData parseTextureData( CArray<BYTE>& data, int pos, const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, bool shouldFlip );
	void getTextureParameters( const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, TTextureType& type, int& width, int& height, int& depth ) const;
	void getTexelParameters( const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType ) const;
	DDS::TDXGIFormat findDxgiFormat( const DDS::CDdsHeader& header ) const;
	void getDxtTexelParameters( DDS::TDXGIFormat dxgiFormat, TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType ) const;
	static void setTexelParameters( TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType,
		TTextureCompressionType compValue, TTexelFormat formatValue, TTexelDataType dataValue );

	CImageData createTextureData( TTextureType type, int width, int height, int depth, TTextureCompressionType compressionType,
		TTexelFormat _imageFormat, TTexelDataType _pixelFormat, int mipmapCount, int arrayCount ) const;

	void throwDdsException( CStringView reason ) const;

	DDS::CDdsHeader createDdsHeader( const CImageData& data ) const;
	DDS::CDdsPixelFormat createPixelFormat() const;
	DDS::CDxt10Header createDxtHeader( const CImageData& data ) const;
	DDS::TDXGIFormat getDxgiFormatFromData( const CImageData& data ) const;
	DDS::TDXGIFormat getDxgiFormatFromTexelFormat( TTexelFormat format, TTexelDataType dataType ) const;
	DDS::TDXGIFormat getDxgiFormatFromCompressionType( TTextureCompressionType type ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

