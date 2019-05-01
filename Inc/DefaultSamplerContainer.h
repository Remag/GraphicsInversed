#pragma once
#include <SamplerObject.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Owner of default sampler objects.
// The objects can be retrieved with a set of global functions.
class CDefaultSamplerContainer {
public:
	CDefaultSamplerContainer();

	CSamplerObject GetDefaultSampler() const
		{ return defaultSampler; }
	CSamplerObject GetLinearSampler() const
		{ return linearSampler; }

private:
	CSamplerOwner defaultSampler;
	CSamplerOwner linearSampler;
};

//////////////////////////////////////////////////////////////////////////

CSamplerObject GINAPI GetDefaultSampler();
CSamplerObject GINAPI GetLinearSampler();

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

