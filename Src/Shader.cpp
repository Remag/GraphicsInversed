#include <common.h>
#pragma hdrstop

#include <Shader.h>
#include <GlContextManager.h>
#include <GinGlobals.h>
#include <GinError.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView CShader::defaultShaderFolder = L"Shaders";
CUnicodeView CShader::currentShaderFolder = defaultShaderFolder;
//////////////////////////////////////////////////////////////////////////

const CError CShader::shaderCompilationError{ L"Shader compilation failed.\nShader name: %0\nError: %1" };

CShader::~CShader()
{
	Release();
}

void CShader::compileShaderFromFile( unsigned shaderType, CUnicodeView filePath )
{
	const CUnicodeString fullPath = FileSystem::MergePath( currentShaderFolder, filePath );
	const CString shaderText = File::ReadText( fullPath );
	compileShaderFromString( shaderType, fullPath, shaderText );
}

void CShader::compileShaderFromString( unsigned shaderType, CUnicodeView shaderName, CStringView shaderText )
{
	assert( !isCompiled() );
	assert( GetGlContextManager().HasContext() );

	shaderId = gl::CreateShader( shaderType );
	const char* shaderTextPtr = shaderText.Ptr();
	const int length = shaderText.Length();
	// Compile the shader from the source.
	gl::ShaderSource( shaderId, 1, &shaderTextPtr, &length );
	gl::CompileShader( shaderId );

#ifdef _DEBUG
	// Report compilation result.
	const CUnicodeString errorMsg = getCompileLogMessage();
	if( !errorMsg.IsEmpty() ) {
		Log::Warning( shaderName + L"\n" + errorMsg, CShaderMessageClass{} );
	}
#endif

	// Check compilation status.
	GLint status;
	gl::GetShaderiv( shaderId, gl::COMPILE_STATUS, &status );
	if( status == gl::FALSE_ ) {
		// Cleanup and throw exception.
		const CUnicodeString errorMsg = getCompileLogMessage();
		Release();
		GenerateCheck( shaderCompilationError, shaderName, errorMsg, CUnicodeString() );
	}
}

CUnicodeString CShader::getCompileLogMessage() const
{
	assert( isCompiled() );
	GLint logLength;
	gl::GetShaderiv( shaderId, gl::INFO_LOG_LENGTH, &logLength );
	CString logString;
	auto logBuffer = logString.CreateRawBuffer( logLength );
	gl::GetShaderInfoLog( shaderId, logLength, 0, logBuffer );
	logBuffer.Release();
	return UnicodeStr( logString );
}

void CShader::Release()
{
	if( isCompiled() && GetGlContextManager().HasContext() ) {
		gl::DeleteShader( GetId() );
		shaderId = 0;
	}
}

void CShader::SetShaderFolder( CUnicodeView newValue )
{
	currentShaderFolder = newValue;
}

//////////////////////////////////////////////////////////////////////////

const CUnicodeView CVertexShader::vertexExt = L".vert";

CVertexShader::CVertexShader( CUnicodeView filePath )
{
	CreateFromFile( filePath );
}

void CVertexShader::CreateFromFile( CUnicodeView filePath )
{
	assert( FileSystem::GetExt( filePath ) == vertexExt );
	compileShaderFromFile( gl::VERTEX_SHADER, filePath );
}

void CVertexShader::CreateFromString( CUnicodeView shaderName, CStringView shaderText )
{
	assert( !shaderName.IsEmpty() );
	compileShaderFromString( gl::VERTEX_SHADER, shaderName, shaderText );
}

//////////////////////////////////////////////////////////////////////////

const CUnicodeView CFragmentShader::fragmentExt = L".frag";

CFragmentShader::CFragmentShader( CUnicodeView filePath )
{
	CreateFromFile( filePath );
}

void CFragmentShader::CreateFromFile( CUnicodeView filePath )
{
	assert( FileSystem::GetExt( filePath ) == fragmentExt );
	compileShaderFromFile( gl::FRAGMENT_SHADER, filePath );
}

void CFragmentShader::CreateFromString( CUnicodeView shaderName, CStringView shaderText )
{
	assert( !shaderName.IsEmpty() );
	compileShaderFromString( gl::FRAGMENT_SHADER, shaderName, shaderText );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

