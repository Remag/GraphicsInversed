#pragma once
#include <GinDefs.h>
#include <TextureWrappers.h>
#include <Mesh.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// A quad in two dimensional space. Stores common sprite parameters.
class GINAPI CSprite {
public:
	// Initialize a sprite without size. Data is assumed to be change regularly.
	CSprite();
	// Initialize the sprite with the given data. Sprite is assumed to not change its base size often.
	explicit CSprite( CVector2<float> imageSize );

	// Access to sprite's base size in model space units.
	CAARect<float> GetBaseRect() const
		{ return baseRect; }
	void SetSize( CVector2<float> newSize );

	// Get/Set sprite's Z value.
	float GetDepth() const
		{ return spriteDepth; }
	void SetDepth( float newDepth )
		{ spriteDepth = newDepth; }
	// Get/Set sprite's animation.
	int GetAnimationIndex() const
		{ return animationIndex; }
	void SetAnimationIndex( int newIndex );

	// Prepare sprite's state and draw the sprite. Shader program must be bound and selected as current.
	void Draw( CShaderProgram program ) const;

private:
	// Sprite's quad size in model space coordinates.
	CAARect<float> baseRect;
	// Z value of a sprite.
	float spriteDepth;

	// Sprite's quad.
	CGlBufferOwner<BT_Array, CVector4<float>> quadData;
	CMeshOwner<CQuadMesh> spriteQuad;
	// Position in sprite's atlas.
	int animationIndex;

	static const int verticesPerQuad = 4;

	void initMesh();

	// Copying is prohibited.
	CSprite( CSprite& ) = delete;
	void operator=( CSprite& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

