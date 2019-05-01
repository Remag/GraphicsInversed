#include <common.h>
#pragma hdrstop

#include <Quad.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CQuad::CQuad()
{
	meshBuffer.ReserveBuffer( 4, BUH_StaticDraw );
	mesh.BindBuffer( meshBuffer, { 0 } );
}

CQuad::CQuad( CAARect<float> rect )	:
	CQuad()
{
	SetBaseRect( rect );
}

void CQuad::SetBaseRect( CAARect<float> rect )
{
	const CStackArray<CVector2<float>, 4> defaultTextureData {
		CVector2<float>{ 0.0f, 0.0f },
		CVector2<float>{ 1.0f, 0.0f },
		CVector2<float>{ 0.0f, 1.0f },
		CVector2<float>{ 1.0f, 1.0f }
	};
	SetRawData( rect, defaultTextureData );
}

void CQuad::SetRawData( CAARect<float> rect, const CStackArray<CVector2<float>, 4>& data )
{
	// Initialize mesh data.
	const CStackArray<CVector4<float>, 4> meshData {
		CVector4<float>( rect.BottomLeft(), data[0] ),
		CVector4<float>( rect.BottomRight(), data[1] ),
		CVector4<float>( rect.TopLeft(), data[2] ),
		CVector4<float>( rect.TopRight(), data[3] )
	};
	meshBuffer.SetBuffer( meshData );
}

void CQuad::Draw( CShaderProgram program ) const
{
	mesh.Draw( program );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
