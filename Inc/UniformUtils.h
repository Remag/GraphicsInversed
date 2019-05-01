#pragma once
#include <DrawEnums.h>
#include <Shader.h>
#include <TextureData.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Shader program uniform data. Contains all the information from the call to glGetActiveUniform.
struct CRawUniformData {
	CString Name;
	int Location;
	TGlType Type;
	int Size;
	// Binding point to be used for texture uniforms. NotFound otherwise.
	int BindingPoint;
	// Set flag indicating that this uniform's value has been changed at least once.
	mutable bool IsSetFlag = false;
};

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

// Uniform set functions. A correct shader program is assumed to be bound.
void GINAPI SetUniformValue( const CRawUniformData& data, int newValue );
void GINAPI SetUniformValue( const CRawUniformData& data, float result );
void GINAPI SetUniformValue( const CRawUniformData& data, CColor newValue );
void GINAPI SetUniformValue( const CRawUniformData& data, CVector2<int> result );
void GINAPI SetUniformValue( const CRawUniformData& data, const CVector3<int>& result );
void GINAPI SetUniformValue( const CRawUniformData& data, const CVector4<int>& result );
void GINAPI SetUniformValue( const CRawUniformData& data, CVector2<float> result );
void GINAPI SetUniformValue( const CRawUniformData& data, const CVector3<float>& result );
void GINAPI SetUniformValue( const CRawUniformData& data, const CVector4<float>& result );

void GINAPI SetMatrix2Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix23Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix32Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix3Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix34Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix43Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetMatrix4Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose );
void GINAPI SetTextureUniformValue( const CRawUniformData& data, CTextureData base, TTextureBindingTarget target );

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CConstTextureBase<format, TBT_Texture1> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_Texture1 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CConstTextureBase<format, TBT_Texture2> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_Texture2 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CConstTextureBase<format, TBT_TextureArray2> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_TextureArray2 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CConstTextureBase<format, TBT_CubeMap> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_CubeMap );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CEditTextureBase<format, TBT_Texture1> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_Texture1 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CEditTextureBase<format, TBT_Texture2> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_Texture2 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CEditTextureBase<format, TBT_TextureArray2> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_TextureArray2 );
}

template <TTextureGlFormat format>
void SetUniformValue( const CRawUniformData& data, CEditTextureBase<format, TBT_CubeMap> newValue )
{
	SetTextureUniformValue( data, newValue.GetTextureData(), TBT_CubeMap );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 2, 2, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True: GB_False;
	SetMatrix2Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 2, 3, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix23Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 3, 2, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix32Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 3, 3, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix3Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 3, 4, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix34Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 4, 3, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix43Value( data, result.Ptr(), needTranspose );
}

template <TMatrixOrder order>
void SetUniformValue( const CRawUniformData& data, const CMatrix<float, 4, 4, order>& result )
{
	const BYTE needTranspose = order == MO_RowMajor ? GB_True : GB_False;
	SetMatrix4Value( data, result.Ptr(), needTranspose );
}

//////////////////////////////////////////////////////////////////////////

// Set uniform value that is undefined at compile time.
inline void SetRawUniformValue( const CRawUniformData& data, CConstRawBuffer newValue )
{
	switch( data.Type ) {
		case GLT_Int:
			SetUniformValue( data, newValue.As<int>() );
			break;
		case GLT_Float:
			SetUniformValue( data, newValue.As<float>() );
			break;
		case GLT_Vec2Float:
			SetUniformValue( data, newValue.As<CVector2<float>>() );
			break;
		case GLT_Vec2Int:
			SetUniformValue( data, newValue.As<CVector2<int>>() );
			break;
		case GLT_Vec3Float:
			SetUniformValue( data, newValue.As<CVector3<float>>() );
			break;
		case GLT_Vec3Int:
			SetUniformValue( data, newValue.As<CVector3<int>>() );
			break;
		case GLT_Vec4Float:
			SetUniformValue( data, newValue.As<CVector4<float>>() );
			break;
		case GLT_Vec4Int:
			SetUniformValue( data, newValue.As<CVector4<int>>() );
			break;
		case GLT_Mat2:
			SetUniformValue( data, newValue.As<CMatrix2<float>>() );
			break;
		case GLT_Mat2x3:
			SetUniformValue( data, newValue.As<CMatrix<float, 2, 3>>() );
			break;
		case GLT_Mat3x2:
			SetUniformValue( data, newValue.As<CMatrix<float, 3, 2>>() );
			break;
		case GLT_Mat3:
			SetUniformValue( data, newValue.As<CMatrix3<float>>() );
			break;
		case GLT_Mat3x4:
			SetUniformValue( data, newValue.As<CMatrix<float, 3, 4>>() );
			break;
		case GLT_Mat4x3:
			SetUniformValue( data, newValue.As<CMatrix<float, 4, 3>>() );
			break;
		case GLT_Mat4:
			SetUniformValue( data, newValue.As<CMatrix4<float>>() );
			break;
		case GLT_Texture1:
			SetTextureUniformValue( data, newValue.As<CTextureData>(), TBT_Texture1 );
			break;
		case GLT_Texture2:
			SetTextureUniformValue( data, newValue.As<CTextureData>(), TBT_Texture2 );
			break;
		case GLT_TextureArray2:
			SetTextureUniformValue( data, newValue.As<CTextureData>(), TBT_TextureArray2 );
			break;
		case GLT_TextureCube:
			SetTextureUniformValue( data, newValue.As<CTextureData>(), TBT_CubeMap );
			break;
		default:
			assert( false );
			return;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


