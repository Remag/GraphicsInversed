#include <common.h>
#pragma hdrstop

#include <ForwardRenderer.h>
#include <ShaderProgram.h>
#include <TextureBinder.h>
#include <Model.h>
#include <DrawMaskSwitchers.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CForwardRendererData::CForwardRendererData( const CVertexShader& vertex, CShaderProgram program ) :
	mainShader( program ),
	mainFilter( mainShader ),
	depthFillShader( vertex, {} ),
	depthFillFilter( depthFillShader )
{
	// Fill the depth filter.
	for( int i = 0; i < depthFillShader.GetUniformCount(); i++ ) {
		GetOrCreateGlComponent<Vertex::ComponentClass>( i, depthFillFilter );
	}
}

//////////////////////////////////////////////////////////////////////////

void CForwardRenderer::renderModel( const CForwardRendererData& renderData, const CModel& model, TVertexConstRef vertexData, CArrayView<TLightSourceConstRef> lights )
{
	const auto& vertexFilter = renderData.GetVertexFilter();
	const auto shader = vertexFilter.Shader();
	assert( CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() == shader.GetId() );

	vertexFilter.FillUniforms( vertexData );

	for( int i = 0; i < model.GetNodeCount(); i++ ) {
		// Set material uniforms.
		renderData.GetMaterialFilter().FillUniforms( model.GetMaterial( i ) );
		// Render the node with each light source.
		for( const auto& light : lights ) {
			renderData.GetLightFilter().FillUniforms( light );
			model.GetMesh( i ).Draw( shader );
		}
	}
}

void CForwardRenderer::renderModelDepth( const CForwardRendererData& renderData, const CModel& model, TVertexConstRef vertexData )
{
	const auto& depthVertexFilter = renderData.GetDepthFillFilter();
	assert( CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() == depthVertexFilter.Shader().GetId() );

	depthVertexFilter.FillUniforms( vertexData );

	for( int i = 0; i < model.GetNodeCount(); i++ ) {
		model.GetMesh( i ).Draw( depthVertexFilter.Shader() );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
