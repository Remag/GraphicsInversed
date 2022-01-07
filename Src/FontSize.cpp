#include <common.h>
#pragma hdrstop

#include <FontSize.h>

#include <ft2build.h>
#include FT_SIZES_H

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CFontSizeOwner::CFontSizeOwner( CFontSizeOwner&& other ) :
	sizeHandle( other.sizeHandle )
{
	other.sizeHandle = nullptr;
}

CFontSizeOwner& CFontSizeOwner::operator=( CFontSizeOwner&& other )
{
	swap( sizeHandle, other.sizeHandle );
	return *this;
}

CFontSizeOwner::~CFontSizeOwner()
{
	if( sizeHandle != nullptr ) {
		FT_Done_Size( sizeHandle );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

