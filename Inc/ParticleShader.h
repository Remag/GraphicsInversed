#pragma once
#include <Shader.h>
#include <ShaderProgram.h>
#include <ParticleEmitter.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Owner of the shader program that creates particles using transform feedback.
class CParticleShaderOwner : public CShaderProgramOwner {
public:
	// Create a shader using the given particle emitter as a source for particle variables.
	CParticleShaderOwner( CShaderData<CVertexShader> vertexShaders, CShaderData<CFragmentShader> fragmentShaders, CArrayView<CParticleVariableData> _particleData ) : 
		CShaderProgramOwner( move( vertexShaders ), move( fragmentShaders ), _particleData ), particleData( _particleData ) {}

	CArrayView<CParticleVariableData> ParticleData() const
		{ return particleData; }

private:
	CArrayView<CParticleVariableData> particleData;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

