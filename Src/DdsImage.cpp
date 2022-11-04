#include <common.h>
#pragma hdrstop

#include <DdsImage.h>
#include <ImageData.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Table that maps old representation of pixel formats to a corresponding DXGI format.
static const DDS::CDXGIToBitMaskEntry bitMaskToDXGIConversionTable[] {
	// Common uncompressed formats.
	{ DDS::DXGIF_R8G8B8A8_UNORM, DDS::DPFF_RGB | DDS::DPFF_AlphaPixels, 32, 0xff, 0xff00, 0xff0000, 0xff000000, 0 },
	{ DDS::DXGIF_B8G8R8A8_UNORM, DDS::DPFF_RGB | DDS::DPFF_AlphaPixels, 32, 0xff0000, 0xff00, 0xff, 0xff000000, 0 },
	// Common compressed formats.
	{ DDS::DXGIF_BC1_UNORM, DDS::DPFF_FourCC, 0, 0, 0, 0, 0, DDS::DCF_DXT1 },
	{ DDS::DXGIF_BC2_UNORM, DDS::DPFF_FourCC, 0, 0, 0, 0, 0, DDS::DCF_DXT3 },
	{ DDS::DXGIF_BC3_UNORM, DDS::DPFF_FourCC, 0, 0, 0, 0, 0, DDS::DCF_DXT5 },
	// Less common formats.
	{ DDS::DXGIF_R16G16_UNORM, DDS::DPFF_RGB | DDS::DPFF_AlphaPixels, 32, 0xffff, 0xffff0000, 0, 0, 0 },
	{ DDS::DXGIF_R10G10B10A2_UNORM, DDS::DPFF_RGB | DDS::DPFF_AlphaPixels, 32, 0x3ff, 0xffc00, 0x3ff00000, 0, 0 },
	{ DDS::DXGIF_R16G16_UNORM, DDS::DPFF_RGB, 32, 0xffff, 0xffff0000, 0, 0, 0 },
	{ DDS::DXGIF_B5G5R5A1_UNORM, DDS::DPFF_RGB | DDS::DPFF_AlphaPixels, 16, 0x7c00, 0x3e0, 0x1f, 0x8000, 0 },
	{ DDS::DXGIF_B5G6R5_UNORM, DDS::DPFF_RGB, 16, 0xf800, 0x7e0, 0x1f, 0, 0 },
	{ DDS::DXGIF_R8_UNORM, DDS::DPFF_Luminance, 8, 0xff, 0, 0, 0, 0 },
	{ DDS::DXGIF_R16_UNORM, DDS::DPFF_Luminance, 16, 0xffff, 0, 0, 0, 0 },
	{ DDS::DXGIF_A8_UNORM, DDS::DPFF_Alpha, 8, 0, 0, 0, 0xff, 0 },
	{ DDS::TDXGIFormat( 0 ), 0, 0, 0, 0, 0, 0 }
};

// Find a DXGI format from old format representation data.
static DDS::TDXGIFormat findDxgiFormatFromTable( DWORD flags, DWORD bitsPerPixel, DWORD rMask, DWORD gMask, DWORD bMask, DWORD aMask, DWORD fourCC, const DDS::CDXGIToBitMaskEntry* table )
{
	for( int i = 0; table[i].Format != 0; i++ ) {
		const DDS::CDXGIToBitMaskEntry& entry = table[i];
		if( HasFlag( flags, entry.Flags ) && 
			( entry.RgbBitCount == 0 || bitsPerPixel == entry.RgbBitCount ) &&
			( entry.RBitMask == 0 || rMask == entry.RBitMask ) &&
			( entry.GBitMask == 0 || gMask == entry.GBitMask ) &&
			( entry.BBitMask == 0 || bMask == entry.BBitMask ) &&
			( entry.ABitMask == 0 || aMask == entry.ABitMask ) &&
			( entry.FourCC == 0 || fourCC == entry.FourCC ) )
		{
			return entry.Format;
		}
	}
	return DDS::DXGIF_Unknown;
}

//////////////////////////////////////////////////////////////////////////

CDdsImage::CDdsImage( CStringPart fileName ) :
	textureFileName( fileName )
{
}

static const int minDdsFileSize = sizeof( DWORD ) + sizeof( DDS::CDdsHeader );
CImageData CDdsImage::CreateImageData( bool topLeftOrigin )
{
	CArray<BYTE> data = readFileBuffer();
	parseMagicNumber( data );
	DDS::CDdsHeader header;
	parseDdsHeader( data, header );
	DDS::CDxt10Header dxtHeader;
	int filePos = minDdsFileSize;
	getOrCreateDxtHeader( data, filePos, header, dxtHeader );
	return parseTextureData( data, filePos, header, dxtHeader, topLeftOrigin );
}

CArray<BYTE> CDdsImage::readFileBuffer()
{
	CFileReader textureFile( textureFileName, FCM_OpenExisting );
	const int length = textureFile.GetLength32();
	if( length < minDdsFileSize ) {
		throwDdsException( "File size is too small to support DDS format" );
	}

	CArray<BYTE> buffer;
	buffer.IncreaseSizeNoInitialize( length );
	textureFile.Read( buffer.Ptr(), length );
	return move( buffer );
}

static const DWORD ddsMagicNumber = 0x20534444;
// Check the presence of a DDS magic at the start of the file.
void CDdsImage::parseMagicNumber( CArray<BYTE>& data )
{
	DWORD fileStart;
	memcpy( &fileStart, data.Ptr(), sizeof( fileStart ) );
	if( ddsMagicNumber != fileStart ) {
		throwDdsException( "Specified file is not DDS" );
	}
}

void CDdsImage::throwDdsException( CStringView reason ) const
{
	throw CDdsException( GetName(), reason );
}

// Fill the CDdsHeader structure.
void CDdsImage::parseDdsHeader( CArray<BYTE>& data, DDS::CDdsHeader& result )
{
	memcpy( &result, data.Ptr() + sizeof( ddsMagicNumber ), sizeof( result ) );
}

// Does the file with the given header have an optional DXT10 header?
bool CDdsImage::hasDxtHeader( const DDS::CDdsHeader& header )
{
	return HasFlag( header.PixelFormat.Flags, DDS::DPFF_FourCC ) && header.PixelFormat.FourCC == DDS::DCF_DX10;
}

// Fill the CDxt10Header structure.
void CDdsImage::getOrCreateDxtHeader( CArray<BYTE>& data, int& pos, const DDS::CDdsHeader& header, DDS::CDxt10Header& result )
{
	if( hasDxtHeader( header ) ) {
		// Read an existing DX10 header.
		if( data.Size() < minDdsFileSize + sizeof( result ) ) {
			throwDdsException( "File size is too small to support DDS format" );
		}
		memcpy( &result, data.Ptr() + pos, sizeof( result ) );
		pos += sizeof( result );
	} else {
		createDxtHeader( header, result );
	}
}

// Fill the CDxt10Header structure manually.
void CDdsImage::createDxtHeader( const DDS::CDdsHeader& header, DDS::CDxt10Header& result )
{
	// Set the dimensionality.
	if( HasFlag( header.Caps2, DDS::DC2F_Volume ) && HasFlag( header.Flags, DDS::DHF_Depth ) && header.Depth > 1 ) {
		result.ResourceDimension = DDS::D3D10RD_Texture3D;
	} else if( header.Height == 1 ) {
		result.ResourceDimension = DDS::D3D10RD_Texture1D;
	} else {
		result.ResourceDimension = DDS::D3D10RD_Texture2D;
	}

	// Set the cubemap property.
	if( HasFlag( header.Caps2, DDS::DC2F_Cubemap ) ) {
		if( header.Caps2 != ( DDS::DC2F_Cubemap | DDS::DC2F_Cubemap_AllFaces ) ) {
			throwDdsException( "Cubemap is missing faces" );
		}
		result.MiscFlag = DDS::DMF_TextureCube;
	} else {
		result.MiscFlag = 0;
	}

	// These parameters cannot be deduced.
	result.ArraySize = 1;
	result.DxgiFormat = DDS::DXGIF_Unknown;
	result.MiscFlags2 = 0;	
}

// Create the CTextureData structure.
CImageData CDdsImage::parseTextureData( CArray<BYTE>& data, int pos, const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, bool shouldFlip )
{
	// Get dimensions.
	int width, height, depth;
	TTextureType textureType;
	getTextureParameters( header, dxtHeader, textureType, width, height, depth );

	// Get texture count.
	const int mipmapCount = HasFlag( header.Flags, DDS::DHF_MimmapCount ) ? header.MipMapCount : 1;
	const int arrayCount = dxtHeader.ArraySize;
	const int cubeFaceCount = textureType == TT_TextureCubeMap ? 6 : 1;

	// Get texel parameters.
	TTextureCompressionType compressionType;
	TTexelFormat format;
	TTexelDataType dataType;
	getTexelParameters( header, dxtHeader, compressionType, format, dataType );

	// Create the texture data.
	CImageData result = createTextureData( textureType, width, height, depth, compressionType, format, dataType, mipmapCount, arrayCount );

	// Fill in the data.
	try {
		for( int arrayPos = 0; arrayPos < arrayCount; arrayPos++ ) {
			for( int facePos = 0; facePos < cubeFaceCount; facePos++ ) {
				for( int mipmapPos = 0; mipmapPos < mipmapCount; mipmapPos++ ) {
					result.SetImageData( mipmapPos, arrayPos, facePos, data.Ptr() + pos, shouldFlip );
					const int dataSize = result.GetImageDataSize( mipmapPos );
					pos += dataSize;
				}
			}
		}
	} catch( CCheckException& e ) {
		// Fill the file name information.
		e.SetFirstParam( GetName() );
		throw;
	}
	return result;
}

void CDdsImage::getTextureParameters( const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, TTextureType& type, int& width, int& height, int& depth ) const
{
	switch( dxtHeader.ResourceDimension ) {
	case DDS::D3D10RD_Texture1D:
		width = header.Width;
		height = 1;
		depth = 1;
		type = TT_Texture1D;
		break;
	case DDS::D3D10RD_Texture2D:
		width = header.Width;
		height = header.Height;
		depth = 1;
		type = HasFlag( dxtHeader.MiscFlag, DDS::DMF_TextureCube ) ? TT_TextureCubeMap : TT_Texture2D;
		break;
	case DDS::D3D10RD_Texture3D:
		width = header.Width;
		height = header.Height;
		depth = header.Depth;
		type = TT_Texture3D;
		break;
	default:
		throwDdsException( "Unknown texture type" );
		break;
	}
}

// Get texture pixel parameters.
void CDdsImage::getTexelParameters( const DDS::CDdsHeader& header, const DDS::CDxt10Header& dxtHeader, TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType ) const
{
	const DDS::TDXGIFormat dxformat = dxtHeader.DxgiFormat == DDS::DXGIF_Unknown
		? findDxgiFormat( header )
		: dxtHeader.DxgiFormat;

	getDxtTexelParameters( dxformat, compressionType, format, dataType );
}

// Get the texel information from the old DDS header.
DDS::TDXGIFormat CDdsImage::findDxgiFormat( const DDS::CDdsHeader& header ) const
{
	const DDS::CDdsPixelFormat& format = header.PixelFormat;
	DDS::TDXGIFormat result = findDxgiFormatFromTable( format.Flags, format.RGBBitCount, format.RBitMask, 
		format.GBitMask, format.BBitMask, format.ABitMask, format.FourCC, bitMaskToDXGIConversionTable );
	
	if( result == DDS::DXGIF_Unknown ) {
		throwDdsException( "Unknown pixel format" );
	}
	return result;
}

// Get texture pixel parameters from the DX10 header.
void CDdsImage::getDxtTexelParameters( DDS::TDXGIFormat dxgiFormat, TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType ) const
{
	switch( dxgiFormat ) {
	case DDS::DXGIF_R8G8B8A8_UNORM:
	case DDS::DXGIF_R8G8B8A8_UNORM_SRGB:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_RGBA, TDT_UnsignedByte );
		break;
	case DDS::DXGIF_R8G8B8A8_SNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_RGBA, TDT_Byte );
		break;
	case DDS::DXGIF_B8G8R8A8_UNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_BGRA, TDT_UnsignedByte );
		break;
	case DDS::DXGIF_R16G16_SNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_RG, TDT_Short );
		break;
	case DDS::DXGIF_R8G8_SNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_RG, TDT_Byte );
		break;
	case DDS::DXGIF_R8_UNORM:
	case DDS::DXGIF_A8_UNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Uncompressed, TF_Red, TDT_UnsignedByte );
		break;
	case DDS::DXGIF_BC1_UNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt1_RGB, TF_Compressed, TDT_Compressed );
		break;
	case DDS::DXGIF_BC1_UNORM_SRGB:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt1_sRGB, TF_Compressed, TDT_Compressed );
		break;
	case DDS::DXGIF_BC2_UNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt3, TF_Compressed, TDT_Compressed );
		break;
	case DDS::DXGIF_BC2_UNORM_SRGB:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt3_sRGBA, TF_Compressed, TDT_Compressed );
		break;
	case DDS::DXGIF_BC3_UNORM:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt5, TF_Compressed, TDT_Compressed );
		break;
	case DDS::DXGIF_BC3_UNORM_SRGB:
		setTexelParameters( compressionType, format, dataType, TCT_Dxt5_sRGBA, TF_Compressed, TDT_Compressed );
		break;
	default:
		throwDdsException( "Unsupported DXGI format." );
		break;
	}
}

void CDdsImage::setTexelParameters( TTextureCompressionType& compressionType, TTexelFormat& format, TTexelDataType& dataType,
	TTextureCompressionType compValue, TTexelFormat formatValue, TTexelDataType dataValue )
{
	compressionType = compValue;
	format = formatValue;
	dataType = dataValue;
}

// Create the data structure that allocates memory for the data.
CImageData CDdsImage::createTextureData( TTextureType type, int width, int height, int depth, TTextureCompressionType compressionType, TTexelFormat texelFormat,
	TTexelDataType texelDataType, int mipmapCount, int arrayCount ) const
{
	if( compressionType == TCT_Uncompressed ) {
		return CImageData( type, width, height, depth, texelFormat, texelDataType, mipmapCount, arrayCount );
	} else {
		return CImageData( type, width, height, depth, compressionType, mipmapCount, arrayCount );
	}
}

void CDdsImage::WriteImageData( const CImageData& data )
{
	CArray<BYTE> result;

	// Find and reserve result buffer size.
	const int magicSize = sizeof( ddsMagicNumber );
	int resultSize = sizeof( DDS::CDdsHeader ) + sizeof( DDS::CDxt10Header ) + magicSize;
	for( int i = 0; i < data.GetMipmapCount(); i++ ) {
		resultSize += data.GetImageDataSize( i ) * data.GetArrayCount() * data.GetCubeFaceCount();
	}
	result.IncreaseSizeNoInitialize( resultSize );

	// Write the magic number.
	int resultPos = 0;
	memcpy( result.Ptr(), &ddsMagicNumber, magicSize );
	resultPos += magicSize;

	// Write the headers.
	const DDS::CDdsHeader ddsHeader = createDdsHeader( data );
	memcpy( result.Ptr() + resultPos, &ddsHeader, sizeof( ddsHeader ) );
	resultPos += sizeof( ddsHeader );
	const DDS::CDxt10Header dxtHeader = createDxtHeader( data );
	memcpy( result.Ptr() + resultPos, &dxtHeader, sizeof( dxtHeader ) );
	resultPos += sizeof( dxtHeader );

	// Write texture data.
	for( int arrayIndex = 0; arrayIndex < data.GetArrayCount(); arrayIndex++ ) {
		for( int cubeFaceIndex = 0; cubeFaceIndex < data.GetCubeFaceCount(); cubeFaceIndex++ ) {
			for( int mipmapIndex = 0; mipmapIndex < data.GetMipmapCount(); mipmapIndex++ ) {
				const int imageSize = data.GetImageDataSize( mipmapIndex );
				assert( resultPos + imageSize <= result.Size() );
				memcpy( result.Ptr() + resultPos, data.GetImageData( mipmapIndex, arrayIndex, cubeFaceIndex ), imageSize );
				resultPos += imageSize;
			}
		}
	}

	// Create an actual file and write the whole buffer to it.
	CFileWriter textureFile( textureFileName, FCM_CreateAlways );
	textureFile.Write( result.Ptr(), result.Size() );
}

DDS::CDdsHeader CDdsImage::createDdsHeader( const CImageData& data ) const
{
	DDS::CDdsHeader result;
	result.Size = sizeof( result );
	
	result.Flags = DDS::DHF_Caps | DDS::DHF_Height | DDS::DHF_Width | DDS::DHF_PixelFormat | DDS::DHF_MimmapCount;
	result.Height = data.Height();
	result.Width = data.Width();
	result.Depth = data.Depth();
	result.MipMapCount = data.GetMipmapCount();
	result.PixelFormat = createPixelFormat();
	result.Caps = DDS::DC1F_Texture;

	switch( data.GetType() ) {
		case TT_TextureCubeMap:
			result.Caps2 = DDS::DC2F_Cubemap | DDS::DC2F_Cubemap_AllFaces;
		break;
		case TT_Texture3D:
			result.Caps2 = DDS::DC2F_Volume;
			result.Flags |= DDS::DHF_Depth;
		break;
		default:
			result.Caps2 = 0;
		break;
	}
	return result;
}

DDS::CDdsPixelFormat CDdsImage::createPixelFormat() const
{
	DDS::CDdsPixelFormat result;

	result.Size = sizeof( result );
	result.Flags = DDS::DPFF_FourCC;
	result.FourCC = DDS::DCF_DX10;
	return result;
}

DDS::CDxt10Header CDdsImage::createDxtHeader( const CImageData& data ) const
{
	DDS::CDxt10Header result;
	result.DxgiFormat = getDxgiFormatFromData( data );
	result.ResourceDimension = data.Depth() > 1 ? DDS::D3D10RD_Texture3D :
		data.Height() > 1 ? DDS::D3D10RD_Texture2D : DDS::D3D10RD_Texture1D;
	
	result.MiscFlag = data.GetType() == TT_TextureCubeMap ? DDS::DMF_TextureCube : 0;
	result.ArraySize = data.GetArrayCount();
	result.MiscFlags2 = 0;

	return result;
}

DDS::TDXGIFormat CDdsImage::getDxgiFormatFromData( const CImageData& data ) const
{
	if( data.GetCompressionType() == TCT_Uncompressed ) {
		// The format is defined by the size and pixel format.
		return getDxgiFormatFromTexelFormat( data.GetTexelFormat(), data.GetTexelDataType() );
	} else {
		// The format is defined by compression type.
		return getDxgiFormatFromCompressionType( data.GetCompressionType() );
	}
}

DDS::TDXGIFormat CDdsImage::getDxgiFormatFromTexelFormat( TTexelFormat format, TTexelDataType dataType ) const
{
	if( format == TF_Red && dataType == TDT_UnsignedByte ) {
		return DDS::DXGIF_R8_UNORM;				
	} else if( format == TF_BGRA && dataType == TDT_UnsignedByte ) {
		return DDS::DXGIF_B8G8R8A8_UNORM;		

	} else if( format == TF_RG ) {
		switch( dataType ) {
			case TDT_Byte:
				return DDS::DXGIF_R8G8_SNORM;
			case TDT_Short:
				return DDS::DXGIF_R16G16_SNORM;
			default:
				break;
		}

	} else if( format == TF_RGBA ) {
		switch( dataType ) {
			case TDT_UnsignedByte:
				return DDS::DXGIF_R8G8B8A8_UNORM;
			case TDT_Byte:
				return DDS::DXGIF_R8G8B8A8_SNORM;
			default:
				break;
		}
	}
	
	throwDdsException( "Unsupported DXGI format." );
	return DDS::TDXGIFormat();
}

DDS::TDXGIFormat CDdsImage::getDxgiFormatFromCompressionType( TTextureCompressionType type ) const
{
	switch( type ) {
		case TCT_Dxt1_RGB:
			return DDS::DXGIF_BC1_UNORM;
		case TCT_Dxt3:
			return DDS::DXGIF_BC2_UNORM;
		case TCT_Dxt5:
			return DDS::DXGIF_BC3_UNORM;
		case TCT_Dxt1_sRGB:
			return DDS::DXGIF_BC1_UNORM_SRGB;
		case TCT_Dxt3_sRGBA: 
			return DDS::DXGIF_BC2_UNORM_SRGB;
		case TCT_Dxt5_sRGBA:
			return DDS::DXGIF_BC3_UNORM_SRGB;
		default:
			assert( false );
	}
	return DDS::TDXGIFormat();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

