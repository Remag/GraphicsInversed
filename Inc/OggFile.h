#pragma once

#ifndef GIN_NO_AUDIO

#include <Gindefs.h>

namespace Gin {

namespace Audio {

enum TAudioDataFormat;
class CSoundOwner;

typedef struct OggVorbis_File OggVorbis_File;
typedef struct vorbis_info vorbis_info;
//////////////////////////////////////////////////////////////////////////

// Exception thrown by invalid OGG files.
class GINAPI COggException : public CFileWrapperException {
public:
	COggException( CUnicodeView fileName, CUnicodeView additionalInfo ) : CFileWrapperException( fileName , additionalInfo ) {}
	
	virtual CUnicodeString GetMessageTemplate() const override
		{ return UnicodeStr( generalOggError ); }
	
private:
	static const CUnicodeView generalOggError;
};

//////////////////////////////////////////////////////////////////////////

// OGG file wrapper. Uses Vorbis library to decode OGG files.
class GINAPI COggFile {
public:
	COggFile();
	explicit COggFile( CUnicodeView fileName );
	~COggFile();

	// Check if the image has been opened.
	bool IsOpen() const
	{ return oggFile.IsOpen(); }
	// Open a given file. Same object cannot be opened twice.
	void Open( CUnicodeView fileName );

	// Read the audio from file. The sequence is separated into chunks of chunkSize.
	CSoundOwner ReadAudioBuffer( int chunkSize );
	// Create an audio buffer from the whole file.
	CSoundOwner ReadAudioBuffer();

private:
	CDynamicFile oggFile;
	// Vorbis library file handle.
	CPtrOwner<OggVorbis_File> vorbisHandle;
	// Vorbis file information for the first logical bit stream.
	vorbis_info* vorbisInfo;

	void fillAndVerifyVorbisInfo();
	bool doesVorbisInfoVary();
	void createVorbisHandle();
	void checkVorbisError( int errorCode ) const;

	int decodeFileData( BYTE* buffer, int size );
	CSoundOwner createAudioBuffer( const CArray<BYTE>& buffer, int chunkSize ) const;
	TAudioDataFormat getAudioFormat() const;

	static size_t relibReadFunction( void* ptr, size_t size, size_t nmemb, void* datasource) ;
	static int relibSeekFunction( void* datasource, __int64 offset, int whence );
	static int relibCloseFunction( void* datasource );
	static long relibTellFunction( void* datasource );
};

//////////////////////////////////////////////////////////////////////////

} // namespace Audio.

}	// namespace Gin.

#endif
