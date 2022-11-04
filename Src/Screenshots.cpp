#include <common.h>
#pragma hdrstop

#include <Screenshots.h>
#include <GinGlobals.h>
#include <MainFrame.h>
#include <RenderMechanism.h>
#include <GlWindowUtils.h>
#include <DrawEnums.h>
#include <PngFile.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

static int screenshotIndex = 0;
static CString screenshotFolder( "Screenshots" );
void SetScreenshotFolder( CString newFolder )
{
	assert( FileSystem::IsNameValid( newFolder ) );
	screenshotFolder = move( newFolder );
	screenshotIndex = 0;
}


static const CStringView screenshotNamePrefix = "Screenshot";
static const CStringView screenshotExt = ".png";
CString CreateUniqueImageName()
{
	if( !FileSystem::DirAccessible( screenshotFolder ) ) {
		FileSystem::CreateDir( screenshotFolder );
	}

	return FileSystem::CreateUniqueName( screenshotFolder, screenshotNamePrefix, screenshotExt );
}

void MakeScreenshot()
{
	CVector2<int> screenSize;
	// Read an image into buffer.
	const auto data = GinInternal::GetMainFrame().GetRenderer().ReadScreenBuffer( TF_RGB, screenSize );

	// Write buffer to file.
	CPngFile pngFile( CreateUniqueImageName() );
	const int scanLineWidth = -CeilTo( screenSize.X() * 3, sizeof( DWORD ) );
	pngFile.Write( data, PCF_RGB, scanLineWidth, screenSize );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
