#pragma once
#include <DrawEnums.h>
// Common utility functions for textures.

namespace Gin {

namespace TextureUtils {

//////////////////////////////////////////////////////////////////////////

// Get the pixel size in bytes.
int GINAPI FindBytesPerPixel( TTexelDataType texelDataType, TTexelFormat texelFormat );

//////////////////////////////////////////////////////////////////////////

}	// namespace TextureUtils.

}	// namespace Gin.

