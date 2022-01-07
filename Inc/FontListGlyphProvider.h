#pragma once
#include <GinDefs.h>
#include <FreeTypeGlyphProvider.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Provider that tries to get glyphs from a given font list until a valid glyph is found.
class GINAPI CFontListGlyphProvider : public IGlyphProvider {
public:
	void AddFont( CFontView fontView, int fontPxHeight );
	// IGlyphProvider.
	virtual CPtrOwner<IGlyph> GetGlyph( int utf32 ) const override final;

private:
	CArray<CFreeTypeGlyphProvider> fontList;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


