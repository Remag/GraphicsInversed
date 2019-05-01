#pragma once

namespace Gin {

namespace GinInternal {
	class CShaderProgramOperations;
}

//////////////////////////////////////////////////////////////////////////

// Wrapper around uniform position information.
// This class does not contain an actual uniform location as defined by OpenGL.
// It contains an index in CShaderProgram class that can be used to retrieve all necessary uniform information.
class CUniformLocation {
public:
	CUniformLocation() = default; 

	int GetPosition() const
		{ return position; }
	int GetProgramId() const
		{ return programId; }

	// Shader program needs access to location creation.
	friend class GinInternal::CShaderProgramOperations;
	
private:
	int position = NotFound;
	int programId = NotFound;

	CUniformLocation( int _position, int _programId ) : position( _position ), programId( _programId ) {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

