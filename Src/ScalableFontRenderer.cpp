#include <common.h>
#pragma hdrstop

#include <ScalableFontRenderer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CScalableFontRenderer::SetFont( CFontEdit newValue )
{
	font = newValue;
	for( auto& rendererData : fontRendererDict ) {
		rendererData.Value().LoadFont( newValue );
	}
}

CTextMesh CScalableFontRenderer::RenderLine( CUnicodePart str, int fontPtSize ) const
{
	return getFontRenderer( fontPtSize ).RenderLine( str );
}

CTextMesh CScalableFontRenderer::RenderLine( CStringPart str, int fontPtSize ) const
{
	return getFontRenderer( fontPtSize ).RenderLine( str );
}

void CScalableFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, int fontPtSize, CArray<CTextMesh>& lines ) const
{
	getFontRenderer( fontPtSize ).RenderMultipleLines( str, lineWidth, lines );
}

CTextMesh CScalableFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int fontPtSize ) const
{
	return getFontRenderer( fontPtSize ).RenderMultipleLines( str, lineWidth, lineHeight );
}

void CScalableFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, int fontPtSize, CArray<CTextMesh>& lines ) const
{
	getFontRenderer( fontPtSize ).RenderMultipleLines( str, lineWidth, lines );
}

CTextMesh CScalableFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int fontPtSize ) const
{
	return getFontRenderer( fontPtSize ).RenderMultipleLines( str, lineWidth, lineHeight );
}

const CFontRenderer& CScalableFontRenderer::getFontRenderer( int fontSize ) const
{
	return fontRendererDict.GetOrCreate( fontSize, font ).Value();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

