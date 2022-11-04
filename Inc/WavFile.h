#pragma once

#ifndef GIN_NO_AUDIO

#include <Gindefs.h>
#include <WavUtils.h>

namespace Gin {

namespace Audio {

class CSoundOwner;
enum TAudioDataFormat;
//////////////////////////////////////////////////////////////////////////

// WAV audio file.
class GINAPI CWavFile {
public:
	CWavFile() = default;
	explicit CWavFile( CStringPart fileName );

	// Check if the image has been opened.
	bool IsOpen() const
	{ return wavFile.IsOpen(); }
	// Open a given file. Same object cannot be opened twice.
	void Open( CStringPart fileName );

	// Read the audio from file. The sequence is separated into chunks of maxBufferSize.
	CSoundOwner ReadAudioBuffer( int chunkSize );
	// Create an audio buffer from the whole file.
	CSoundOwner ReadAudioBuffer();

private:
	// File with the audio data.
	CDynamicFile wavFile;
	// WAV header structures.
	WAV::CRiffHeader riffHeader;
	WAV::CWavSubheader1 wavHeader1;
	WAV::CWavSubheader2 wavHeader2;

	CArray<BYTE> readHeaderDataFromFile();

	void parseRiffHeader( const CArray<BYTE>& data, WAV::CRiffHeader& result, int& pos ) const;
	void parseFirstWavSubheader( const CArray<BYTE>& data, WAV::CWavSubheader1& result, int& pos ) const; 
	void parseSecondWavSubheader( const CArray<BYTE>& data, WAV::CWavSubheader2& result, int& pos ) const;

	CSoundOwner parseAudioData( int chunkSize );
	TAudioDataFormat getDataFormat( const WAV::CWavSubheader1& header ) const;
	TAudioDataFormat getMonoDataFormat( const WAV::CWavSubheader1& header ) const;
	TAudioDataFormat getStereoDataFormat( const WAV::CWavSubheader1& header ) const;

	void checkWavError( bool condition, CStringPart errorStr ) const;
};

//////////////////////////////////////////////////////////////////////////

} // namespace Audio.

}	// namespace Gin.

#endif

