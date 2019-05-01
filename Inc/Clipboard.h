#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Functions for interacting with windows clipboard buffer for copy and paste operations.
// Gl window must be created for the clipboard to work.
namespace Clipboard {

// Text copy operation. Returns true on success.
bool GINAPI CopyText( CUnicodeView text );

// Text paste operation. Returns an empty string on failure.
CUnicodeString GINAPI PasteText();

}	// namespace Clipboard.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

