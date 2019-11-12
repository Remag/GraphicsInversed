#pragma once

#ifndef GIN_NO_AUDIO

#include <Gindefs.h>

namespace Gin {

namespace Audio {

//////////////////////////////////////////////////////////////////////////

// Representation of a playing audio source. Records can be stopped by the audio manager at any time.
class GINAPI CAudioRecord {
public:
	enum TRecordState {
		RS_Initial = 0x1011,	// AL_INITIAL
		RS_Playing = 0x1012,	// AL_PLAYING
		RS_Paused = 0x1013,	// AL_PAUSED
		RS_Stopped = 0x1014	// AL_STOPPED
	};

	CAudioRecord() = default;
	CAudioRecord( int recordPos, int generation );

	bool IsValid() const
		{ return sourcePos != NotFound; }

	// State of the record.
	TRecordState GetState() const;
	// Check if the record is in the "ready to play" or "playing" state.
	bool IsActive() const;

	// Play the given record. Initially created records are already playing.
	void Play();
	// Stop playing and remove the record. After stopping the record the only way to play it again is from the source.
	void Stop();
	// Pause the record without stopping it.
	void Pause();
	// Rewind the record to its initial state.
	void Rewind();
	
private:
	// Record index in the context manager source array.
	int sourcePos = NotFound;
	// Record generation. Generation and sourcePos are used together to get an actual source index.
	int recordGeneration = NotFound;

	void checkLastAudioError() const;
	int getRecordId() const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif

