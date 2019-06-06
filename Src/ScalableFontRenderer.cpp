#include <common.h>
#pragma hdrstop

#include <ScalableFontRenderer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

void CScalableFontRenderer::SetFont( CFontEdit newValue )
{
	font = newValue;
	for( auto& rendererData : fontRendererDict ) {
		rendererData.Value().LoadFont( newValue, rendererData.Value().GetPixelHeight() );
	}
}

CTextMesh CScalableFontRenderer::RenderLine( CUnicodePart str, int fontPxSize ) const
{
	return getFontRenderer( fontPxSize ).RenderLine( str );
}

CTextMesh CScalableFontRenderer::RenderLine( CStringPart str, int fontPxSize ) const
{
	return getFontRenderer( fontPxSize ).RenderLine( str );
}

void CScalableFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, int fontPxSize, CArray<CTextMesh>& lines ) const
{
	getFontRenderer( fontPxSize ).RenderMultipleLines( str, lineWidth, lines );
}

CTextMesh CScalableFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int fontPxSize ) const
{
	return getFontRenderer( fontPxSize ).RenderMultipleLines( str, lineWidth, lineHeight );
}

void CScalableFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, int fontPxSize, CArray<CTextMesh>& lines ) const
{
	getFontRenderer( fontPxSize ).RenderMultipleLines( str, lineWidth, lines );
}

CTextMesh CScalableFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int fontPxSize ) const
{
	return getFontRenderer( fontPxSize ).RenderMultipleLines( str, lineWidth, lineHeight );
}

const CFontRenderer& CScalableFontRenderer::getFontRenderer( int fontPxSize ) const
{
	return fontRendererDict.GetOrCreate( fontPxSize, font, fontPxSize ).Value();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

