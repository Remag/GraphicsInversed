#include <common.h>
#pragma hdrstop

#ifndef GIN_NO_AUDIO

#include <WavUtils.h>
#include <WavFile.h>
#include <AudioSequence.h>

namespace Gin {

const CStringView CWavException::generalWavError = "WAV parsing error: %1.\nFile name: %0.";

namespace Audio {

//////////////////////////////////////////////////////////////////////////

CWavFile::CWavFile( CStringPart fileName )
{
	Open( fileName );
}

void CWavFile::Open( CStringPart fileName )
{
	assert( !IsOpen() );
	wavFile.Open( fileName, FRWM_Read, FCM_OpenExisting, FSM_DenyNone );

	try {
		CArray<BYTE> data = readHeaderDataFromFile();
		int dataPos = 0;
		parseRiffHeader( data, riffHeader, dataPos );
		parseFirstWavSubheader( data, wavHeader1, dataPos );
		parseSecondWavSubheader( data, wavHeader2, dataPos );
		wavFile.Seek( dataPos, FSP_Begin );
	} catch( const CException& ) {
		wavFile.Close();
		throw;
	}
}

CSoundOwner CWavFile::ReadAudioBuffer()
{
	assert( IsOpen() );
	return parseAudioData( wavHeader2.Size );
}

CSoundOwner CWavFile::ReadAudioBuffer( int maxBufferSize )
{
	assert( IsOpen() );
	// Audio buffer's size must contain valid amount of samples.
	const int chunkSize = FloorTo( maxBufferSize, wavHeader1.ByteRate );
	return parseAudioData( chunkSize );
}

CArray<BYTE> CWavFile::readHeaderDataFromFile()
{
	const int maxHeaderLength = sizeof( WAV::CRiffHeader ) + sizeof( WAV::CWavSubheader1 ) + sizeof( WAV::CWavSubheader2 ) + 2;
	CArray<BYTE> buffer;
	buffer.IncreaseSizeNoInitialize( maxHeaderLength );
	wavFile.Read( buffer.Ptr(), maxHeaderLength );
	return move( buffer );
}

static const CStringPart wavFileTooSmallError = "WAV file too small";
static const CStringPart invalidWavFileError = "Invalid WAV file";
static const int riffFileId = 0x46464952;	// "RIFF".
static const int wavFileFormat = 0x45564157;	// "WAVE".
void CWavFile::parseRiffHeader( const CArray<BYTE>& data, WAV::CRiffHeader& result, int& pos ) const
{
	const int newPos = pos + sizeof( WAV::CRiffHeader );
	checkWavError( newPos < data.Size(), wavFileTooSmallError );
	memcpy( &result, data.Ptr() + pos, sizeof( WAV::CRiffHeader ) );

	checkWavError( result.Id == riffFileId, invalidWavFileError );
	checkWavError( result.Format == wavFileFormat, invalidWavFileError );

	pos = newPos;
}

static const CStringPart compressionNotSupportedError = "Compressed WAV files are not supported";
static const int wavFormatId = 0x20746d66;	// "fmt ".
void CWavFile::parseFirstWavSubheader( const CArray<BYTE>& data, WAV::CWavSubheader1& result, int& pos ) const
{
	const int newPos = pos + sizeof( WAV::CWavSubheader1 );
	checkWavError( newPos < data.Size(), wavFileTooSmallError );
	memcpy( &result, data.Ptr() + pos, sizeof( WAV::CWavSubheader1 ) );

	checkWavError( result.Id == wavFormatId, invalidWavFileError );
	checkWavError( result.AudioFormat == 1, compressionNotSupportedError );

	// Compressed WAV files can contain additional information in the first subheader.
	// These files are not supported.
	const int realHeaderSize = result.Size + 8;
	const int maxHeaderSize = sizeof( WAV::CWavSubheader1 ) + 2;
	checkWavError( realHeaderSize <= maxHeaderSize, compressionNotSupportedError );

	// Real header size can be bigger than the size of subheader structure.
	pos += realHeaderSize;
}

static const int wavDataId = 0x61746164;	// "data".
void CWavFile::parseSecondWavSubheader( const CArray<BYTE>& data, WAV::CWavSubheader2& result, int& pos ) const
{
	const int newPos = pos + sizeof( WAV::CWavSubheader2 );
	checkWavError( newPos <= data.Size(), wavFileTooSmallError );
	memcpy( &result, data.Ptr() + pos, sizeof( WAV::CWavSubheader2 ) );

	checkWavError( result.Id == wavDataId, invalidWavFileError );
	pos = newPos;
}

CSoundOwner CWavFile::parseAudioData( int chunkSize )
{
	// Create sequence.
	const int chunkCount = wavHeader2.Size / chunkSize;
	assert( chunkCount * chunkSize == wavHeader2.Size );
	CSoundOwner result( chunkCount ); 

	// Read from file.
	CArray<BYTE, CRuntimeHeap> buffer;
	buffer.IncreaseSizeNoInitialize( wavHeader2.Size );
	const int bufferSize = wavFile.Read( buffer.Ptr(), buffer.Size() );
	assert( bufferSize == buffer.Size() );

	const TAudioDataFormat format = getDataFormat( wavHeader1 );

	// Fill sequence.
	int bufferPos = 0;
	for( int i = 0; i < chunkCount; i++ ) {
		result.SetData( i, format, wavHeader1.SampleRate, buffer.Ptr() + bufferPos, chunkSize );
		bufferPos += chunkSize;
	}
	return result;
}

static const CStringView unsupportedFeatureError = "WAV feature unsupported";
TAudioDataFormat CWavFile::getDataFormat( const WAV::CWavSubheader1& header ) const
{
	if( header.NumChannels == 1 ) {
		return getMonoDataFormat( header );
	} else if( header.NumChannels == 2 ) {
		return getStereoDataFormat( header );
	} else {
		checkWavError( false, unsupportedFeatureError );
		return ADF_Stereo8;
	}
}

TAudioDataFormat CWavFile::getMonoDataFormat( const WAV::CWavSubheader1& header ) const
{
	if( header.BitsPerSample == 8 ) {
		return ADF_Mono8;
	} else if( header.BitsPerSample == 16 ) {
		return ADF_Mono16;
	} else {
		checkWavError( false, unsupportedFeatureError );
		return ADF_Stereo8;
	}
}

TAudioDataFormat CWavFile::getStereoDataFormat( const WAV::CWavSubheader1& header ) const
{
	if( header.BitsPerSample == 8 ) {
		return ADF_Stereo8;
	} else if( header.BitsPerSample == 16 ) {
		return ADF_Stereo16;
	} else {
		checkWavError( false, unsupportedFeatureError );
		return ADF_Stereo8;
	}
}

void CWavFile::checkWavError( bool condition, CStringPart errorStr ) const
{
	if( !condition ) {
		throw CWavException( wavFile.GetFileName(), errorStr );
	}
}

//////////////////////////////////////////////////////////////////////////

} // namespace Audio.

}	// namespace Gin.

#endif
