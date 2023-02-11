#pragma once
// Gin specific type traits.

#include <DrawEnums.h>
#include <Gindefs.h>

namespace Gin {
	
namespace GinTypes {

//////////////////////////////////////////////////////////////////////////

// Corresponding OpenGl type.

// The structure contains various information about the given size:
// Type - TGlType enumeration.
// InnermostGlType - TGlType enumeration for the inner type of compound types like vector. For simple types the value is the same as Type.
// ElemCount - count of elements in compound type. Equals one in simple types.
template <class ClassType>
struct GlType {
};

template<>
struct GlType<BYTE> {
	static const TGlType Type = GLT_UnsignedByte;
	static const TGlType InnermostGlType = GLT_UnsignedByte;
	static const int ElemCount = 1;
};

template<>
struct GlType<int> {
	static const TGlType Type = GLT_Int;
	static const TGlType InnermostGlType = GLT_Int;
	static const int ElemCount = 1;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<unsigned> {
	static const TGlType Type = GLT_UnsignedInt;
	static const TGlType InnermostGlType = GLT_UnsignedInt;
	static const int ElemCount = 1;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<float> {
	static const TGlType Type = GLT_Float;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 1;
	static const bool ShouldNormalize = false;
};

template<>
struct GlType<CVector2<int>> {
	static const TGlType Type = GLT_Vec2Int;
	static const TGlType InnermostGlType = GLT_Int;
	static const int ElemCount = 2;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<CVector3<int>> {
	static const TGlType Type = GLT_Vec3Int;
	static const TGlType InnermostGlType = GLT_Int;
	static const int ElemCount = 3;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<CVector4<int>> {
	static const TGlType Type = GLT_Vec4Int;
	static const TGlType InnermostGlType = GLT_Int;
	static const int ElemCount = 4;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<CVector2<float>> {
	static const TGlType Type = GLT_Vec2Float;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 2;
	static const bool ShouldNormalize = false;
};

template<>
struct GlType<CVector3<float>> {
	static const TGlType Type = GLT_Vec3Float;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 3;
	static const bool ShouldNormalize = false;
};

template<>
struct GlType<CVector4<float>> {
	static const TGlType Type = GLT_Vec4Float;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 4;
	static const bool ShouldNormalize = false;
};

template<>
struct GlType<CColor> {
	static const TGlType Type = GLT_Vec4Float;
	static const TGlType InnermostGlType = GLT_UnsignedByte;
	static const int ElemCount = 4;
	static const bool ShouldNormalize = true;
};

template<>
struct GlType<CVector2<BYTE>> {
	static const TGlType InnermostGlType = GLT_UnsignedByte;
	static const int ElemCount = 2;
};

template<>
struct GlType<CVector3<BYTE>> {
	static const TGlType InnermostGlType = GLT_UnsignedByte;
	static const int ElemCount = 3;
};

template<>
struct GlType<CVector4<BYTE>> {
	static const TGlType InnermostGlType = GLT_UnsignedByte;
	static const int ElemCount = 4;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 2, 2, matOrder>> {
	static const TGlType Type = GLT_Mat2;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 4;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 2, 3, matOrder>> {
	static const TGlType Type = GLT_Mat2x3;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 6;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 3, 2, matOrder>> {
	static const TGlType Type = GLT_Mat3x2;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 6;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 3, 3, matOrder>> {
	static const TGlType Type = GLT_Mat3;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 9;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 3, 4, matOrder>> {
	static const TGlType Type = GLT_Mat3x4;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 12;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 4, 3, matOrder>> {
	static const TGlType Type = GLT_Mat4x3;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 12;
	static const bool ShouldNormalize = false;
};

template<TMatrixOrder matOrder>
struct GlType<CMatrix<float, 4, 4, matOrder>> {
	static const TGlType Type = GLT_Mat4;
	static const TGlType InnermostGlType = GLT_Float;
	static const int ElemCount = 16;
	static const bool ShouldNormalize = false;
};

template<TTextureGlFormat format>
struct GlType<CTexture<TBT_Texture1, format>> {
	static const TGlType Type = GLT_Texture1;
};

template<TTextureGlFormat format>
struct GlType<CTexture<TBT_Texture2, format>> {
	static const TGlType Type = GLT_Texture2;
};

template<TTextureGlFormat format>
struct GlType<CTexture<TBT_TextureArray2, format>> {
	static const TGlType Type = GLT_TextureArray2;
};

template<TTextureGlFormat format>
struct GlType<CTexture<TBT_CubeMap, format>> {
	static const TGlType Type = GLT_TextureCube;
};

template<class ElemType, int i>
struct GlType<ElemType[i]> {
	// Multiple dimension arrays are prohibited.
	staticAssert( !Types::IsArray<ElemType>::Result );

	static const TGlType Type = GlType<ElemType>::Type;
	static const TGlType InnermostGlType = GlType<ElemType>::InnermostGlType;
	static const int ElemCount = i * GlType<ElemType>::ElemCount;
	static const bool ShouldNormalize = GlType<ElemType>::ShouldNormalize;
};

//////////////////////////////////////////////////////////////////////////

// Structure with all necessary OpenGL type information.
struct CGlTypeInfo {
	TGlType Type;
	TGlType InnerMostType;
	int ElemCount;
	int ByteSize;
	int CAlingment;
	bool ShouldNormalize;
};

// Get the GlType information at runtime.
CGlTypeInfo GINAPI GetGlTypeInformation( TGlType type );

}	// namespace GinTypes.

}	// namespace Gin.

