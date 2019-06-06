#pragma once
#include <FontRenderer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mechanism for rendering the same font at different sizes.
class GINAPI CScalableFontRenderer {
public:
	explicit CScalableFontRenderer( CFontEdit _font ) : font( _font ) {}

	void SetFont( CFontEdit newValue );

	// Render a single unicode line and return a text mesh containing necessary rendering information.
	CTextMesh RenderLine( CUnicodePart str, int fontPxSize ) const;
	// Render a single UTF8 line and return a text mesh containing necessary rendering information.
	CTextMesh RenderLine( CStringPart str, int fontPxSize ) const;
	// Separate the given string into lines with a maximum width and render each line separately.
	void RenderMultipleLines( CUnicodePart str, int lineWidth, int fontPxSize, CArray<CTextMesh>& lines ) const;
	CTextMesh RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int fontPxSize ) const;
	void RenderMultipleLines( CStringPart str, int lineWidth, int fontPxSize, CArray<CTextMesh>& lines ) const;
	CTextMesh RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int fontPxSize ) const;

private:
	CFontEdit font;
	// Font renderers for different font sizes.
	mutable CMap<int, CFontRenderer> fontRendererDict;

	const CFontRenderer& getFontRenderer( int fontPxSize ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

