#pragma once
#include <Mesh.h>
#include <GlBuffer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Rectangular mesh and its buffer.
class GINAPI CQuad {
public:
	CQuad();
	explicit CQuad( CAARect<float> rect );

	CQuadMesh GetMesh() const
		{ return mesh; }

	// Fill the quad with rect data.
	void SetBaseRect( CAARect<float> rect );
	// Set the quad data in a raw form. Data should be presented in the the following order: BL, BR, TL, TR.
	void SetRawData( CAARect<float> rect, const CStackArray<CVector2<float>, 4>& data );
	// Call the draw command on the mesh. Shader program must be active.
	void Draw( CShaderProgram program ) const;

private:
	CMeshOwner<CQuadMesh> mesh;
	CGlBufferOwner<BT_Array, CVector4<float>> meshBuffer;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

