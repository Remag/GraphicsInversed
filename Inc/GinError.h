#pragma once

#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Open GL error exception.
class GINAPI COpenGLException : public CException {
public:
	explicit COpenGLException( int errorCode );

	// CException.
	virtual CUnicodeString GetMessageText() const override final;

private:
	int errorCode;
};

//////////////////////////////////////////////////////////////////////////

// Throw an Open GL exception if an error is present.
void GINAPI CheckGlError();

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

