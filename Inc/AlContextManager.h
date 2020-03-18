#pragma once

#include <Gindefs.h>
#include <AudioRecord.h>
#include <AudioListener.h>
#include <AudioUtils.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {

//////////////////////////////////////////////////////////////////////////

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
class CSoundView;
//////////////////////////////////////////////////////////////////////////

// Class for managing OpenAL context. A single context at a time is assumed.
class GINAPI CAlContextManager {
public:
	CAlContextManager() : device( 0 ), context( 0 ) {}
	~CAlContextManager();

	// Get context's listener object.
	CAudioListener& GetListener()
		{ return listener; }
	const CAudioListener& GetListener() const
		{ return listener; }

	// Create a new playing audio source. 
	// If the source cannot be created, an existing record is stopped and its id is reused.
	CAudioRecord CreateRecord( CSoundView seq, TSourcePriority priority, CVector3<float> pos, CVector3<float> velocity, bool isLooping );
	int GetAudioRecord( int sourcePos, int sourceGeneration ) const;

	// Stop playing all the sounds and detach the buffers.
	void StopAllRecords();

	// Is the context created.
	bool HasContext() const
		{ return context != nullptr; }
	// Create the context.
	void Initialize();
	// Delete the context.
	void Cleanup();

private:
	// Used audio device.
	ALCdevice* device;
	// Native handle to the audio context.
	ALCcontext* context;
	// Listener object of the context.
	CAudioListener listener;

	struct CPlayingSource {
		// Source identifier in the OpenAl system.
		unsigned Id;
		// Source identifiers can be reused. To distinguish between them, generation number is increased with every reuse.
		int Generation = 0;
		// Source priority. Low priority sources can be reused.
		TSourcePriority Priority = SP_LowPriority;

		explicit CPlayingSource( unsigned id ) : Id( id ) {}
	};

	// List of playing records with an ID.
	CStaticArray<CPlayingSource> activeRecords;

	void initSources();
	int findSourcePos( TSourcePriority priority );
	bool isActive( const CPlayingSource& src ) const;
	void playSource( int srcId, CSoundView seq, CVector3<float> pos, CVector3<float> velocity, bool isLooping ) const;
	void stopSource( CPlayingSource& src );

	// Copying is prohibited.
	CAlContextManager( const CAlContextManager& ) = delete;
	void operator=( const CAlContextManager& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

#endif

}	// namespace Gin.

