#pragma once
#include <Gindefs.h>
#include <GinComponents.h>
#include <UniformFilterOwner.h>
#include <ScreenBuffer.h>
#include <TextureWrappers.h>
#include <GlWindowUtils.h>
#include <ShaderProgram.h>
#include <BlendModeSwitcher.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class containing all the non-temporary data necessary for the forward renderer. This class can be reused between render operations.
class GINAPI CForwardRendererData {
public:
	// Construct the data from a given shader program data.
	// Separate vertex shader is used to construct a simplified shader program for depth filling.
	// The viewport size is not set during construction and must be specified separately before the rendering is performed.
	CForwardRendererData( const CVertexShader& vertex, CShaderProgram program );

	CShaderProgram GetMainShader() const
		{ return mainShader; }
	CShaderProgram GetDepthFillShader() const
		{ return depthFillShader; }

	CUniformFilter<Vertex::ComponentClass> GetDepthFillFilter() const
		{ return depthFillFilter; }

	CUniformFilter<Vertex::ComponentClass> GetVertexFilter() const
		{ return mainFilter; }
	CUniformFilter<LightSource::ComponentClass> GetLightFilter() const
		{ return mainFilter; }
	CUniformFilter<Material::ComponentClass> GetMaterialFilter() const
		{ return mainFilter; }

private:
	CShaderProgramOwner depthFillShader;
	CUniformFilterOwner<Vertex::ComponentClass> depthFillFilter;

	CShaderProgram mainShader;
	TModelUniformFilterOwner mainFilter;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for performing forward rendering with multiple light sources.
// Class constructor initiates and performs the forward rendering routine.
class GINAPI CForwardRenderer {
public:
	// Perform a given render operation on a set of models.
	// All models will be drawn twice: first time to fill the depth buffer, and second time, for an actual rendering.
	// ModelRange must provide range-based constant iteration support.
	// ModelRange element must have two fields: "const CModel& Model" and "TVertexConstRef VertexData".
	template <class ModelRange>
	CForwardRenderer( const ModelRange& models, CArrayView<TLightSourceConstRef> lights, const CDepthScreenBuffer<TGF_RGB>& result );

private:
	void renderModel( const CForwardRendererData& renderData, const CModel& model, TVertexConstRef vertexData, CArrayView<TLightSourceConstRef> lights );
	void renderModelDepth( const CForwardRendererData& renderData, const CModel& model, TVertexConstRef vertexData );
};

//////////////////////////////////////////////////////////////////////////

template <class ModelRange>
CForwardRenderer::CForwardRenderer( const ModelRange& models, CArrayView<TLightSourceConstRef> lights, const CDepthScreenBuffer<TGF_RGB>& result )
{
	assert( !result.GetSize().IsNull() );
	CScreenSwitcher swt( result );
	CBlendModeSwitcher blendSwt( BF_One, BF_One );
	
	{
		// Fill Z buffer.
		CColorMaskSwitcher colorSwt( false );
		for( const auto& model : models ) {
			CShaderProgramSwitcher shaderSwt( model.RenderData.GetDepthFillShader() );
			renderModelDepth( model.RenderData, model.Model, model.VertexData );
		}
	}
	// Perform rendering.
	for( const auto& model : models ) {
		CShaderProgramSwitcher shaderSwt( model.RenderData.GetMainShader() );
		renderModel( model.RenderData, model.Model, model.VertexData, lights );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

