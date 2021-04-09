#pragma once
#include <ShaderProgram.h>
#include <Mesh.h>
#include <GlBuffer.h>

namespace Gin {

class CParticleSystem;
class CParticleShaderOwner;
class IParticleEmitter;
struct CParticleVariableData;
//////////////////////////////////////////////////////////////////////////

// Result of particle system render initialization.
// Can be used as a regular array mesh for draw commands.
// Deinitializes particle rendering on destruction.
class GINAPI CParticleMesh : public CArrayMesh {
public:
	~CParticleMesh();

	using CArrayMesh::Draw;
	void Draw( CShaderProgram shader );

	// Particle system can construct object of this class during initialization.
	friend class CParticleSystem;

private:
	const CParticleSystem& particleSystem;

	explicit CParticleMesh( const CParticleSystem& particleSystem );
	CParticleMesh( const CParticleMesh& ) = default;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for emitting and rendering particles using transform feedback.
class GINAPI CParticleSystem {
public:
	// Create a particle system based the given shader and an arbitrary compatible emitter.
	CParticleSystem( IParticleEmitter& emitter, int initParticleCount, CRandomGenerator& rng );

	int GetParticleCount() const
		{ return particleCount; }

	// Change the system's emitter. It is checked to emit the same particles as the original one.
	void SetEmitter( IParticleEmitter& newEmitter );
	// Clear and initialize the particles again.
	void Reset( int particleCount, CRandomGenerator& rng );

	// Perform the drawing step.
	CParticleMesh RenderPartilces() const;

	// Retrieve the latest generation result.
	CDynamicGlBuffer<BT_TransformFeedback> GetFrontBuffer() const
		{ return CDynamicGlBuffer<BT_TransformFeedback>( frontBuffer, particleTypeInfo ); }
	// Retrieve the buffer with the previous generation. This buffer is uninitialized until the first render.
	CDynamicGlBuffer<BT_TransformFeedback> GetBackBuffer() const
		{ return CDynamicGlBuffer<BT_TransformFeedback>( backBuffer, particleTypeInfo ); }
	CArrayMesh GetFrontMesh() const
		{ return frontMesh; }

	// Particle mesh swaps system's buffers on its destruction.
	friend class CParticleMesh;
	
private:
	// Current particle emitter.
	IParticleEmitter* currentEmitter;
	// Double buffered transform feedback data.
	// Buffers need to be swapped after drawing hence mutable.
	mutable CMeshOwner<CArrayMesh> frontMesh;
	mutable CGlBufferOwner<BT_TransformFeedback, BYTE> frontBuffer;
	mutable CMeshOwner<CArrayMesh> backMesh;
	mutable CGlBufferOwner<BT_TransformFeedback, BYTE> backBuffer;
	// List of types used as buffer inputs.
	CStaticArray<TGlType> particleTypeInfo;

	int particleCount;
	// Size of a single particle in bytes.
	int particleSize = 0;

	void swapBuffers() const;

	void initializeMeshes( CArrayView<CParticleVariableData> data );
	static bool isEmitterCompatible( const IParticleEmitter& emitter, CArrayView<CParticleVariableData> data );
	void fillParticleBuffer( CRandomGenerator& rng, CArrayBuffer<BYTE> bufferPtr ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

