#include <common.h>
#pragma hdrstop

#include <AudioRecord.h>
#include <Gindefs.h>
#include <AlGlobals.h>
#include <AlContextManager.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {

//////////////////////////////////////////////////////////////////////////

CAudioRecord::CAudioRecord( int _sourcePos, int generation ) :
	sourcePos( _sourcePos ),
	recordGeneration( generation )
{
}

CAudioRecord::TRecordState CAudioRecord::GetState() const
{
	const int recordId = getRecordId();
	if( recordId == NotFound ) {
		return RS_Stopped;
	}
	int result;
	alGetSourcei( recordId, AL_SOURCE_STATE, &result );
	checkLastAudioError();
	return TRecordState( result );
}

void CAudioRecord::checkLastAudioError() const
{
	assert( alGetError() == AL_NO_ERROR );
}

int CAudioRecord::getRecordId() const
{
	return GetAudioContextManager().GetAudioRecord( sourcePos, recordGeneration );
}

bool CAudioRecord::IsActive() const
{
	const TRecordState currentState = GetState();
	return ( currentState == RS_Paused || currentState == RS_Playing );
}

void CAudioRecord::Play()
{
	const int recordId = getRecordId();
	if( recordId != NotFound ) {
		alSourcePlay( recordId );
		checkLastAudioError();
	}
}

void CAudioRecord::Stop()
{
	const int recordId = getRecordId();
	if( recordId != NotFound ) {
		alSourceStop( recordId );
		checkLastAudioError();
	}
}

void CAudioRecord::Pause()
{
	const int recordId = getRecordId();
	if( recordId != NotFound ) {
		alSourcePause( recordId );
		checkLastAudioError();
	}
}

void CAudioRecord::Rewind()
{
	const int recordId = getRecordId();
	if( recordId != NotFound ) {
		alSourceRewind( recordId );
		checkLastAudioError();
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

#endif

}	// namespace Gin.
