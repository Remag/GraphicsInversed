#include <common.h>
#pragma hdrstop

#include <UniformUtils.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

void SetUniformValue( const CRawUniformData& data, int newValue )
{
	gl::Uniform1i( data.Location, newValue );
}

void SetUniformValue( const CRawUniformData& data, float result )
{
	gl::Uniform1f( data.Location, result );
}

void SetUniformValue( const CRawUniformData& data, CColor newValue )
{
	gl::Uniform4f( data.Location, newValue.GetRed(), newValue.GetGreen(), newValue.GetBlue(), newValue.GetAlpha() );
}

void SetUniformValue( const CRawUniformData& data, CVector2<int> result )
{
	gl::Uniform2i( data.Location, result.X(), result.Y() );
}

void SetUniformValue( const CRawUniformData& data, const CVector3<int>& result )
{
	gl::Uniform3i( data.Location, result.X(), result.Y(), result.Z() );
}

void SetUniformValue( const CRawUniformData& data, const CVector4<int>& result )
{
	gl::Uniform4i( data.Location, result.X(), result.Y(), result.Z(), result.W() );
}

void SetUniformValue( const CRawUniformData& data, CVector2<float> result )
{
	gl::Uniform2f( data.Location, result.X(), result.Y() );
}

void SetUniformValue( const CRawUniformData& data, const CVector3<float>& result )
{
	gl::Uniform3f( data.Location, result.X(), result.Y(), result.Z() );
}

void SetUniformValue( const CRawUniformData& data, const CVector4<float>& result )
{
	gl::Uniform4f( data.Location, result.X(), result.Y(), result.Z(), result.W() );
}

void GINAPI GinInternal::SetMatrix2Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix2fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix23Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix2x3fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix32Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix3x2fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix3Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix3fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix34Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix3x4fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix43Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix4x3fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI GinInternal::SetMatrix4Value( const CRawUniformData& data, const float* matrixPtr, BYTE needTranspose )
{
	gl::UniformMatrix4fv( data.Location, 1, needTranspose, matrixPtr );
}

void GINAPI SetTextureUniformValue( const CRawUniformData& data, CTextureData base, TTextureBindingTarget target )
{
	gl::Uniform1i( data.Location, data.BindingPoint );
	base.ConnectBindingPoint( target, data.BindingPoint );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

}	// namespace Gin.
