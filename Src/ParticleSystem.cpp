#include <common.h>
#pragma hdrstop

#include <ParticleSystem.h>
#include <ParticleShader.h>
#include <ParticleEmitter.h>
#include <GinTypes.h>
#include <BufferMapper.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CParticleMesh::CParticleMesh( const CParticleSystem& _particleSystem ) :
	CArrayMesh( _particleSystem.GetFrontMesh() ),
	particleSystem( _particleSystem )
{
	gl::BindBufferBase( BT_TransformFeedback, 0, particleSystem.GetBackBuffer().GetId() );
	gl::BeginTransformFeedback( MDM_Points );
}

CParticleMesh::~CParticleMesh()
{
	gl::EndTransformFeedback();
	gl::BindBufferBase( BT_TransformFeedback, 0, 0 );
	particleSystem.swapBuffers();
}

void CParticleMesh::Draw( CShaderProgram shader )
{
	CArrayMesh::Draw( shader, particleSystem.GetParticleCount() );
}

//////////////////////////////////////////////////////////////////////////

CParticleSystem::CParticleSystem( IParticleEmitter& emitter, int initParticleCount, CRandomGenerator& rng ) :
	currentEmitter( &emitter ),
	frontMesh( MDM_Points ),
	backMesh( MDM_Points )
{
	CArrayView<CParticleVariableData> particleData = emitter.GetEmittedParticles();
	for( auto data : particleData ) {
		particleSize += GinTypes::GetGlTypeInformation( data.GlType ).ByteSize;
	}
	initializeMeshes( particleData );
	Reset( initParticleCount, rng );
}

void CParticleSystem::initializeMeshes( CArrayView<CParticleVariableData> data )
{
	int currentOffset = 0;
	particleTypeInfo.ResetBuffer( data.Size() );
	for( auto particle : data ) {
		const auto& typeInfo = GinTypes::GetGlTypeInformation( particle.GlType );
		backMesh.BindRawBuffer( ConvertGlBuffer<BT_Array>( backBuffer ), typeInfo.ElemCount, typeInfo.InnerMostType, particle.InLocation, currentOffset, particleSize );
		frontMesh.BindRawBuffer( ConvertGlBuffer<BT_Array>( frontBuffer ), typeInfo.ElemCount, typeInfo.InnerMostType, particle.InLocation, currentOffset, particleSize );
		particleTypeInfo.Add( particle.GlType );
		currentOffset += typeInfo.ByteSize;
	}
}

bool CParticleSystem::isEmitterCompatible( const IParticleEmitter& emitter, CArrayView<CParticleVariableData> data )
{
	CArrayView<CParticleVariableData> leftData = emitter.GetEmittedParticles();
	if( leftData.Size() != data.Size() ) {
		return false;
	}

	for( int i = 0; i < leftData.Size(); i++ ) {
		if( leftData[i].GlType != data[i].GlType 
			|| leftData[i].InLocation != data[i].InLocation
			|| leftData[i].OutName != data[i].OutName )
		{
			return false;
		}
	}
	return true;
}

void CParticleSystem::SetEmitter( IParticleEmitter& newEmitter )
{
	assert( isEmitterCompatible( newEmitter, currentEmitter->GetEmittedParticles() ) );
	currentEmitter = &newEmitter;
}

void CParticleSystem::Reset( int newParticleCount, CRandomGenerator& rng )
{
	particleCount = newParticleCount;
	const int particlesByteSize = particleCount * particleSize;
	frontBuffer.ReserveBuffer( particlesByteSize, BUH_DynamicCopy );
	backBuffer.ReserveBuffer( particlesByteSize, BUH_DynamicCopy );
	CBufferMapper( BWMM_Write, frontBuffer, &CParticleSystem::fillParticleBuffer, *this, rng );
}

void CParticleSystem::fillParticleBuffer( CRandomGenerator& rng, CArrayBuffer<BYTE> bufferPtr ) const
{
	currentEmitter->FillRawParticleData( bufferPtr, rng );
}

CParticleMesh CParticleSystem::RenderPartilces() const
{
	return CParticleMesh( *this );
}

void CParticleSystem::swapBuffers() const
{
	swap( frontMesh, backMesh );
	swap( frontBuffer, backBuffer );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
