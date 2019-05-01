#include <common.h>
#pragma hdrstop

#include <AudioSequence.h>
#include <AlGlobals.h>
#include <AlContextManager.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {

//////////////////////////////////////////////////////////////////////////

CSoundOwner::CSoundOwner( int bufferCount )
{
	bufferIds.ResetSize( bufferCount );
	assert( GetAudioContextManager().HasContext() );
	alGenBuffers( bufferCount, bufferIds.Ptr() );
	checkAudioError();
}

CSoundOwner::~CSoundOwner()
{
	assert( GetAudioContextManager().HasContext() );
	alDeleteBuffers( bufferIds.Size(), bufferIds.Ptr() );
	checkAudioError();
}

void CSoundOwner::SetData( int bufferPos, TAudioDataFormat format, int frequency, const void* data, int size )
{
	assert( bufferPos >= 0 && bufferPos < bufferIds.Size() );
	assert( frequency > 0 );
	alBufferData( bufferIds[bufferPos], format, data, size, frequency );
	checkAudioError();
}

void CSoundOwner::checkAudioError()
{
	assert( alGetError() == AL_NO_ERROR );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

#endif

}	// namespace Gin.
