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
	CTextMesh RenderLine( CUnicodePart str, int fontPtSize ) const;
	// Render a single UTF8 line and return a text mesh containing necessary rendering information.
	CTextMesh RenderLine( CStringPart str, int fontPtSize ) const;
	// Separate the given string into lines with a maximum width and render each line separately.
	void RenderMultipleLines( CUnicodePart str, int lineWidth, int fontPtSize, CArray<CTextMesh>& lines ) const;
	CTextMesh RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int fontPtSize ) const;
	void RenderMultipleLines( CStringPart str, int lineWidth, int fontPtSize, CArray<CTextMesh>& lines ) const;
	CTextMesh RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int fontPtSize ) const;

private:
	CFontEdit font;
	// Font renderers for different font sizes.
	mutable CMap<int, CFontRenderer> fontRendererDict;

	const CFontRenderer& getFontRenderer( int fontSize ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

