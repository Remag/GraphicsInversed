#include <common.h>
#pragma hdrstop

#include <TextureUtils.h>

namespace Gin {

namespace TextureUtils {

//////////////////////////////////////////////////////////////////////////

static int findUnpackedBytesPerPixel( TTexelFormat texelFormat, int elemSize )
{
	switch( texelFormat ) {
	case TF_Red:
	case TF_Depth:
	case TF_RedInt:
	case TF_StencilIndex:
		return elemSize;
	case TF_RG:
	case TF_RGInt:
	case TF_DepthStencil:
		return elemSize * 2;
	case TF_RGB:
	case TF_BGR:
	case TF_RGBInt:
	case TF_BGRInt:
		return elemSize * 3;
	case TF_RGBA:
	case TF_BGRA:
	case TF_RGBAInt:
	case TF_BGRAInt:
		return elemSize * 4;
	case TF_Compressed:
	default:
		assert( false );
		return NotFound;
	}
}

int FindBytesPerPixel( TTexelDataType texelDataType, TTexelFormat texelFormat )
{
	switch( texelDataType ) {
	case TDT_UnsignedByte:
	case TDT_Byte:
		return findUnpackedBytesPerPixel( texelFormat, 1 );
	case TDT_UnsignedShort:
	case TDT_Short:
		return findUnpackedBytesPerPixel( texelFormat, 2 );
	case TDT_UnsignedInt:
	case TDT_Int:
	case TDT_Float:
		return findUnpackedBytesPerPixel( texelFormat, 4 );
	case TDT_UnsignedByte332: 
	case TDT_UnsingedByte233Rev:
		return 1;
	case TDT_UnsignedShort565:
	case TDT_UnsignedShort565Rev:
	case TDT_UnsignedShort4444:
	case TDT_UnsignedShort4444Rev:
	case TDT_UnsignedShort5551: 
	case TDT_UnsignedShort1555Rev:
		return 2;
	case TDT_UnsignedInt8888:
	case TDT_UnsignedInt8888Rev:
	case TDT_UnsignedInt1010102:
	case TDT_UnsignedInt2101010Rev:
		return 4;
	case TDT_Compressed:
	default:
		assert( false );
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace TextureUtils.

}	// namespace Gin.
