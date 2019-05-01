#include <common.h>
#pragma hdrstop

#include <Model.h>
#include <GinComponents.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CModel::AddNode( CModelNodeData&& nodeData )
{
	nodes.Add( move( nodeData ) );
}

int CModel::GetNodeCount() const
{
	return nodes.Size();
}

CElementMesh CModel::GetMesh( int nodePos ) const
{
	return nodes[nodePos].Mesh;
}

TMaterialConstRef CModel::GetMaterial( int nodePos ) const
{
	return nodes[nodePos].Material;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
