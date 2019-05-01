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

// Message class type used by shader program warnings.
struct CShaderMessageClass {};
//////////////////////////////////////////////////////////////////////////

// Generic shader. Contains creation and error handling methods common for all shaders.
class GINAPI CShader {
public:
	unsigned GetId() const
		{ return shaderId; }
	void Release();

	static void SetShaderFolder( CUnicodeView newValue );

protected:
	CShader() = default;
	CShader( CShader&& other ) : shaderId( other.shaderId ) { other.shaderId = 0; }
	~CShader();

	// Compile the shader and remember its id.
	void compileShaderFromFile( unsigned shaderType, CUnicodeView filePath );
	void compileShaderFromString( unsigned shaderType, CUnicodeView shaderName, CStringView shaderText );
	bool isCompiled() const
		{ return GetId() != 0; }

private:
	unsigned shaderId = 0;
	// Error that is sent on shader compilation failure.
	static const CError shaderCompilationError;

	static const CUnicodeView defaultShaderFolder;
	static CUnicodeView currentShaderFolder;

	// Retrieve the error message after failed compilation.
	CUnicodeString getCompileLogMessage() const;

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
	explicit CVertexShader( CUnicodeView filePath );
	CVertexShader( CVertexShader&& ) = default;

	// Compile the shader using the given file.
	// FilePath can be absolute or relative.
	// Relative paths are merged with the current shader folder string.
	void CreateFromFile( CUnicodeView filePath );
	// Create shader from string. shaderName is used for error logging.
	void CreateFromString( CUnicodeView shaderName, CStringView shaderText );

private:
	// Extension for vertex shaders.
	static const CUnicodeView vertexExt;

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
	explicit CFragmentShader( CUnicodeView filePath );
	CFragmentShader( CFragmentShader&& ) = default;

	// Compile the shader using the given file.
	// FilePath can be absolute or relative.
	// Relative paths are merged with the current shader folder string.
	void CreateFromFile( CUnicodeView filePath );
	// Create shader from string. shaderName is used for error logging.
	void CreateFromString( CUnicodeView shaderName, CStringView shaderText );

private:
	// Extension for fragment shaders.
	static const CUnicodeView fragmentExt;

	// Copying is prohibited.
	CFragmentShader( const CFragmentShader& ) = delete;
	void operator=( const CFragmentShader& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

