#include <common.h>
#pragma hdrstop

#include <GifFile.h>
#include <gifdec.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CGifFile::CGifFile( CUnicodePart _fileName ) :
	fileName( _fileName )
{
}

void CGifFile::Read( CArray<CColor>& result, CVector2<int>& resultSize, CArray<int>& frameEndTimes ) const
{
	assert( result.IsEmpty() );

	CFileReader file( fileName, FCM_OpenExisting );
	CArray<BYTE> fileData;
	const int fileLength = file.GetLength32();
	fileData.IncreaseSize( fileLength );
	file.Read( fileData.Ptr(), fileLength );

	doReadRawData( fileName, fileData, result, resultSize, frameEndTimes );
}

void CGifFile::ReadRawData( CArrayView<BYTE> gifData, CArray<CColor>& result, CVector2<int>& resultSize, CArray<int>& frameEndTimes )
{
	doReadRawData( CUnicodePart(), gifData, result, resultSize, frameEndTimes ); 
}

void CGifFile::doReadRawData( CUnicodePart fileName, CArrayView<BYTE> gifData, CArray<CColor>& result, CVector2<int>& resultSize, CArray<int>& frameEndTimes )
{
	try {
		const GinInternal::CGiffBuffer buffer{ gifData, 0 };
		auto decodeData = GinInternal::gd_open_gif( buffer );
		readGifFrames( decodeData, result, frameEndTimes );
		resultSize.X() = decodeData.width;
		resultSize.Y() = decodeData.height;
	} catch( GinInternal::CGifInternalException& e ) {
		throw CGifException( fileName, e.GetMessageText() );
	}
}

void CGifFile::readGifFrames( GinInternal::CGiffDecodeData& decodeData, CArray<CColor>& result, CArray<int>& frameEndTimes )
{
	const auto width = decodeData.width;
	const auto height = decodeData.height;
	const auto frameArea = width * height;
	CPersistentStorage<CStaticArray<BYTE>, 64> perFrameColorData;
	CArray<BYTE> frameTransparentColors;
	int currentEndTime = 0;
	while( gd_get_frame( &decodeData ) == 1 ) {
		auto& newFrameData = perFrameColorData.Add();
		newFrameData.ResetSize( frameArea * 3 );
		gd_render_frame( &decodeData, newFrameData.Ptr() );
		currentEndTime += decodeData.gce.delay * 10;
		frameEndTimes.Add( currentEndTime );
		if( decodeData.gce.transparency == 1 ) {
			const auto colorPtr = &decodeData.palette->colors[decodeData.gce.tindex * 3];
			frameTransparentColors.Add( *colorPtr );
			frameTransparentColors.Add( *( colorPtr + 1 ) );
			frameTransparentColors.Add( *( colorPtr + 2 ) );
		}
	}

	const auto frameCount = perFrameColorData.Size();
	const auto rowStride = width * frameCount;
	result.IncreaseSize( frameArea * frameCount );
	for( int i = 0; i < frameCount; i++ ) {
		for( int y = 0; y < height; y++ ) {
			for( int x = 0; x < width; x++ ) {
				const auto destPos = rowStride * y + i * width + x;
				const auto srcPos = 3 * ( width * ( height - y - 1 ) + x );
				const auto srcColor = findTransparentColor( perFrameColorData[i], srcPos, frameTransparentColors, i * 3 );
				result[destPos] = srcColor;
			}
		}
	}
}

CColor CGifFile::findTransparentColor( CArrayView<BYTE> frameColors, int framePos, CArrayView<BYTE> transparentColors, int colorPos )
{
	if( transparentColors.IsEmpty() ) {
		return createColor( frameColors, framePos );
	}
	if( frameColors[framePos] == transparentColors[colorPos]
		&& frameColors[framePos + 1] == transparentColors[colorPos + 1]
		&& frameColors[framePos + 2] == transparentColors[colorPos + 2] ) 
	{
		return CColor( 0, 0, 0, 0 );
	}
	return createColor( frameColors, framePos );
}

CColor CGifFile::createColor( CArrayView<BYTE> frameColors, int framePos )
{
	// Fill the color structure as if it was stored in an RBG format.
	// This matches the output of the png file wrapper.
	return CColor( frameColors[framePos + 2], frameColors[framePos + 1], frameColors[framePos] );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Twist.

