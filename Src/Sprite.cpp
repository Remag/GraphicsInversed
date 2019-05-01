#include <common.h>
#pragma hdrstop

#include <Sprite.h>
#include <Shader.h>
#include <TextureBinder.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CSprite::CSprite() :
	spriteDepth( 0.0f ),
	animationIndex( 0 )
{
	initMesh();
}

CSprite::CSprite( CVector2<float> imageSize ) :
	CSprite()
{
	SetSize( imageSize );
}

void CSprite::initMesh()
{
	quadData.ReserveBuffer( verticesPerQuad, BUH_DynamicDraw );
	spriteQuad.BindBuffer( quadData, { 0 } );
}

void CSprite::SetAnimationIndex( int newIndex )
{
	assert( newIndex >= 0 );
	animationIndex = newIndex;
}

void CSprite::SetSize( CVector2<float> newSize )
{
	const float halfWidth = newSize.X() / 2;
	const float halfHeight = newSize.Y() / 2;

	baseRect.SetRect( -halfWidth, halfHeight, halfWidth, -halfHeight );

	CStackArray<CVector4<float>, verticesPerQuad> vectorData {
		CVector4<float>( baseRect.BottomLeft(), 0.0f, 0.0f ),
		CVector4<float>( baseRect.BottomRight(), 1.0f, 0.0f ),
		CVector4<float>( baseRect.TopLeft(), 0.0f, 1.0f ),
		CVector4<float>( baseRect.TopRight(), 1.0f, 1.0f ),
	};
	quadData.SetBuffer( vectorData );
}

void CSprite::Draw( CShaderProgram program ) const
{
	assert( quadData.HasData() );
	assert( !baseRect.IsNull() );
	spriteQuad.Draw( program );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
