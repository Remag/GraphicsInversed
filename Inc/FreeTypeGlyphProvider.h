#pragma once
#include <GinDefs.h>
#include <GlyphProvider.h>
#include <FontSize.h>
#include <Font.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Glyph provider that utilizes FreeType to render requested glyphs.
class GINAPI CFreeTypeGlyphProvider : public IGlyphProvider {
public:
	CFreeTypeGlyphProvider( CFontView font, int fontPxHeight );

	unsigned GetGlyphIndex( int utf32 ) const;
	CPtrOwner<IGlyph> GetGlyphByIndex( unsigned index ) const;

	virtual CPtrOwner<IGlyph> GetGlyph( int utf32 ) const override final;

private:
	CFontView fontView;
	CFontSizeOwner fontSize;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


