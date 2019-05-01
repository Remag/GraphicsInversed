#include <common.h>
#pragma hdrstop

#include <Clipboard.h>

#include <GinGlobals.h>
#include <GlWindow.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Utility class that closes the clipboard on destruction.
class CClipboard {
public:
	CClipboard() = default;
	~CClipboard()
		{ ::CloseClipboard(); }
};

//////////////////////////////////////////////////////////////////////////

bool Clipboard::CopyText( CUnicodeView text )
{
	const auto windowHandle = GetMainWindow().Handle();
	// Open the clipboard an set its owner to the main gl window.
	if( !::OpenClipboard( windowHandle ) ) {
		return false;
	}
	CClipboard clipboard;

	::EmptyClipboard();

	const int textFullLength = text.Length() + 1;
	const int textByteLength = textFullLength * sizeof( wchar_t );
	const auto globalMemHandle = ::GlobalAlloc( GMEM_MOVEABLE, textByteLength );
	if( globalMemHandle == NULL ) {
		return false;
	}

	// Copy the data to a global memory block.
	const auto globalPtr = ::GlobalLock( globalMemHandle );
	if( globalPtr == NULL ) {
		::GlobalUnlock( globalMemHandle );
		::GlobalFree( globalMemHandle );
		return false;
	}

	memcpy( globalPtr, text.Ptr(), textByteLength );
	::GlobalUnlock( globalMemHandle );

	// Set the clipboard data and close the clipboard.
	const auto result = ::SetClipboardData( CF_UNICODETEXT, globalMemHandle );
	return result != NULL;
}

CUnicodeString Clipboard::PasteText()
{
	if( !::IsClipboardFormatAvailable( CF_UNICODETEXT ) ) {
		return CUnicodeString();
	}

	const auto windowHandle = GetMainWindow().Handle();

	// Open the clipboard.
	if( !::OpenClipboard( windowHandle ) ) {
		return CUnicodeString();
	}
	CClipboard clipboard;

	const auto globalMemHandle = ::GetClipboardData( CF_UNICODETEXT );

	if( globalMemHandle == NULL ) {
		return CUnicodeString();
	}

	const auto strPtr = ::GlobalLock( globalMemHandle );
	if( strPtr == NULL ) {
		return CUnicodeString();
	}

	CUnicodeString result;
	const int blockSize = ::GlobalSize( globalMemHandle );
	const auto str = static_cast<const wchar_t*>( strPtr );
	const int blockLength = blockSize / 2;
	for( int i = 0; i < blockLength; i++ ) {
		const auto ch = str[i];
		if( ch == 0 ) {
			break;
		}
		result += ch;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
