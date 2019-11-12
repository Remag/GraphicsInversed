#include <common.h>
#pragma hdrstop

#ifndef GIN_NO_AUDIO

#include <OggFile.h>
#include <AudioSequence.h>

namespace Gin {

namespace Audio {

const CUnicodeView COggException::generalOggError = L"OGG parsing error: %1.\nFile name: %0.";
//////////////////////////////////////////////////////////////////////////


COggFile::COggFile()
{
	vorbisHandle = CreateOwner<OggVorbis_File>();
}

COggFile::COggFile( CUnicodeView fileName ) :
	COggFile()
{
	Open( fileName );
}

COggFile::~COggFile()
{
	if( IsOpen() ) {
		ov_clear( vorbisHandle.Ptr() );
	}
}

void COggFile::Open( CUnicodeView fileName )
{
	assert( !IsOpen() );
	oggFile.Open( fileName, FRWM_Read, FCM_OpenExisting, FSM_DenyNone );

	try {
		createVorbisHandle();
		fillAndVerifyVorbisInfo();
	} catch( const CException& ) {
		oggFile.Close();
		throw;
	}
}

static const int defaultChunkSize = 64 * 1024;
CSoundOwner COggFile::ReadAudioBuffer( int approximateChunkSize )
{
	assert( IsOpen() );

	const int byteRate = vorbisInfo->channels * vorbisInfo->rate * 2;
	const int chunkSize = CeilTo( approximateChunkSize, byteRate );

	CArray<BYTE> buffer;
	for( ;; ) {
		const int prevSize = buffer.Size();
		buffer.IncreaseSizeNoInitialize( prevSize + defaultChunkSize );
		const int bytesRead = decodeFileData( buffer.Ptr() + prevSize, defaultChunkSize );
		if( bytesRead < defaultChunkSize ) {
			buffer.DeleteLast( defaultChunkSize - bytesRead );
			break;
		}
	}
	return createAudioBuffer( buffer, chunkSize );
}

CSoundOwner COggFile::ReadAudioBuffer()
{
	return ReadAudioBuffer( defaultChunkSize );
	assert( IsOpen() );
}

// Decode new data and but it in buffer.
int COggFile::decodeFileData( BYTE* buffer, int size )
{
	int bitstreamIndex;
	int bytesRead = 0;
	for( ;; ) {
		const int currentBytesRead = ov_read( vorbisHandle.Ptr(), reinterpret_cast<char*>( buffer + bytesRead ), size - bytesRead, 0, 2, 1, &bitstreamIndex );
		if( currentBytesRead <= 0 ) {
			return bytesRead;
		}
		bytesRead += currentBytesRead;
	}
	assert( false );
	return 0;
}

CSoundOwner COggFile::createAudioBuffer( const CArray<BYTE>& buffer, int chunkSize ) const
{
	TAudioDataFormat format = getAudioFormat();
	const int chunkCount = Ceil( buffer.Size(), chunkSize );
	CSoundOwner result( chunkCount );
	int bufferPos = 0;
	for( int i = 0; i < chunkCount - 1; i++ ) {
		result.SetData( i, format, vorbisInfo->rate, buffer.Ptr() + bufferPos, chunkSize );
		bufferPos += chunkSize;
	}
	result.SetData( chunkCount - 1, format, vorbisInfo->rate, buffer.Ptr() + bufferPos, buffer.Size() - bufferPos );
	return move( result );
}

TAudioDataFormat COggFile::getAudioFormat() const
{
	assert( vorbisInfo != 0 && vorbisInfo->channels <= 2 );
	return vorbisInfo->channels == 1 ? ADF_Mono16 : ADF_Stereo16;
}

static const CUnicodeView unsupportedFeatureError = L"OGG file uses unsupported features";
void COggFile::fillAndVerifyVorbisInfo()
{
	vorbisInfo = ov_info( vorbisHandle.Ptr(), 0 ), sizeof( vorbisInfo );

	if( doesVorbisInfoVary() || vorbisInfo->channels > 2 ) {
		ov_clear( vorbisHandle.Ptr() );
		throw COggException( oggFile.GetFileName(), unsupportedFeatureError );
	}
}

// Check if the Vorbis file contain logical bit streams with varying sample rate or number of channels.
bool COggFile::doesVorbisInfoVary()
{
	const int streamCount = ov_streams( vorbisHandle.Ptr() );
	for( int i = 1; i < streamCount; i++ ) {
		const vorbis_info* streamInfo = ov_info( vorbisHandle.Ptr(), i );
		if( streamInfo->channels != vorbisInfo->channels || streamInfo->rate != vorbisInfo->rate ) {
			return true;
		}
	}
	return false;
}

void COggFile::createVorbisHandle()
{
	ov_callbacks relibCallbacks;
	relibCallbacks.close_func = relibCloseFunction;
	relibCallbacks.read_func = relibReadFunction;
	relibCallbacks.seek_func = relibSeekFunction;
	relibCallbacks.tell_func = relibTellFunction;

	checkVorbisError( ov_open_callbacks( &oggFile, vorbisHandle.Ptr(), 0, 0, relibCallbacks ) );
}

size_t COggFile::relibReadFunction( void* ptr, size_t size, size_t nmemb, void* datasource )
{
	CDynamicFile* filePtr = static_cast<CDynamicFile*>( datasource );
	const int byteSize = size * nmemb;

	return filePtr->Read( ptr, byteSize );
}

int COggFile::relibSeekFunction( void* datasource, __int64 offset, int whence )
{
	CDynamicFile* filePtr = static_cast<CDynamicFile*>( datasource );
	
	const auto pos = ( whence == SEEK_CUR ) ? FSP_Current
		: ( whence == SEEK_END ) ? FSP_End
		: FSP_Begin;
		
	filePtr->Seek( offset, pos );
	return 0;
}

int COggFile::relibCloseFunction( void* )
{
	return 0;
}

long COggFile::relibTellFunction( void* datasource )
{
	CDynamicFile* filePtr = static_cast<CDynamicFile*>( datasource );
	return numeric_cast<long>( filePtr->GetPosition() );
}

static const CUnicodeView notVorbisError = L"File doesn't contain valid Vorbis library data";
static const CUnicodeView versionMismatchError = L"Vorbis library version mismatch";
static const CUnicodeView badHeaderError = L"Invalid Vorbis library file header";
static const CUnicodeView logicFaultError = L"Internal logic fault in Vorbis library";
static const CUnicodeView unknownError = L"Unknown Vorbis library error";
void COggFile::checkVorbisError( int errorCode ) const
{
	if( errorCode >= 0 ) {
		return;
	}
	CUnicodeString errorStr;
	switch( errorCode ) {
	case OV_EREAD:
		// Should not happen, CFile throws on itself.
		assert( false );
		break;
	case OV_ENOTVORBIS:
		errorStr = notVorbisError;
		break;
	case OV_EVERSION:
		errorStr = versionMismatchError;
		break;
	case OV_EBADHEADER:
		errorStr = badHeaderError;
		break;
	case OV_EFAULT:
		errorStr = logicFaultError;
		break;
	default:
		errorStr = unknownError;
		break;
	}
	throw COggException( oggFile.GetFileName(), move( errorStr ) );
}

//////////////////////////////////////////////////////////////////////////

} // namespace Audio.

}	// namespace Gin.

#endif

