#pragma once
#include <Mesh.h>
#include <GinComponents.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Data associated with a single node of a model.
struct CModelNodeData {
	CMeshOwner<CElementMesh> Mesh;
	CGlBufferOwner<BT_ElementArray, int> Indices;
	TMaterialConstRef Material;

	explicit CModelNodeData( CGlBufferOwner<BT_ElementArray, int>&& indices ) : Mesh( MDM_Triangles, indices ), Indices( move( indices ) ) {}
};

//////////////////////////////////////////////////////////////////////////

// A collection of meshes and materials that constitute a renderable object.
class GINAPI CModel {
public:
	typedef CRawGlBuffer<BT_Array> TAttributeBuffer;
	typedef CRawGlBufferOwner<BT_Array> TAttributeBufferOwner;

	CModel() = default;
	explicit CModel( TAttributeBufferOwner&& _vertexAttributes, CVector3<float> size ) :
		vertexAttributes( move( _vertexAttributes ) ), modelSize( size ) {}
	CModel( CModel&& other ) = default;
	CModel& operator=( CModel&& other ) = default;

	// Size in model space units.
	CVector3<float> Size() const
		{ return modelSize; }
	void SetSize( CVector3<float> newValue )
		{ modelSize = newValue; }
	TAttributeBuffer GetVertexAttributes() const
		{ return vertexAttributes; }

	template <class... Types>
	void SetVertexAttributes( CArrayView<CTuple<Types...>> newValue, TBufferUsageHint hint );

	void AddNode( CModelNodeData&& nodeData );

	int GetNodeCount() const;
	CElementMesh GetMesh( int nodePos ) const;
	TMaterialConstRef GetMaterial( int nodePos ) const;

private:
	// Vertex data common to all nodes.
	TAttributeBufferOwner vertexAttributes;
	// Nodes of the model.
	CArray<CModelNodeData> nodes;
	// Model size in model space units.
	CVector3<float> modelSize;

	// Copying is prohibited.
	CModel( CModel& ) = delete;
	void operator=( CModel& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <class... Types>
void CModel::SetVertexAttributes( CArrayView<CTuple<Types...>> newValue, TBufferUsageHint hint )
{
	staticAssert( sizeof...( Types ) > 0 );
	vertexAttributes.CreateBuffer( static_cast<CArrayView<BYTE>>( newValue ), hint );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

