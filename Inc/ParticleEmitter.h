#pragma once
#include <Gindefs.h>
#include <DrawEnums.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

struct CParticleVariableData {
	int InLocation;
	CStringView OutName;
	TGlType GlType;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for generating new particles for the particle system.
class IParticleEmitter {
public:
	IParticleEmitter() = default;

	// Get an array of emitted particle variables.
	// The array is expected to have static storage duration.
	virtual CArrayView<CParticleVariableData> GetEmittedParticles() const = 0;
	// Fill the given array with interleaved particle data.
	virtual void FillRawParticleData( CArrayBuffer<BYTE> particleData, CRandomGenerator& rng ) = 0;

private:
	// Copying is prohibited.
	IParticleEmitter( IParticleEmitter& ) = delete;
	void operator=( IParticleEmitter& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

