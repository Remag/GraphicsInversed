#pragma once
#include <GinDefs.h>
// WAV file support classes and functions.

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace WAV {

//////////////////////////////////////////////////////////////////////////

struct CRiffHeader {
	int Id;	// Contains the letters "RIFF" ( 0x46464952 ).
	int Size;	// the size of the rest of the chunk following this number.
	int Format;	// Contains the letters "WAVE" ( 0x45564157 ).
};

struct CWavSubheader1 {
	int Id;	// Contains the letters "fmt " ( 0x20746d66 ).
	int Size;	// 16 for PCM.  This is the size of the rest of the subchunk which follows this number.
	short AudioFormat;	// Data format. PCM format == 1. Other values mean compression.
	short NumChannels;	// Number of channels in the data.
	int SampleRate;	// Data sampling rate.
	int ByteRate;	// SampleRate * NumChannels * BitsPerSample / 8.
	short BlockAlign;	// NumChannels * BitsPerSample / 8. The number of bytes for one sample including all channels.
	short BitsPerSample;	// Number of bits per each sample.
};

struct CWavSubheader2 {
	int Id;	// Contains the letters "data" ( 0x61746164 ).
	int Size;	// NumSamples * NumChannels * BitsPerSample / 8. Number of bytes in the data.
};

//////////////////////////////////////////////////////////////////////////

}	// namespace WAV.

//////////////////////////////////////////////////////////////////////////

// Exception thrown by the WAV file.
class GINAPI CWavException : public CFileWrapperException {
public:
	CWavException( CUnicodePart fileName, CUnicodePart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}
	
	virtual CUnicodeString GetMessageTemplate() const override
		{ return UnicodeStr( generalWavError ); }
	
private:
	static const CUnicodeView generalWavError;
};

//////////////////////////////////////////////////////////////////////////

#endif

}	// namespace Gin.
