#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Screenshot making functions and utilities.
// Specify the full path to a new screenshot storage folder.
void GINAPI SetScreenshotFolder( CUnicodeString newFolder );
// Create a BMP screenshot file and put it in the screenshot folder.
void GINAPI MakeScreenshot();
// Create a unique .bmp file name in the screenshot folder.
CUnicodeString GINAPI CreateUniqueImageName();

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

