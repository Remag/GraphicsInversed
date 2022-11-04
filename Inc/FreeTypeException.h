#pragma once
#include <GinDefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

class GINAPI CFreeTypeException : public CException {
public:
	// Create an exception from the given code.
	explicit CFreeTypeException( int code );

	virtual CString GetMessageText() const override;
	
private:
	// Basic error description.
	CString errorBase;
	// Name of the FreeType module that threw an error.
	CString moduleName;
	// Code of the error.
	int errorCode;

	void setInfoFromCode( int code );
	static CStringView getModuleName( int code );
};

//////////////////////////////////////////////////////////////////////////

inline void checkFreeTypeError( int errorCode )
{
	if( errorCode != 0 ) {
		throw CFreeTypeException( errorCode );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.