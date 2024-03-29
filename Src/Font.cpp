#include <common.h>
#pragma hdrstop

#include <Font.h>
#include <FontSize.h>
#include <Glyph.h>
#include <FreeTypeException.h>

#include <FreeType\ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CFontView::CFontView( const CFontOwner& owner ) :
	CFontView( owner.View() )
{
}

bool CFontView::HasGlyph( unsigned charCode ) const
{
	return GetGlyphIndex( charCode ) != 0;
}

unsigned CFontView::GetGlyphIndex( unsigned charCode ) const
{
	return FT_Get_Char_Index( fontFace, charCode );
}

CGlyph CFontView::GetGlyph( unsigned charCode, CFontSizeView fontSize ) const
{
	FT_Activate_Size( fontSize.GetHandle() );
	checkFreeTypeError( FT_Load_Char( fontFace, charCode, FT_LOAD_DEFAULT ) );
	return CGlyph( fontFace->glyph );
}

CGlyph CFontView::GetGlyphByIndex( unsigned glyphIndex, CFontSizeView fontSize ) const
{
	FT_Activate_Size( fontSize.GetHandle() );
	checkFreeTypeError( FT_Load_Glyph( fontFace, glyphIndex, FT_LOAD_DEFAULT ) );
	return CGlyph( fontFace->glyph );
}

CFontSizeOwner CFontView::CreateSizeObject( int pxSize ) const
{
	return CreateSizeObject( CVector2<int>( pxSize, pxSize ) );
}

CFontSizeOwner CFontView::CreateSizeObject( CVector2<int> pxSize ) const
{
	assert( IsLoaded() );
	FT_Size newSize;
	FT_New_Size( GetFtFace(), &newSize );
	CFontSizeOwner result( newSize );
	FT_Activate_Size( newSize );
	checkFreeTypeError( FT_Set_Pixel_Sizes( GetFtFace(), pxSize.X(), pxSize.Y() ) );
	return result;
}

//////////////////////////////////////////////////////////////////////////

CFontOwner::CFontOwner( CStringView name )
{
	Load( name );
}

CFontOwner::CFontOwner( CFontOwner&& other ) :
	fontData( move( other.fontData ) ),
	view( other.view )
{
	other.detachView();
}

CFontOwner& CFontOwner::operator=( CFontOwner&& other )
{
	if( IsLoaded() ) {
		Unload();
	}

	fontData = move( other.fontData );
	view = move( other.view );

	other.detachView();
	return *this;
}

void CFontOwner::detachView()
{
	view.GetFtFace() = nullptr;
}

CFontOwner::~CFontOwner()
{
	try {
		if( IsLoaded() ) {
			Unload();
		}
	} catch( const CException& e ) {
		Log::Exception( e );
		cleanup();
	}
	assert( !IsLoaded() );
}

// Clear Relib internal structures.
void CFontOwner::cleanup()
{
	fontData.FreeBuffer();
}

void CFontOwner::Load( CStringView name )
{
	assert( !IsLoaded() );

	// Open the file.
	CFileReader fontFile( name, FCM_OpenExisting );

	// Get file contents in the buffer.
	CArray<BYTE> data;
	data.IncreaseSizeNoInitialize( fontFile.GetLength32() );
	fontFile.Read( data.Ptr(), data.Size() );

	// Create a font face from contents.
	checkFreeTypeError( FT_New_Memory_Face( freeTypeLib, data.Ptr(), data.Size(), 0, &view.GetFtFace() ) );

	// No exceptions were thrown, fill Relib structures.
	fontData = move( data );
}

void CFontOwner::Unload()
{
	assert( IsLoaded() );

	checkFreeTypeError( FT_Done_Face( view.GetFtFace() ) );
	cleanup();
	detachView();
}

CFontSizeOwner CFontOwner::CreateSizeObject( int pxSize ) const
{
	return view.CreateSizeObject( pxSize );
}

CFontSizeOwner CFontOwner::CreateSizeObject( CVector2<int> pxSize ) const
{
	return view.CreateSizeObject( pxSize );
}

bool CFontOwner::HasGlyph( unsigned charCode ) const
{
	return view.HasGlyph( charCode );
}

unsigned CFontOwner::GetGlyphIndex( unsigned charCode ) const
{
	return view.GetGlyphIndex( charCode );
}

CGlyph CFontOwner::GetGlyph( unsigned charCode, CFontSizeView fontSize ) const
{
	return view.GetGlyph( charCode, fontSize );
}

CGlyph CFontOwner::GetGlyphByIndex( unsigned glyphIndex, CFontSizeView fontSize ) const
{
	return view.GetGlyphByIndex( glyphIndex, fontSize );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

