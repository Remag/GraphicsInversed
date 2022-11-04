#include <common.h>
#pragma hdrstop

#include <Shader.h>
#include <GlContextManager.h>
#include <GinGlobals.h>
#include <GinError.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

const CStringView CShader::defaultShaderFolder = "Shaders";
CString CShader::currentShaderFolder( defaultShaderFolder );
//////////////////////////////////////////////////////////////////////////

const CError CShader::shaderCompilationError{ "Shader compilation failed.\nShader name: %0\nError: %1" };
CShader::~CShader()
{
	Release();
}

void CShader::compileShaderFromFile( unsigned shaderType, CStringView filePath )
{
	const auto fullPath = FileSystem::MergePath( currentShaderFolder, filePath );
	const auto shaderText = File::ReadText( fullPath );
	compileShaderFromString( shaderType, fullPath, shaderText );
}

void CShader::compileShaderFromString( unsigned shaderType, CStringView shaderName, CStringView shaderText )
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
	const auto errorMsg = getCompileLogMessage();
	if( !errorMsg.IsEmpty() ) {
		Log::Warning( shaderName + "\n" + errorMsg );
	}
#endif

	// Check compilation status.
	GLint status;
	gl::GetShaderiv( shaderId, gl::COMPILE_STATUS, &status );
	if( status == gl::FALSE_ ) {
		// Cleanup and throw exception.
		const auto errorMsg = getCompileLogMessage();
		Release();
		GenerateCheck( shaderCompilationError, shaderName, errorMsg, CString() );
	}
}

CString CShader::getCompileLogMessage() const
{
	assert( isCompiled() );
	GLint logLength;
	gl::GetShaderiv( shaderId, gl::INFO_LOG_LENGTH, &logLength );
	CString logString;
	auto logBuffer = logString.CreateRawBuffer( logLength );
	gl::GetShaderInfoLog( shaderId, logLength, 0, logBuffer );
	logBuffer.Release();
	return logString;
}

void CShader::Release()
{
	if( isCompiled() && GetGlContextManager().HasContext() ) {
		gl::DeleteShader( GetId() );
		shaderId = 0;
	}
}

void CShader::SetShaderFolder( CString newValue )
{
	currentShaderFolder = newValue;
}

//////////////////////////////////////////////////////////////////////////

const CStringView CVertexShader::vertexExt = ".vert";
CVertexShader::CVertexShader( CStringView filePath )
{
	CreateFromFile( filePath );
}

void CVertexShader::CreateFromFile( CStringView filePath )
{
	assert( FileSystem::GetExt( filePath ) == vertexExt );
	compileShaderFromFile( gl::VERTEX_SHADER, filePath );
}

void CVertexShader::CreateFromString( CStringView shaderName, CStringView shaderText )
{
	assert( !shaderName.IsEmpty() );
	compileShaderFromString( gl::VERTEX_SHADER, shaderName, shaderText );
}

//////////////////////////////////////////////////////////////////////////

const CStringView CFragmentShader::fragmentExt = ".frag";
CFragmentShader::CFragmentShader( CStringView filePath )
{
	CreateFromFile( filePath );
}

void CFragmentShader::CreateFromFile( CStringView filePath )
{
	assert( FileSystem::GetExt( filePath ) == fragmentExt );
	compileShaderFromFile( gl::FRAGMENT_SHADER, filePath );
}

void CFragmentShader::CreateFromString( CStringView shaderName, CStringView shaderText )
{
	assert( !shaderName.IsEmpty() );
	compileShaderFromString( gl::FRAGMENT_SHADER, shaderName, shaderText );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

