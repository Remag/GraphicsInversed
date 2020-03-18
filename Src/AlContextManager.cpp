#include <common.h>
#pragma hdrstop

#ifndef GIN_NO_AUDIO

#include <OpenAl\alc.h>
#include <AlContextManager.h>
#include <AudioSequence.h>

namespace Gin {

namespace Audio {

//////////////////////////////////////////////////////////////////////////

void CAlContextManager::Initialize()
{
	// Open the default device.
	device = alcOpenDevice( 0 );
	assert( device != 0 );
	context = alcCreateContext( device, 0 );
	assert( context != 0 );
	const bool makeCurrentResult = alcMakeContextCurrent( context ) != ALC_FALSE;
	makeCurrentResult;
	assert( makeCurrentResult );
	initSources();
}

const int maxSourcesCount = 16;
void CAlContextManager::initSources()
{
	CStackArray<unsigned, maxSourcesCount> srcIds;
	alGenSources( maxSourcesCount, srcIds.Ptr() );
	assert( alGetError() == AL_NO_ERROR );

	activeRecords.ResetBuffer( maxSourcesCount );
	for( auto id : srcIds ) {
		activeRecords.Add( id );
	}
}

CAlContextManager::~CAlContextManager()
{
	if( HasContext() ) {
		Cleanup();
	}
}

void CAlContextManager::Cleanup()
{
	alcMakeContextCurrent( 0 );
	alcDestroyContext( context );
	context = nullptr;
	alcCloseDevice( device );
	device = 0;
}

CAudioRecord CAlContextManager::CreateRecord( CSoundView seq, TSourcePriority priority, CVector3<float> pos, CVector3<float> velocity, bool isLooping )
{
	const auto freeSourcePos = findSourcePos( priority );
	const auto& freeSource = activeRecords[freeSourcePos];
	playSource( freeSource.Id, seq, pos, velocity, isLooping );
	return CAudioRecord( freeSourcePos, freeSource.Generation );
}

int CAlContextManager::findSourcePos( TSourcePriority priority ) 
{
	const auto recordCount = activeRecords.Size();
	for( int i = 0; i < recordCount; i++ ) {
		auto& src = activeRecords[i];
		if( !isActive( src ) ) {
			stopSource( src );
			src.Priority = priority;
			return i;
		}
	}

	// All created sources are playing or paused. At this point we can delete a random low priority sound, it will most likely not be noticeable.
	for( int i = 0; i < recordCount; i++ ) {
		auto& src = activeRecords[i];
		if( src.Priority == SP_LowPriority ) {
			stopSource( src );
			src.Priority = priority;
			return i;
		}
	}

	// All the playing sources are high priority. This is not supposed to happen, just throw.
	assert( false );
	return 0;
}

bool CAlContextManager::isActive( const CPlayingSource& src ) const
{
	int result;
	alGetSourcei( src.Id, AL_SOURCE_STATE, &result );
	return result != CAudioRecord::RS_Initial && result != CAudioRecord::RS_Stopped;	
}

void CAlContextManager::playSource( int srcId, CSoundView seq, CVector3<float> pos, CVector3<float> velocity, bool isLooping ) const
{
	assert( srcId >= 0 );
	alSourcefv( srcId, AEP_Position, pos.Ptr() );
	alSourcefv( srcId, AEP_Velocity, velocity.Ptr() );
	alSourcei( srcId, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE );
	alSourceQueueBuffers( srcId, seq.Buffers().Size(), seq.Buffers().Ptr() );
	alSourcePlay( srcId );
	assert( alGetError() == AL_NO_ERROR );
}

void CAlContextManager::stopSource( CPlayingSource& src )
{
	alSourceStop( src.Id );
	alSourcei( src.Id, AL_BUFFER, 0 );
	src.Generation++;
}

int CAlContextManager::GetAudioRecord( int sourcePos, int sourceGeneration ) const
{
	assert( sourcePos >= 0 && sourcePos < activeRecords.Size() );
	return activeRecords[sourcePos].Generation == sourceGeneration ? activeRecords[sourcePos].Id : NotFound;
}

void CAlContextManager::StopAllRecords()
{
	for( auto& src : activeRecords ) {
		stopSource( src );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif

