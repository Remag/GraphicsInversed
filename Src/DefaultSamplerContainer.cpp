#include <common.h>
#pragma hdrstop

#include <DefaultSamplerContainer.h>
#include <GinGlobals.h>
#include <GlContextManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CDefaultSamplerContainer::CDefaultSamplerContainer()
{
	linearSampler.SetMinFilter( TF_Linear );
	linearSampler.SetTextureWrap( TWD_WrapR, TWM_ClampToEdge );
	linearSampler.SetTextureWrap( TWD_WrapS, TWM_ClampToEdge );
	linearSampler.SetTextureWrap( TWD_WrapT, TWM_ClampToEdge );
}

//////////////////////////////////////////////////////////////////////////

CSamplerObject GetDefaultSampler()
{
	return GetGlContextManager().GetSamplerContainer().GetDefaultSampler();
}

CSamplerObject GetLinearSampler()
{
	return GetGlContextManager().GetSamplerContainer().GetLinearSampler();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
