#include <common.h>
#pragma hdrstop

#include <GinError.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

COpenGLException::COpenGLException( int _errorCode ) :
	errorCode( _errorCode )
{
}

CString COpenGLException::GetMessageText() const
{
	const CStringView openGlMessageTemplate = "Open GL library error! Error code: %0.";
	return openGlMessageTemplate.SubstParam( errorCode );
}

void CheckGlError()
{
#if !defined( RELIB_FINAL )
	const int errorCode = gl::GetError();
	if( errorCode != 0 ) {
		ProgramDebugModeBreak();
		throw COpenGLException( errorCode );
	}
#endif // RELIB_FINAL
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
