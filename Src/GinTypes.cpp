#include <common.h>
#pragma hdrstop

#include <GinTypes.h>
#include <TextureWrappers.h>

namespace Gin {

namespace GinTypes {

//////////////////////////////////////////////////////////////////////////

static CGlTypeInfo GlTypeInformationList[] = {
	{ GLT_Int, GLT_Int, 1, 4, 4, true },
	{ GLT_UnsignedInt, GLT_UnsignedInt, 1, 4, 4, true },
	{ GLT_Vec2Int, GLT_Int, 2, 8, 4, true },
	{ GLT_Vec3Int, GLT_Int, 3, 12, 4, true },
	{ GLT_Vec4Int, GLT_Int, 4, 16, 4, true },

	{ GLT_Float, GLT_Float, 1, 4, 4, false },
	{ GLT_Vec2Float, GLT_Float, 2, 8, 4, false },
	{ GLT_Vec3Float, GLT_Float, 3, 12, 4, false },
	{ GLT_Vec4Float, GLT_Float, 4, 16, 4, false },

	{ GLT_Mat2, GLT_Float, 4, 16, 4, false },
	{ GLT_Mat2x3, GLT_Float, 6, 24, 4, false },
	{ GLT_Mat3x2, GLT_Float, 6, 24, 4, false },
	{ GLT_Mat3, GLT_Float, 9, 36, 4, false },
	{ GLT_Mat3x4, GLT_Float, 12, 48, 4, false },
	{ GLT_Mat4x3, GLT_Float, 12, 48, 4, false },
	{ GLT_Mat4, GLT_Float, 16, 64, 4, false },

	{ GLT_Texture1, GLT_Texture1, 1, sizeof( GinInternal::CTextureData ), alignof( GinInternal::CTextureData ), false },
	{ GLT_Texture2, GLT_Texture2, 1, sizeof( GinInternal::CTextureData ), alignof( GinInternal::CTextureData ), false },
	{ GLT_TextureArray2, GLT_TextureArray2, 1, sizeof( GinInternal::CTextureData ), alignof( GinInternal::CTextureData ), false },
	{ GLT_TextureCube, GLT_TextureCube, 1, sizeof( GinInternal::CTextureData ), alignof( GinInternal::CTextureData ), false }

};

CGlTypeInfo GinTypes::GetGlTypeInformation( TGlType type )
{
	for( const auto& glType : GlTypeInformationList ) {
		if( glType.Type == type ) { 
			return glType;
		}
	}
	assert( false );
	return GlTypeInformationList[0];
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinTypes.

}	// namespace Gin.
