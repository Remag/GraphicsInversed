#include <common.h>
#pragma hdrstop

#include <FontRenderer.h>
#include <TextureBinder.h>
#include <Mesh.h>
#include <GinGlobals.h>
#include <GlWindow.h>
#include <GinError.h>
#include <PixelVector.h>
#include <PixelRect.h>
#include <BlendModeSwitcher.h>
#include <BufferMapper.h>
#include <DefaultSamplerContainer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CTextMesh::CTextMesh() :
	meshData( CGlBufferOwner<BT_Array, CVector4<float>>::CreateRawBuffer() ),
	mesh( CMeshOwner<CArrayMesh>::CreateRawMesh() )
{
}

CTextMesh::CTextMesh( const CFontRenderer& _owner ) : 
	meshData( CGlBufferOwner<BT_Array, CVector4<float>>::CreateRawBuffer() ),
	mesh( CMeshOwner<CArrayMesh>::CreateRawMesh() ),
	owner( &_owner )
{
}

CTextMesh::CTextMesh( CGlBufferOwner<BT_Array, CVector4<float>>&& dataSource, CMeshOwner<CArrayMesh>&& source, CPixelRect rect, const CFontRenderer& _owner, int _vertexCount ) : 
	meshData( move( dataSource ) ),
	mesh( move( source ) ),
	boundRect( rect ),
	owner( &_owner ),
	vertexCount( _vertexCount )
{
}

void CTextMesh::DrawExact( const CMatrix3<float>& modelToClip, CColor textColor /*= CColor( 255, 255, 255, 255 ) */ ) const
{
	assert( owner != nullptr );
	owner->DisplayText( *this, modelToClip, textColor );
}

void CTextMesh::DrawAligned( const CMatrix3<float>& modelToPixel, const CMatrix3<float>& pixelToClip, CColor textColor /*= CColor( 255, 255, 255, 255 ) */ ) const
{
	auto gridAlignedTransform = modelToPixel;
	gridAlignedTransform( 2, 0 ) = Round( modelToPixel( 2, 0 ) ) * 1.0f;
	gridAlignedTransform( 2, 1 ) = Round( modelToPixel( 2, 1 ) ) * 1.0f;
	DrawExact( pixelToClip * gridAlignedTransform, textColor );
}

//////////////////////////////////////////////////////////////////////////

CUnicodeString CFontRenderer::asciiCharsStr;
CPtrOwner<CFontRenderer::CFontShaderData> CFontRenderer::shaderData;
//////////////////////////////////////////////////////////////////////////

CFontRenderer::CFontRenderer()
{
	assert( shaderData != nullptr );
	fontTexture.SetSamplerObject( GetLinearSampler() );
}

CFontRenderer::CFontRenderer( CFontView _font, int fontPxHeight ) :
	font( _font ),
	fontSize( _font.CreateSizeObject( fontPxHeight ) ),
	fontPixelHeight( fontPxHeight )
{
}

void CFontRenderer::InitializeShaderData()
{
	shaderData = CreateOwner<CFontShaderData>();
	if( asciiCharsStr.IsEmpty() ) {
		initAsciiCharString( asciiCharsStr );
	}
}

void CFontRenderer::ClearShaderData()
{
	shaderData = nullptr;
}

void CFontRenderer::LoadFont( CFontView newFont, int fontPxHeight )
{
	UnloadFont();
	font = newFont;
	fontSize = font.CreateSizeObject( fontPxHeight );
	fontPixelHeight = fontPxHeight;
}

void CFontRenderer::UnloadFont()
{
	setAtlasDimensions( CVector2<int>{} );
	font = CFontView{};
	fontData.FreeBuffer();
}

void CFontRenderer::LoadBasicCharSet()
{
	LoadCharSet( asciiCharsStr );
}

// Distance between glyphs in the texture.
static const int glyphPadding = 1;
void CFontRenderer::LoadCharSet( CUnicodePart str )
{
	assert( IsFontLoaded() );
	fontData.ReserveBuffer( fontData.Size() + str.Length() );
	// Fill the glyph data.
	CArray<CGlyph> glyphs;
	glyphs.ReserveBuffer( str.Length() );

	const auto atlasOldSize = getIntegerAtlasSize();
	CVector2<int> atlasNewSize = atlasOldSize;
	for( int i = 0; i < str.Length(); i++ ) {
		// Find the glyph's UTF32 code.
		const int glyphCode = parseUtf16Character( str, i );
		if( fontData.Has( glyphCode ) ) {
			// Skip the already loaded values.
			continue;
		}

		// Get the glyph and send the fondData.
		glyphs.Add( font.GetGlyph( glyphCode, fontSize ) );
		const CGlyphData glyphData = glyphs.Last().GetGlyphData();
		fontData.Set( glyphCode, CRenderGlyphData( glyphData, atlasNewSize.X() ) );

		// Remember offsets.
		atlasNewSize.X() += glyphData.Size.X() + glyphPadding;
		atlasNewSize.Y() = max( atlasNewSize.Y(), glyphData.Size.Y() );
	}
	CTextureBinder binder( fontTexture );
	increaseTextureSize( atlasNewSize );

	CArray<BYTE> zeroBuffer;
	// Fill the glyph texture.
	int textureOffset = atlasOldSize.X();
	for( int i = 0; i < glyphs.Size(); i++ ) {
		const int glyphCode = glyphs[i].GetCode();
		const CGlyphData& glyphData = fontData[glyphCode].GlyphData;
		// Negative pitch is not supported. At least until a single font with negative pitch is found.
		assert( glyphData.Pitch >= 0 );
		assert( glyphData.Size.X() == glyphData.Pitch );
		fillTextureBuffer( fontTexture, textureOffset, glyphs[i].GetBitmap(), glyphData.Size, atlasNewSize.Y(), zeroBuffer );
		textureOffset += glyphData.Size.X() + glyphPadding;
	}
	setAtlasDimensions( atlasNewSize );
}

void CFontRenderer::increaseTextureSize( CVector2<int> newSize ) const
{
	if( newSize.X() <= fontTextureSize.X() && newSize.Y() <= fontTextureSize.Y() ) {
		return;
	} else {
		fontTexture.GrowBuffer( fontTextureSize, newSize, TF_Red, TDT_UnsignedByte );
		fontTextureSize = newSize;
	}
}

void CFontRenderer::fillTextureBuffer( CTextureOwner<TBT_Texture2, TGF_Red>& target, int textureOffset, const BYTE* bitmap, CVector2<int> bitmapSize, int maxHeight, CArray<BYTE>& zeroBuffer ) const
{
	const CVector2<int> rightPaddingSize{ glyphPadding, maxHeight };
	const auto rightPaddingArea = rightPaddingSize.X() * rightPaddingSize.Y();
	const CVector2<int> topPaddingSize{ bitmapSize.X(), maxHeight - bitmapSize.Y() };
	const auto topPaddingArea = topPaddingSize.X() * topPaddingSize.Y();
	const auto maxPaddingArea = max( topPaddingArea, rightPaddingArea );
	if( zeroBuffer.Size() < maxPaddingArea ) {
		zeroBuffer.IncreaseSize( maxPaddingArea );
	}

	target.SetSubData( { textureOffset, 0 }, bitmap, bitmapSize, 0, TF_Red, TDT_UnsignedByte );
	target.SetSubData( { textureOffset + bitmapSize.X(), 0 }, zeroBuffer.Ptr(), rightPaddingSize, 0, TF_Red, TDT_UnsignedByte );
	if( topPaddingArea > 0 ) {
		target.SetSubData( { textureOffset, bitmapSize.Y() }, zeroBuffer.Ptr(), topPaddingSize, 0, TF_Red, TDT_UnsignedByte );
	}
}

const CUnicodeView invalidStrError = L"Invalid string passed to renderer: %0.";
// Get the UTF16 character code from str at strPos. A parsed UTF32 character is returned.
int CFontRenderer::parseUtf16Character( CUnicodePart str, int& strPos ) const
{
	assert( strPos < str.Length() );
	int result;
	if( !Unicode::TryConvertUtf16ToUtf32( str[strPos], result ) ) {
		const auto nextPos = strPos + 1;
		if( nextPos == str.Length() ) {
			Log::Warning( invalidStrError.SubstParam( str ), this );
			return str[strPos];
		}
		if( !Unicode::TryConvertUtf16ToUtf32( str[strPos], str[nextPos], result ) ) {
			Log::Warning( invalidStrError.SubstParam( str ), this );
			result = str[nextPos];
			strPos++;
		} else {
			strPos += 2;
		}
	} else {
		strPos++;
	}
	return result;
}

int CFontRenderer::parseUtf8Character( CStringPart str, int& strPos ) const
{
	assert( strPos < str.Length() );
	int result;
	const auto parsedCount = Unicode::TryConvertUtf8ToUtf32( str.begin() + strPos, str.Length() - strPos, result );
	if( parsedCount == 0 ) {
		Log::Warning( invalidStrError.SubstParam( UnicodeStr( str ) ), this );
		strPos++;
		return 0;
	}

	strPos += parsedCount;
	return result;
}

CShaderProgram CFontRenderer::Shader()
{
	return shaderData->FontProgram;
}

const CGlyphData& CFontRenderer::GetGlyphData( int symbolUTF ) const
{
	return getOrCreateRenderData( symbolUTF ).GlyphData;
}

const CFontRenderer::CRenderGlyphData& CFontRenderer::getOrCreateRenderData( int glyphCode ) const
{
	CRenderGlyphData& charData = fontData.GetOrCreate( glyphCode ).Value();
	if( charData.GlyphOffset == NotFound ) {
		addCharToTexture( glyphCode, charData );
	}
	return charData;
}

static const int verticesPerChar = 6;
CTextMesh CFontRenderer::RenderLine( CUnicodePart str ) const
{
	return doRenderTextLine( str.begin(), str.Length(), &CFontRenderer::renderSingleUtf16Line );
}

CTextMesh CFontRenderer::RenderLine( CStringPart str ) const
{ 
	return doRenderTextLine( str.begin(), str.Length(), &CFontRenderer::renderSingleUtf8Line );
}

CTextMesh CFontRenderer::doRenderTextLine( const void* lineBuffer, int length, void ( CFontRenderer::*renderMethod )( const void*, int, CPixelRect&, int&, CArrayBuffer<CVector4<float>> ) const ) const
{
	if( length == 0 ) {
		return CTextMesh( *this );
	}
	// Create mesh data from string.
	CGlBufferOwner<BT_Array, CVector4<float>> stringData;
	stringData.ReserveBuffer( length * verticesPerChar, BUH_StaticDraw );
	CTextureBinder binder( fontTexture );

	CPixelRect boundRect;
	int lineVertexCount = 0;
	CBufferMapper( BWMM_Write, stringData, renderMethod, this, lineBuffer, length, boundRect, lineVertexCount );

	CMeshOwner<CArrayMesh> textMesh( MDM_Triangles );
	textMesh.BindBuffer( stringData, { 0 } );

	return CTextMesh( move( stringData ), move( textMesh ), boundRect, *this, lineVertexCount );
}

void CFontRenderer::renderSingleUtf16Line( const void* strBuffer, int length, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	CUnicodePart str( static_cast<const wchar_t*>( strBuffer ), length );
	CVector2<int> pos;
	renderUtf16Line( str, pos, 0, boundRect, lineVertexCount, stringData );
}

void CFontRenderer::renderSingleUtf8Line( const void* strBuffer, int length, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	CStringPart str( static_cast<const char*>( strBuffer ), length );
	CVector2<int> pos;
	renderUtf8Line( str, pos, 0, boundRect, lineVertexCount, stringData );
}

// Render a single line of text. Return the amount of vertices created.
void CFontRenderer::renderUtf16Line( CUnicodePart line, CVector2<int>& fontPos, int dataOffset, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	const int lineLength = line.Length();
	// The loop variable can skip through surrogate pairs, we need to remember the number of rendered symbols separately.
	int symbolIndex = dataOffset;
	for( int i = 0; i < lineLength; ) {
		// Find the glyph's UTF32 code.
		const int glyphCode = parseUtf16Character( line, i );
		symbolIndex = addNewGlyph( glyphCode, boundRect, fontPos, symbolIndex, lineVertexCount, stringData );
	}
}

void CFontRenderer::renderUtf8Line( CStringPart line, CVector2<int>& fontPos, int dataOffset, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	const int lineLength = line.Length();
	// The loop variable can skip through surrogate pairs, we need to remember the number of rendered symbols separately.
	int symbolIndex = dataOffset;
	for( int i = 0; i < lineLength; ) {
		// Find the glyph's UTF32 code.
		const int glyphCode = parseUtf8Character( line, i );
		symbolIndex = addNewGlyph( glyphCode, boundRect, fontPos, symbolIndex, lineVertexCount, stringData );
	}
}

int CFontRenderer::addNewGlyph( int glyphCode, CPixelRect& boundRect, CVector2<int>& fontPos, int symbolIndex, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	const CRenderGlyphData& charData = getOrCreateRenderData( glyphCode );
	boundRect = GetRectUnion( boundRect, addQuadToMesh( fontPos, charData, stringData, symbolIndex * verticesPerChar ) );

	fontPos += charData.GlyphData.Advance;
	lineVertexCount += verticesPerChar;
	return symbolIndex + 1;
}

// Load the given character to the texture and fill the result glyph data structure.
void CFontRenderer::addCharToTexture( int charCode, CRenderGlyphData& result ) const
{
	assert( font.IsLoaded() );

	CGlyph charGlyph = font.GetGlyph( charCode, fontSize );
	const CGlyphData glyphData = charGlyph.GetGlyphData();
	result.GlyphData = glyphData;
	const auto atlasIntegerSize = getIntegerAtlasSize();
	result.GlyphOffset = atlasIntegerSize.X();
	
	const int newWidth = atlasIntegerSize.X() + glyphData.Size.X() + glyphPadding;
	const int newHeight = max( atlasIntegerSize.Y(), glyphData.Size.Y() );
	const CVector2<int> newSize( newWidth, newHeight );

	if( fontTextureSize.X() < newWidth ||
		fontTextureSize.Y() < newHeight )
	{
		increaseTextureSize( newSize );
	}
	CArray<BYTE> zeroBuffer;
	fillTextureBuffer( fontTexture, atlasIntegerSize.X(), charGlyph.GetBitmap(), glyphData.Size, newHeight, zeroBuffer );
	setAtlasDimensions( newSize );
	CheckGlError();
}

// Construct a quad from given data and add it to the mesh. Return the bounding rectangle of the quad.
CPixelRect CFontRenderer::addQuadToMesh( CVector2<int> fontPos, const CRenderGlyphData& charRenderData,
	CArrayBuffer<CVector4<float>> stringData, int meshOffset ) const
{
	const CGlyphData& glyphData = charRenderData.GlyphData;
	const float charTextureOffset = charRenderData.GlyphOffset * 1.f;
	const float charTextureWidth = glyphData.Size.X() * 1.f;
	const float charTextureHeight = glyphData.Size.Y() * 1.f;
	
	const CVector2<float> charFontSize = static_cast<CVector2<float>>( glyphData.Size );
	const CVector2<float> charFontPos = static_cast<CVector2<float>>( fontPos + glyphData.Offset );

	// Create 4 vertices for the quad.
	const CVector4<float> topLeft{ charFontPos.X(), charFontPos.Y(), charTextureOffset, 0.f };
	const CVector4<float> bottomLeft{ charFontPos.X(), charFontPos.Y() - charFontSize.Y(), charTextureOffset, charTextureHeight };
	const CVector4<float> bottomRight{ charFontPos.X() + charFontSize.X(), charFontPos.Y() - charFontSize.Y(), charTextureOffset + charTextureWidth, charTextureHeight };
	const CVector4<float> topRight{ charFontPos.X() + charFontSize.X(), charFontPos.Y(), charTextureOffset + charTextureWidth, 0.f };

	stringData[meshOffset] = topLeft;
	stringData[meshOffset + 1] = bottomLeft;
	stringData[meshOffset + 2] = bottomRight;
	stringData[meshOffset + 3] = topLeft;
	stringData[meshOffset + 4] = bottomRight;
	stringData[meshOffset + 5] = topRight;
	// Consider empty glyphs to be 1px tall to not generate empty bound rectangles.
	const auto glyphBoundsHeight = max( 1, glyphData.Size.Y() );
	return CPixelRect{ CPixelVector( bottomLeft.XY() ), glyphData.Advance.X(), glyphBoundsHeight };
}

void CFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, CArray<CTextMesh>& lines ) const
{
	if( str.IsEmpty() ) {
		return;
	}

	CArray<CVector4<float>> tempLineBuffer;
	CTextureBinder binder( fontTexture );
	const int length = str.Length();
	int strPos = 0;
	CPixelRect lineRect;
	int lineStartPos = 0;
	// Render the string word by word.
	while( strPos < length ) {
		const auto hAdvance = calculateWhitespaceHAdvance( str, strPos );
		if( hAdvance == -1 ) {
			addLineMesh( lineStartPos, strPos, lineRect, tempLineBuffer, lines );
			tempLineBuffer.Empty();
			lineRect = CPixelRect();
			lineStartPos = strPos;
			continue;
		}

		const int prevLineEnd = tempLineBuffer.Size();
		const int wordStartPos = strPos;
		CVector2<int> wordOffset( Round( lineRect.Right() ) + hAdvance, 0 );
		const auto wordRect = renderUtf16Word( str, wordOffset, lineWidth, strPos, tempLineBuffer );
		const auto newWidth = wordRect.Right();
		if( newWidth <= lineWidth ) {
			// New word fits, continue rendering the line.
			lineRect = GetRectUnion( lineRect, wordRect );
		} else {
			// New word doesn't fit, add the existing line and start a new one with the rendered word.
			addLineMesh( lineStartPos, strPos, lineRect, tempLineBuffer.Left( prevLineEnd ), lines );
			tempLineBuffer.DeleteAt( 0, prevLineEnd );
			lineRect = startNewLine( lineRect.Right(), wordRect, tempLineBuffer );
			lineStartPos = wordStartPos;
		}
	}

	// Add the last part to the lines.
	addLineMesh( lineStartPos, length, lineRect, tempLineBuffer, lines );
}

void CFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, CArray<CTextMesh>& lines ) const
{
	if( str.IsEmpty() ) {
		return;
	}

	CArray<CVector4<float>> tempLineBuffer;
	CTextureBinder binder( fontTexture );
	const int length = str.Length();
	int strPos = 0;
	CPixelRect lineRect;
	int lineStartPos = 0;
	// Render the string word by word.
	while( strPos < length ) {
		const auto hAdvance = calculateWhitespaceHAdvance( str, strPos );
		if( hAdvance == -1 ) {
			addLineMesh( lineStartPos, strPos, lineRect, tempLineBuffer, lines );
			tempLineBuffer.Empty();
			lineRect = CPixelRect();
			lineStartPos = strPos;
			continue;
		}
		const int prevLineEnd = tempLineBuffer.Size();
		const int wordStartPos = strPos;
		CVector2<int> wordOffset( Round( lineRect.Right() ) + hAdvance, 0 );
		const auto wordRect = renderUtf8Word( str, wordOffset, lineWidth, strPos, tempLineBuffer );
		const auto newWidth = wordRect.Right();
		if( newWidth <= lineWidth ) {
			// New word fits, continue rendering the line.
			lineRect = GetRectUnion( lineRect, wordRect );
		} else {
			// New word doesn't fit, add the existing line and start a new one with the rendered word.
			addLineMesh( lineStartPos, strPos, lineRect, tempLineBuffer.Left( prevLineEnd ), lines );
			tempLineBuffer.DeleteAt( 0, prevLineEnd );
			lineRect = startNewLine( lineRect.Right(), wordRect, tempLineBuffer );
			lineStartPos = wordStartPos;
		}
	}

	// Add the last part to the lines.
	addLineMesh( lineStartPos, length, lineRect, tempLineBuffer, lines );
}

int CFontRenderer::calculateWhitespaceHAdvance( CUnicodePart str, int& strPos ) const
{
	auto result = 0;
	const auto length = str.Length();
	for( int i = strPos; i < length; ) {
		const auto ch = str[i];
		if( ch == L'\n' ) {
			strPos = i + 1;
			return -1;
		} else if( !str.IsCharWhiteSpace( ch ) ) {
			strPos = i;
			return result;
		}
		const int glyphCode = parseUtf16Character( str, i );
		const auto& charData = getOrCreateRenderData( glyphCode );
		result += charData.GlyphData.Advance.X();
	}
	return result;
}

int CFontRenderer::calculateWhitespaceHAdvance( CStringPart str, int& strPos ) const
{
	auto result = 0;
	const auto length = str.Length();
	for( int i = strPos; i < length; ) {
		const auto ch = str[i];
		if( ch == '\n' ) {
			strPos = i + 1;
			return -1;
		} else if( !str.IsCharWhiteSpace( ch ) ) {
			strPos = i;
			return result;
		}
		const int glyphCode = parseUtf8Character( str, i );
		const auto& charData = getOrCreateRenderData( glyphCode );
		result += charData.GlyphData.Advance.X();
	}
	return result;
}

void CFontRenderer::addLineMesh( int lineStartPos, int strPos, CPixelRect lineRect, CArrayView<CVector4<float>> lineBuffer, CArray<CTextMesh>& lines ) const
{
	assert( strPos >= lineStartPos );
	CGlBufferOwner<BT_Array, CVector4<float>> lineData;
	lineData.CreateBuffer( lineBuffer, BUH_StaticDraw );
	CMeshOwner<CArrayMesh> lineMesh( MDM_Triangles );
	lineMesh.BindBuffer( lineData, { 0 } );
	CTextMesh resultMesh{ move( lineData ), move( lineMesh ), lineRect, *this, lineBuffer.Size() };
	lines.Add( move( resultMesh ) );
}

CPixelRect CFontRenderer::renderUtf16Word( CUnicodePart str, CVector2<int>& symbolPos, int maxWidth, int& strPos, CArray<CVector4<float>>& wordBuffer ) const
{
	CPixelRect wordRect;
	const int length = str.Length();
	while( strPos < length ) {
		if( str.IsCharWhiteSpace( str[strPos] ) ) {
			break;
		}
		const int glyphCode = parseUtf16Character( str, strPos );
		if( !tryAddWordCharacter( glyphCode, maxWidth, symbolPos, wordRect, wordBuffer ) ) {
			strPos++;
			break;
		}
	}
	return wordRect;
}

CPixelRect CFontRenderer::renderUtf8Word( CStringPart str, CVector2<int>& symbolPos, int maxWidth, int& strPos, CArray<CVector4<float>>& wordBuffer ) const
{
	CPixelRect wordRect;
	const int length = str.Length();
	while( strPos < length ) {
		if( str.IsCharWhiteSpace( str[strPos] ) ) {
			break;
		}
		const int glyphCode = parseUtf8Character( str, strPos );
		if( !tryAddWordCharacter( glyphCode, maxWidth, symbolPos, wordRect, wordBuffer ) ) {
			strPos++;
			break;
		}
	}
	return wordRect;
}

bool CFontRenderer::tryAddWordCharacter( int glyphCode, int maxWidth, CVector2<int>& symbolPos, CPixelRect& wordRect, CArray<CVector4<float>>& wordBuffer ) const
{
	// Find the glyph's UTF32 code.
	const CRenderGlyphData& charData = getOrCreateRenderData( glyphCode );
	// Add the character to word buffer.
	const int bufferPos = wordBuffer.Size();
	wordBuffer.IncreaseSize( bufferPos + verticesPerChar );
	const auto symbolRect = addQuadToMesh( symbolPos, charData, wordBuffer, bufferPos );
	const auto wordWidth = wordRect.Width();
	if( wordWidth == 0 || symbolRect.Width() + wordWidth <= maxWidth ) {
		// The rendered word currently fits on the line, add the character.
		// The first character is added unconditionally to ensure progress.
		wordRect = GetRectUnion( wordRect, symbolRect );
		symbolPos += charData.GlyphData.Advance;
		return true;
	} else {
		// Rare case, where a single word cannot fit on the line. Delete the last added character and finish word rendering.
		wordBuffer.DeleteLast( verticesPerChar );
		return false;
	}
}

CPixelRect CFontRenderer::startNewLine( float lineOffset, CPixelRect wordRect, CArrayBuffer<CVector4<float>> lineWordBuffer )
{
	wordRect.Left() -= lineOffset;
	wordRect.Right() -= lineOffset;
	for( auto& pos : lineWordBuffer ) {
		pos.X() -= lineOffset;
	}
	return wordRect;
}

CPixelRect CFontRenderer::startNewLine( CPixelVector lineOffset, CPixelRect wordRect, CArrayBuffer<CVector4<float>> lineWordBuffer )
{
	wordRect.OffsetRect( lineOffset );
	for( auto& pos : lineWordBuffer ) {
		pos.X() += lineOffset.X();
		pos.Y() += lineOffset.Y();
	}
	return wordRect;
}

CParagraphRenderResult CFontRenderer::RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int startHOffset ) const
{
	if( str.IsEmpty() ) {
		return CParagraphRenderResult( CTextMesh( *this ), CVector2<int>( startHOffset, 0 ) );
	}
	// Create mesh data from string.
	CGlBufferOwner<BT_Array, CVector4<float>> stringData;
	stringData.ReserveBuffer( str.Length() * verticesPerChar, BUH_StaticDraw );
	CTextureBinder binder( fontTexture );

	CPixelRect boundRect;
	int lineVertexCount = 0;
	CVector2<int> endOffset( startHOffset, 0 );
	CBufferMapper( BWMM_Write, stringData, &CFontRenderer::renderMultipleUtf16Lines, this, str, lineWidth, lineHeight, boundRect, lineVertexCount, endOffset );

	CMeshOwner<CArrayMesh> textMesh( MDM_Triangles );
	textMesh.BindBuffer( stringData, { 0 } );

	return CParagraphRenderResult( CTextMesh( move( stringData ), move( textMesh ), boundRect, *this, lineVertexCount ), endOffset );
}

CParagraphRenderResult CFontRenderer::RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int startHOffset ) const
{
	if( str.IsEmpty() ) {
		return CParagraphRenderResult( CTextMesh( *this ), CVector2<int>( startHOffset, 0 ) );
	}
	// Create mesh data from string.
	CGlBufferOwner<BT_Array, CVector4<float>> stringData;
	stringData.ReserveBuffer( str.Length() * verticesPerChar, BUH_StaticDraw );
	CTextureBinder binder( fontTexture );

	CPixelRect boundRect;
	int lineVertexCount = 0;
	CVector2<int> endOffset( startHOffset, 0 );
	CBufferMapper( BWMM_Write, stringData, &CFontRenderer::renderMultipleUtf8Lines, this, str, lineWidth, lineHeight, boundRect, lineVertexCount, endOffset );

	CMeshOwner<CArrayMesh> textMesh( MDM_Triangles );
	textMesh.BindBuffer( stringData, { 0 } );

	return CParagraphRenderResult( CTextMesh( move( stringData ), move( textMesh ), boundRect, *this, lineVertexCount ), endOffset );
}

void CFontRenderer::renderMultipleUtf16Lines( CUnicodePart str, int lineWidth, int lineHeight, CPixelRect& boundRect,
	int& totalVertexCount, CVector2<int>& lineOffset, CArrayBuffer<CVector4<float>> stringData ) const
{
	CArray<CVector4<float>> tempWordBuffer;
	const int length = str.Length();
	int strPos = 0;
	// Render the string word by word.
	while( strPos < length ) {
		const auto hAdvance = calculateWhitespaceHAdvance( str, strPos );
		if( hAdvance == -1 ) {
			lineOffset.X() = 0;
			lineOffset.Y() -= lineHeight;
			continue;
		}
		lineOffset.X() += hAdvance;
		const auto prevLineEndPosX = lineOffset.X();
		const auto wordRect = renderUtf16Word( str, lineOffset, lineWidth, strPos, tempWordBuffer );
		copyWordToBuffer( wordRect, tempWordBuffer, lineWidth, lineHeight, prevLineEndPosX, lineOffset, boundRect, totalVertexCount, stringData );
	}
}

void CFontRenderer::renderMultipleUtf8Lines( CStringPart str, int lineWidth, int lineHeight, CPixelRect& boundRect,
	int& totalVertexCount, CVector2<int>& lineOffset, CArrayBuffer<CVector4<float>> stringData ) const
{
	CArray<CVector4<float>> tempWordBuffer;
	const int length = str.Length();
	int strPos = 0;
	// Render the string word by word.
	while( strPos < length ) {
		const auto hAdvance = calculateWhitespaceHAdvance( str, strPos );
		if( hAdvance == -1 ) {
			lineOffset.X() = 0;
			lineOffset.Y() -= lineHeight;
			continue;
		}
		lineOffset.X() += hAdvance;
		const auto prevLineEndPosX = lineOffset.X(); 
		const auto wordRect = renderUtf8Word( str, lineOffset, lineWidth, strPos, tempWordBuffer );
		copyWordToBuffer( wordRect, tempWordBuffer, lineWidth, lineHeight, prevLineEndPosX, lineOffset, boundRect, totalVertexCount, stringData );
	}
}

void CFontRenderer::copyWordToBuffer( CPixelRect wordRect, CArray<CVector4<float>>& tempWordBuffer, int lineWidth, int lineHeight, int prevLineEndPosX, CVector2<int>& linePos,
	CPixelRect& boundRect, int& totalVertexCount, CArrayBuffer<CVector4<float>> stringData ) const
{
	const auto wordRight = Round( wordRect.Right() );
	if( wordRight <= lineWidth ) {
		// New word fits, continue rendering the line.
		boundRect = GetRectUnion( boundRect, wordRect );
	} else {
		// New word doesn't fit, move it to a new line.
		const auto newWordRect = startNewLine( CPixelVector( -prevLineEndPosX, -lineHeight ), wordRect, tempWordBuffer );
		boundRect = GetRectUnion( boundRect, newWordRect );
		linePos.X() = Round( newWordRect.Right() );
		linePos.Y() -= lineHeight;
	}

	for( auto wordVertex : tempWordBuffer ) {
		stringData[totalVertexCount] = wordVertex;
		totalVertexCount++;
	}
	tempWordBuffer.Empty();
}

void CFontRenderer::DisplayText( const CTextMesh& textMesh, const CMatrix3<float>& modelToClip, CColor color ) const
{
	assert( textMesh.owner == this );
	assert( CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() == shaderData->FontProgram.GetId() );
	
	if( textMesh.IsEmpty() ) {
		// No text to draw.
		return;
	}

	// Enable alpha blending.
	CBlendModeSwitcher blendSwt( BF_SrcAlpha, BF_OneMinusSrcAlpha );
	
	shaderData->AtlasSizeUniform.Set( atlasSize );
	shaderData->FontColorUniform.Set( color );
	shaderData->FontUniform.Set( fontTexture );
	shaderData->ModelToClipUniform.Set( modelToClip );
	shaderData->ZOrderUniform.Set( textMesh.zOrder );
	// Draw the mesh.
	textMesh.mesh.Draw( shaderData->FontProgram, textMesh.vertexCount );
}

CVector2<int> CFontRenderer::getIntegerAtlasSize() const
{
	return CVector2<int>{ Round( atlasSize.X() ), Round( atlasSize.Y() ) };
}

void CFontRenderer::setAtlasDimensions( CVector2<int> newSize ) const
{
	atlasSize = CVector2<float>{ 1.0f * newSize.X(), 1.0f * newSize.Y() };
}

void CFontRenderer::initAsciiCharString( CUnicodeString& str )
{
	assert( str.IsEmpty() );
	str.ReserveBuffer( asciiSymbolCount );
	for( int i = asciiSymbolOffset; i < asciiSymbolCount + asciiSymbolOffset; i++ ) {
		str += static_cast<wchar_t>( i );
	}
}

//////////////////////////////////////////////////////////////////////////

static const CUnicodeView defaultShaderName = L"Default text rendering shader";

static const CStringView defaultVertexShaderText = "#version 110\n \
attribute vec4 vertexData; \
varying vec2 texCoord;	\
uniform vec2 atlasSize; \
uniform mat3 modelToClip; \
uniform float zOrder; \
void main() {	\
vec3 textModelPos = vec3( vertexData.xy, 1 ); \
vec3 textScreenPos = modelToClip * textModelPos; \
gl_Position = vec4( textScreenPos.xy, zOrder, 1 );	\
texCoord = vertexData.zw / atlasSize;	\
}";

static const CStringView defaultFragmentShaderText = "#version 110\n \
varying vec2 texCoord;	\
uniform sampler2D fontAtlas;	\
uniform vec4 color;	\
void main() {	\
gl_FragColor = vec4( 1, 1, 1, texture2D( fontAtlas, texCoord ).r ) * color;	\
}";

CShaderProgramOwner CFontRenderer::CFontShaderData::createDefaultProgram()
{
	CVertexShader vertexShader;
	vertexShader.CreateFromString( defaultShaderName, defaultVertexShaderText );
	CFragmentShader fragmentShader;
	fragmentShader.CreateFromString( defaultShaderName, defaultFragmentShaderText );

	const CShaderLayoutInfo layoutInfo{ { "vertexData", 0 } };
	return CShaderProgramOwner( vertexShader, fragmentShader, layoutInfo );
}

static const CStringView atlasUniformName = "atlasSize";
static const CStringView colorUniformName = "color";
static const CStringView fontTextureName = "fontAtlas";
static const CStringView modelToCliplName = "modelToClip";
static const CStringView zOrderName = "zOrder";
CFontRenderer::CFontShaderData::CFontShaderData() :
	FontProgram( createDefaultProgram() )
{
	FontColorUniform = FontProgram.GetUniform( colorUniformName );
	AtlasSizeUniform = FontProgram.GetUniform( atlasUniformName );
	FontUniform = FontProgram.GetUniform( fontTextureName );
	ModelToClipUniform = FontProgram.GetUniform( modelToCliplName );
	ZOrderUniform = FontProgram.GetUniform( zOrderName );
	CShaderProgramSwitcher swt{ FontProgram };
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
