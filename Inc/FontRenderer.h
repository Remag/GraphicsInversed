#pragma once
#include <Gindefs.h>
#include <Uniform.h>
#include <ShaderProgram.h>
#include <TextureOwner.h>
#include <TextureWrappers.h>
#include <Mesh.h>
#include <SamplerObject.h>
#include <PixelRect.h>

namespace Gin {

class CFontRenderer;
class CPixelVector;
//////////////////////////////////////////////////////////////////////////

// Mesh containing text position information. This object is returned by CFontRenderer rendering methods.
class GINAPI CTextMesh {
public:
	CTextMesh();

	bool IsEmpty() const
		{ return vertexCount == 0; }

	const CFontRenderer* GetFontRenderer() const
		{ return owner; }

	// Size of the text mesh bound rectangle.
	CPixelRect BoundRect() const
		{ return boundRect; }

	// Draw the text mesh using the font renderer that created it.
	void DrawExact( const CMatrix3<float>& modelToClip, float zOrder, CColor textColor = CColor( 255, 255, 255, 255 ) ) const;
	// Draw the text mesh using the font renderer that created it.
	// The mesh is aligned to pixel boundaries if possible.
	void DrawAligned( const CMatrix3<float>& modelToPixel, const CMatrix3<float>& pixelToClip, float zOrder, CColor textColor = CColor( 255, 255, 255, 255 ) ) const;

	// Only the renderer can create and use text meshes.
	friend class CFontRenderer;

private:
	// Unicode text representation.
	CGlBufferOwner<BT_Array, CVector4<float>> meshData;
	// Mesh bounding rectangle.
	CPixelRect boundRect;
	CMeshOwner<CArrayMesh> mesh;
	// Font renderer that created the mesh.
	const CFontRenderer* owner = nullptr;
	int vertexCount = 0;

	// Create an empty text mesh.
	explicit CTextMesh( const CFontRenderer& owner );
	// Move the created mesh into text mesh.
	CTextMesh( CGlBufferOwner<BT_Array, CVector4<float>>&& dataSource, CMeshOwner<CArrayMesh>&& source, CPixelRect boundRect, const CFontRenderer& owner, int vertexCount );
};

//////////////////////////////////////////////////////////////////////////

struct CParagraphRenderResult {
	CTextMesh Mesh;
	CVector2<int> EndOffset;

	explicit CParagraphRenderResult( CTextMesh mesh, CVector2<int> endOffset ) : Mesh( move( mesh ) ), EndOffset( endOffset ) {}
};

//////////////////////////////////////////////////////////////////////////

// Class for rendering unicode characters.
class GINAPI CFontRenderer {
public:
	CFontRenderer();
	explicit CFontRenderer( CPtrOwner<IGlyphProvider> glyphProvider );

	// Initialize the font shader. Called automatically during application initialization.
	static void InitializeShaderData();
	// Delete the font shader. It will be recompiled on demand. Useful in case of a context switch.
	static void ClearShaderData();

	bool IsFontLoaded() const
		{ return glyphProvider != nullptr; }
	void SetGlyphProvider( CPtrOwner<IGlyphProvider> newValue );
	// Unload all characters, delete the texture and invalidate all rendered text meshes.
	void UnloadFont();

	// Populate the texture with basic ASCII symbols.
	void LoadBasicCharSet() const;
	// Populate the texture with symbols from the given set or string.
	void LoadCharSet( CUnicodePart str ) const;
	// Access cached glyph data for a given UTF32 character.
	// If the character has not been rendered, it is added to the texture.
	CGlyphSizeData GetGlyphData( unsigned symbolUTF ) const;

	// Shader program used to draw the font.
	static CShaderProgram Shader();

	// Render a single unicode line and return a text mesh containing necessary rendering information.
	CTextMesh RenderLine( CUnicodePart str ) const;
	// Render a single UTF8 line and return a text mesh containing necessary rendering information..
	CTextMesh RenderLine( CStringPart str ) const;
	// Separate the given string into lines with a maximum width and render each line separately.
	void RenderMultipleLines( CUnicodePart str, int lineWidth, int startHOffset, CArray<CTextMesh>& lines ) const;
	CParagraphRenderResult RenderMultipleLines( CUnicodePart str, int lineWidth, int lineHeight, int startHOffset ) const;
	void RenderMultipleLines( CStringPart str, int lineWidth, int startHOffset, CArray<CTextMesh>& lines ) const;
	CParagraphRenderResult RenderMultipleLines( CStringPart str, int lineWidth, int lineHeight, int startHOffset ) const;
	// Draw the rendered string with the given pixel space position.
	void DisplayText( const CTextMesh& textMesh, const CMatrix3<float>& modelToClip, float zOrder, CColor color ) const;

private:
	// Position and size information of the atlas with glyphs.
	struct CGlyphAtlasState {
		CVector2<int> Size;
		CVector2<int> Offset;
		int LineHeight = 0;
	};

	// Data that is necessary for rendering from the atlas.
	struct CRenderGlyphData {
		// Glyph metrics.
		CGlyphSizeData GlyphData;
		// Offset in the atlas in pixels.
		CVector2<int> GlyphOffset;

		CRenderGlyphData() : GlyphOffset( NotFound, NotFound ) {}
		CRenderGlyphData( CGlyphSizeData data, CVector2<int> offset ) : GlyphData( data ), GlyphOffset( offset ) {}
	};

	struct CFontShaderData {
		// Default shader program that is used for rendering.
		CShaderProgramOwner FontProgram;
		// Unicode string containing all the basic ASCII characters.
		// Uniforms for default program.
		// Font color.
		CUniform<CColor> FontColorUniform;
		// Atlas size.
		CUniform<CVector2<float>> AtlasSizeUniform;
		// Glyph atlas.
		CUniform<CTexture<TBT_Texture2, TGF_Red>> FontUniform;
		// Transformation from pixel space to clip space.
		CUniform<CMatrix3<float>> ModelToClipUniform;
		// Text Z order.
		CUniform<float> ZOrderUniform;

		CFontShaderData();
		static CShaderProgramOwner createDefaultProgram();
	};

	// Font used by the renderer.
	CPtrOwner<IGlyphProvider> glyphProvider;

	// Map containing connections between symbol codes and their offset in the texture atlas.
	mutable CMap<unsigned, CRenderGlyphData> fontData;
	// Texture atlas with glyph bitmaps.
	mutable CTextureOwner<TBT_Texture2, TGF_Red> fontTexture;
	mutable CGlyphAtlasState fontTextureState;

	// Offset of the first printable character.
	static const int asciiSymbolOffset = 32;
	// Total number of printable ASCII characters.
	static const int asciiSymbolCount = 128 - asciiSymbolOffset;

	static CPtrOwner<CFontShaderData> shaderData;
	static CUnicodeString asciiCharsStr;

	unsigned parseUtf16Character( CUnicodePart str, int& index ) const;
	unsigned parseUtf8Character( CStringPart str, int& index ) const;
	void fillTextureBuffer( CTextureOwner<TBT_Texture2, TGF_Red>& target, CVector2<int> textureOffset, const BYTE* bitmap, CVector2<int> bitmapSize ) const;
	CGlyphAtlasState fitGlyphIntoAtlas( CGlyphAtlasState state, CVector2<int> glyphSize ) const;
	void setGlyphAtlasState( CGlyphAtlasState newState ) const;
	void padGlyphAtlas( CVector2<int> oldSize, CVector2<int> newSize ) const;

	int calculateWhitespaceHAdvance( CUnicodePart str, int& strPos ) const;
	int calculateWhitespaceHAdvance( CStringPart str, int& strPos ) const;
	void addLineMesh( int lineStartPos, int strPos, CPixelRect lineRect, CArrayView<CVector4<float>> lineBuffer, CArray<CTextMesh>& lines ) const;
	CPixelRect renderUtf8Word( CStringPart str, CVector2<int>& symbolPos, int maxWidth, int& strPos, CArray<CVector4<float>>& wordBuffer ) const;
	CPixelRect renderUtf16Word( CUnicodePart str, CVector2<int>& symbolPos, int maxWidth, int& strPos, CArray<CVector4<float>>& wordBuffer ) const;
	bool tryAddWordCharacter( unsigned glyphCode, int maxWidth, CVector2<int>& symbolPos, CPixelRect& wordRect, CArray<CVector4<float>>& wordBuffer ) const;
	static CPixelRect startNewLine( float lineOffset, CPixelRect wordRect, CArrayBuffer<CVector4<float>> lineWordBuffer );
	static CPixelRect startNewLine( CPixelVector lineOffset, CPixelRect wordRect, CArrayBuffer<CVector4<float>> lineWordBuffer );

	CTextMesh doRenderTextLine( const void* lineBuffer, int length, void ( CFontRenderer::*renderMethod )( const void*, int, CPixelRect&, int&, CArrayBuffer<CVector4<float>> ) const ) const;
	void renderSingleUtf16Line( const void* strBuffer, int length, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;
	void renderMultipleUtf16Lines( CUnicodePart str, int lineWidth, int lineHeight, CPixelRect& boundRect, int& totalVertexCount, CVector2<int>& lineOffset, 
		CArrayBuffer<CVector4<float>> stringData ) const;
	void renderMultipleUtf8Lines( CStringPart str, int lineWidth, int lineHeight, CPixelRect& boundRect, int& totalVertexCount, CVector2<int>& lineOffset,
		CArrayBuffer<CVector4<float>> stringData ) const;
	void copyWordToBuffer( CPixelRect wordRect, CArray<CVector4<float>>& wordBuffer, int lineWidth, int lineHeight, int prevLineEndPosX, CVector2<int>& linePos,
		CPixelRect& boundRect, int& totalVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;

	void renderSingleUtf8Line( const void* strBuffer, int length, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;
	void renderUtf16Line( CUnicodePart line, CVector2<int>& pos, int dataOffset, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;
	void renderUtf8Line( CStringPart line, CVector2<int>& pos, int dataOffset, CPixelRect& boundRect, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;
	int addNewGlyph( unsigned glyphCode, CPixelRect& boundRect, CVector2<int>& fontPos, int symbolIndex, int& lineVertexCount, CArrayBuffer<CVector4<float>> stringData ) const;
	CPixelRect addQuadToMesh( CVector2<int> pos, CRenderGlyphData charData, CArrayBuffer<CVector4<float>> stringData, int meshOffset ) const;
	void addCharToTexture( unsigned charCode, CRenderGlyphData& result ) const;
	CRenderGlyphData getOrCreateRenderData( unsigned glyphCode ) const;
	static void initAsciiCharString( CUnicodeString& str );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

