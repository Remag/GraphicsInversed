#pragma once
#include <Gindefs.h>
#include <DrawEnums.h>

namespace Gin {

namespace GinInternal {
	template <class Type>
	class CTextureUniform;
}

template <class Type>
class CUniform;

//////////////////////////////////////////////////////////////////////////

// Generic shader. Contains creation and error handling methods common for all shaders.
class GINAPI CShader {
public:
	unsigned GetId() const
		{ return shaderId; }
	void Release();

	static void SetShaderFolder( CString newValue );

protected:
	CShader() = default;
	CShader( CShader&& other ) : shaderId( other.shaderId ) { other.shaderId = 0; }
	~CShader();

	// Compile the shader and remember its id.
	void compileShaderFromFile( unsigned shaderType, CStringView filePath );
	void compileShaderFromString( unsigned shaderType, CStringView shaderName, CStringView shaderText );
	bool isCompiled() const
		{ return GetId() != 0; }

private:
	unsigned shaderId = 0;
	// Error that is sent on shader compilation failure.
	static const CError shaderCompilationError;

	static const CStringView defaultShaderFolder;
	static CString currentShaderFolder;

	// Retrieve the error message after failed compilation.
	CString getCompileLogMessage() const;

	// Copying is prohibited.
	CShader( CShader& ) = delete;
	void operator=( CShader& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Shader wrapper. Loads and compiles vertex shaders.
class GINAPI CVertexShader : public CShader {
public:
	CVertexShader() = default;
	// Load and compile using CreateFromFile.
	explicit CVertexShader( CStringView filePath );
	CVertexShader( CVertexShader&& ) = default;

	// Compile the shader using the given file.
	// FilePath can be absolute or relative.
	// Relative paths are merged with the current shader folder string.
	void CreateFromFile( CStringView filePath );
	// Create shader from string. shaderName is used for error logging.
	void CreateFromString( CStringView shaderName, CStringView shaderText );

private:
	// Extension for vertex shaders.
	static const CStringView vertexExt;

	// Copying is prohibited.
	CVertexShader( const CVertexShader& ) = delete;
	void operator=( const CVertexShader& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Shader wrapper. Loads and compiles fragment shaders.
class GINAPI CFragmentShader : public CShader {
public:
	CFragmentShader() = default;
	// Load and compile.
	explicit CFragmentShader( CStringView filePath );
	CFragmentShader( CFragmentShader&& ) = default;

	// Compile the shader using the given file.
	// FilePath can be absolute or relative.
	// Relative paths are merged with the current shader folder string.
	void CreateFromFile( CStringView filePath );
	// Create shader from string. shaderName is used for error logging.
	void CreateFromString( CStringView shaderName, CStringView shaderText );

private:
	// Extension for fragment shaders.
	static const CStringView fragmentExt;

	// Copying is prohibited.
	CFragmentShader( const CFragmentShader& ) = delete;
	void operator=( const CFragmentShader& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

