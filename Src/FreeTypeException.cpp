#include <common.h>
#pragma hdrstop

#include <FreeTypeException.h>

#include <FreeType\ft2build.h>
#include FT_FREETYPE_H

namespace Gin {

extern const CStringView GeneralFreeTypeError;
//////////////////////////////////////////////////////////////////////////

CFreeTypeException::CFreeTypeException( int code )
	: errorBase( GeneralFreeTypeError )
{
	setInfoFromCode( code );
}

CFreeTypeException::CFreeTypeException( const CFreeTypeException& other )
	: errorBase( copy( other.errorBase ) ),
	  moduleName( copy( other.moduleName ) ),
	  errorCode( other.errorCode )
{
}

// Code contains information about the error and the module that threw an error.
// This method gets this information from the code.
void CFreeTypeException::setInfoFromCode( int code )
{
	errorCode = FT_ERROR_BASE( code );
	const int moduleCode = FT_ERROR_MODULE( code );
	moduleName = getModuleName( moduleCode );
}

CStringView CFreeTypeException::getModuleName( int moduleCode )
{
#ifdef FT_CONFIG_OPTION_USE_MODULE_ERRORS
	switch( moduleCode ) {
		case FT_Mod_Err_Base:
			return "Base";
		case FT_Mod_Err_Autofit:
			return "Autofit";
		case FT_Mod_Err_BDF:
			return "BDF";
		case FT_Mod_Err_Bzip2:
			return "Bzip2";
		case FT_Mod_Err_Cache:
			return "Cache";
		case FT_Mod_Err_CFF:
			return "CFF";
		case FT_Mod_Err_CID:
			return "CID";
		case FT_Mod_Err_LZW:
			return "LZW";
		case FT_Mod_Err_OTvalid:
			return "OTvalid";
		case FT_Mod_Err_PCF:
			return "PCF";
		case FT_Mod_Err_PFR:
			return "PFR";
		case FT_Mod_Err_PSaux:
			return "PSaux";
		case FT_Mod_Err_PShinter:
			return "PShinter";
		case FT_Mod_Err_PSnames:
			return "PSnames";
		case FT_Mod_Err_Raster:
			return "Raster";
		case FT_Mod_Err_SFNT:
			return "SFNT";
		case FT_Mod_Err_Smooth:
			return "Smooth";
		case FT_Mod_Err_TrueType:
			return "TrueType";
		case FT_Mod_Err_Type1:
			return "Type1";
		case FT_Mod_Err_Type42:
			return "Type42";
		case FT_Mod_Err_Winfonts:
			return "Winfonts";
		case FT_Mod_Err_GXvalid:
			return "GXvalid";
		default:
			return "Unknown";
	}
#else
	moduleCode;
	return "Unknown";
#endif
}

CString CFreeTypeException::GetMessageText() const
{
	return errorBase.SubstParam( UnicodeStr( errorCode ), moduleName );
}

//////////////////////////////////////////////////////////////////////////

}	 // namespace Gin.