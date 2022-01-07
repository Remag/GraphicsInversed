#pragma once
#include <GinDefs.h>
#include <GlyphInc.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Generic interface for rendering a glyph.
class GINAPI IGlyphProvider {
public:
	virtual ~IGlyphProvider() {}

	virtual CPtrOwner<IGlyph> GetGlyph( int utf32 ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


