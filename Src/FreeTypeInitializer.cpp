#include <common.h>
#pragma hdrstop

#include <FreeTypeInitializer.h>

#include <Font.h>
#include <FreeTypeException.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CFreeTypeInitializer::CFreeTypeInitializer()
{
	assert( CFontOwner::freeTypeLib == 0 );

	checkFreeTypeError( FT_Init_FreeType( &CFontOwner::freeTypeLib ) );
}

CFreeTypeInitializer::~CFreeTypeInitializer()
{
	assert( CFontOwner::freeTypeLib != 0 );

	checkFreeTypeError( FT_Done_FreeType( CFontOwner::freeTypeLib ) );
	CFontOwner::freeTypeLib = 0;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.