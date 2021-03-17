#pragma once

#ifndef GIN_NO_AUDIO

#include <Gindefs.h>

namespace Gin {

namespace Audio {

// Data format for the audio.
enum TAudioDataFormat {
	ADF_Mono8 = 0x1100,	// AL_FORMAT_MONO8
	ADF_Mono16 = 0x1101,	// AL_FORMAT_MONO16
	ADF_Stereo8 = 0x1102,	// AL_FORMAT_STEREO8
	ADF_Stereo16 = 0x1103	// AL_FORMAT_STEREO16
};

//////////////////////////////////////////////////////////////////////////

// A non owning sound sequence.
class GINAPI CSoundView {
public:
	CSoundView() = default;
	explicit CSoundView( CArrayView<unsigned> _bufferIds ) : bufferIds( _bufferIds ) {}

	bool IsEmpty() const
		{ return bufferIds.IsEmpty(); }

	// Identifiers of the underlying audio buffers.
	CArrayView<unsigned> Buffers() const
		{ return bufferIds; }

	bool operator==( CSoundView other ) const
		{ return bufferIds.Ptr() == other.bufferIds.Ptr(); }

private:
	CArrayView<unsigned> bufferIds;
};

//////////////////////////////////////////////////////////////////////////

// A sequence of audio buffers representing a single audio sample.
class GINAPI CSoundOwner {
public:
	CSoundOwner() = default;
	explicit CSoundOwner( int bufferCount );
	CSoundOwner( CSoundOwner&& ) = default;
	CSoundOwner& operator=( CSoundOwner&& ) = default;
	~CSoundOwner();

	operator CSoundView() const
		{ return CSoundView( bufferIds ); }

	bool IsEmpty() const
		{ return bufferIds.IsEmpty(); }

	// Identifiers of the underlying audio buffers.
	CArrayView<unsigned> Buffers() const
		{ return bufferIds; }

	// Set buffers data with the given format and frequency. bufferPos denotes index in the bufferIds array.
	void SetData( int bufferPos, TAudioDataFormat format, int frequency, const void* data, int size );
	
private:
	CStaticArray<unsigned> bufferIds;

	void checkAudioError();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif

