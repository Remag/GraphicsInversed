#include <common.h>
#pragma hdrstop

#include <ShaderProgram.h>
#include <Shader.h>
#include <GinError.h>
#include <GlContextManager.h>
#include <GinGlobals.h>
#include <ParticleEmitter.h>

namespace Gin {

namespace GinInternal {

const CError CShaderProgramOperations::unsetUniformError{ "Shader uniform %0 is not set before the draw call." };
//////////////////////////////////////////////////////////////////////////

bool CShaderProgramOperations::IsLinked() const
{
	assert( programId != 0 );
	GLint status;
	gl::GetProgramiv( programId, gl::LINK_STATUS, &status );
	return status == gl::TRUE_;
}

bool CShaderProgramOperations::checkUniformsSet( CArrayView<CRawUniformData> allUniforms ) const
{
	for( const auto& data : allUniforms ) {
		if( !data.IsSetFlag ) {
			GenerateCheck( unsetUniformError, data.Name );
			return false;
		}
	}
	return true;
}

CUniformLocation CShaderProgramOperations::getUniform( CStringView name, CArrayView<CRawUniformData> activeUniforms ) const
{
	const int dataPos = SearchPos( activeUniforms, name, EqualByAction( &CRawUniformData::Name ) );
#ifdef _DEBUG
	if( dataPos == NotFound ) {
		// Uniform with this name wasn't found, maybe it wasn't active. Report the error but continue working.
		static const CStringView inactiveUniformMsg = "Inactive uniform found: %0.";
		Log::Warning( inactiveUniformMsg.SubstParam( name ) );
	}
#endif
	return getUniform( dataPos );
}

CUniformLocation CShaderProgramOperations::getUniform( int uniformPos ) const
{
	return CUniformLocation( uniformPos, programId );
}

void CShaderProgramOperations::setRawUniform( CUniformLocation location, CConstRawBuffer value, CArrayView<CRawUniformData> allUniforms ) const
{
	if( location.GetPosition() == NotFound ) {
		return;
	}
	const auto& uniformData = prepareUniformData( location, allUniforms );
	GinInternal::SetRawUniformValue( uniformData, value );
	CheckGlError();
}

const CRawUniformData& CShaderProgramOperations::prepareUniformData( CUniformLocation location, CArrayView<CRawUniformData> allUniforms ) const
{
	assert( location.GetProgramId() == static_cast<int>( programId ) );
	assert( CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() == programId );
	const auto& result = allUniforms[location.GetPosition()];
	result.IsSetFlag = true;
	return result;
}

}

//////////////////////////////////////////////////////////////////////////

CShaderLayoutInfo::CShaderLayoutInfo( std::initializer_list<CVertexAttributeLayoutInfo> info )
{
	attributeNames.ReserveBuffer( info.size() );
	for( const auto& attribInfo : info ) {
		attributeNames.AddWithinCapacity( Str( attribInfo.Name ), attribInfo.Location );
	}
}

//////////////////////////////////////////////////////////////////////////

const CError CShaderProgramOwner::shaderLinkingError{ "Shader linking failed.\nError: %0" };
GLuint CShaderProgramOwner::createProgramId()
{
	const GLuint result = gl::CreateProgram();
	CheckGlError();
	return result;
}

CShaderProgramOwner::CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData ) :
	CShaderProgramOperations( createProgramId() )
{
	linkProgram( vertexData.GetIds(), fragmentData.GetIds() );
}

CShaderProgramOwner::CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData, const CShaderLayoutInfo& layoutInfo ) :
	CShaderProgramOperations( createProgramId() )
{
	bindLayoutInfo( layoutInfo );
	linkProgram( vertexData.GetIds(), fragmentData.GetIds() );
}

CShaderProgramOwner::CShaderProgramOwner( CShaderProgramOwner&& other ) :
	CShaderProgramOperations( other ),
	attachedShaders( move( other.attachedShaders ) ),
	activeUniforms( move( other.activeUniforms ) )
{
	other.setProgramId( 0 );
}

CShaderProgramOwner::CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData, CArrayView<CParticleVariableData> particleData ) :
	CShaderProgramOperations( createProgramId() )
{
	CFlexibleArray<const char*, 4> particleNames;
	particleNames.IncreaseSizeNoInitialize( particleData.Size() );
	for( int i = 0; i < particleData.Size(); i++ ) {
		particleNames[i] = particleData[i].OutName.Ptr();
	}
	gl::TransformFeedbackVaryings( GetId(), particleData.Size(), particleNames.Ptr(), gl::INTERLEAVED_ATTRIBS );

	linkProgram( vertexData.GetIds(), fragmentData.GetIds() );
}

CShaderProgramOwner::~CShaderProgramOwner()
{
	deleteProgram();
}

void CShaderProgramOwner::bindLayoutInfo( const CShaderLayoutInfo& layoutInfo )
{
	const int layoutInfoSize = layoutInfo.GetAttributeCount();
	for( int i = 0; i < layoutInfoSize; i++ ) {
		const auto& attribInfo = layoutInfo.GetAttributeInfo( i );
		gl::BindAttribLocation( GetId(), attribInfo.Location, attribInfo.Name.Ptr() );
	}
	CheckGlError();
}

void CShaderProgramOwner::linkProgram( CArrayView<unsigned> vertexIds, CArrayView<unsigned> fragmentIds )
{
	assert( GetId() != 0 );
	assert( !IsLinked() );

	// Attach shaders.
	for( int id : vertexIds ) {
		gl::AttachShader( GetId(), id );
	}
	for( int id : fragmentIds ) {
		gl::AttachShader( GetId(), id );
	}

	// Perform linking.
	gl::LinkProgram( GetId() );

#ifdef _DEBUG

	// Report linking result.
	const auto errorMsg = getLinkingLogMessage();
	if( !errorMsg.IsEmpty() ) {
		Log::Warning( errorMsg );
	}
#endif

	// Get link status.
	GLint status;
	gl::GetProgramiv( GetId(), gl::LINK_STATUS, &status );
	if( !IsLinked() ) {
		const auto errorMsg = getLinkingLogMessage();
		deleteProgram();
		GenerateCheck( shaderLinkingError, errorMsg );
	} else {
		fillUniformInformation();
	}
	
	// Detach shaders.
	for( int id : vertexIds ) {
		gl::DetachShader( GetId(), id );
	}
	for( int id : fragmentIds ) {
		gl::DetachShader( GetId(), id );
	}
}

void CShaderProgramOwner::deleteProgram()
{
	if( GetId() != 0 ) {
		gl::DeleteProgram( GetId() );
		setProgramId( 0 );
	}
}

CString CShaderProgramOwner::getLinkingLogMessage() const
{
	assert( GetId() != 0 );
	GLint logLength;
	gl::GetProgramiv( GetId(), gl::INFO_LOG_LENGTH, &logLength );
	CString logString;
	auto logBuffer = logString.CreateRawBuffer( logLength );
	gl::GetProgramInfoLog( GetId(), logLength, 0, logBuffer );
	logBuffer.Release();
	return logString;
}

static CStringView builtInUniformPrefix = "gl_";
void CShaderProgramOwner::fillUniformInformation()
{
	int uniformCount;
	gl::GetProgramiv( GetId(), gl::ACTIVE_UNIFORMS, &uniformCount );
	assert( uniformCount >= 0 );
	int maxUniformLength;
	gl::GetProgramiv( GetId(), gl::ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength );
	assert( maxUniformLength >= 0 );

	int currentBindingPoint = 0;
	// Go through all the uniforms and find the current one.
	for( int i = 0; i < uniformCount; i++ ) {
		CRawUniformData newData;
		int nameLength;
		GLenum uniformType;
		auto nameBuffer = newData.Name.CreateRawBuffer( maxUniformLength );
		gl::GetActiveUniform( GetId(), i, maxUniformLength, &nameLength, &newData.Size, &uniformType, nameBuffer );
		nameBuffer.Release( nameLength );
		// Check for a rare case of an active built in uniform.
		if( newData.Name.Find( builtInUniformPrefix ) == 0 ) {
			continue;
		}
		newData.Location = gl::GetUniformLocation( GetId(), newData.Name.Ptr() );
		assert( newData.Location != NotFound );
		newData.Type = static_cast<TGlType>( uniformType );
		// Create a binding point for textures.
		switch( newData.Type ) {
			case GLT_Texture1:
			case GLT_Texture2:
			case GLT_Texture3:
			case GLT_TextureCube:
			case GLT_TextureArray1:
			case GLT_TextureArray2:
				newData.BindingPoint = getNextFreeBindingPoint( currentBindingPoint );
				break;
			default:
				newData.BindingPoint = NotFound;
				break;
		}
		activeUniforms.Add( move( newData ) );
	}
}

int CShaderProgramOwner::getNextFreeBindingPoint( int& previousPoint ) const
{
	// Binding points start from index 1. Binding point 0 is reserved as a dummy point
	// that is active by default so that other bindings are intact during various operations with textures.
	// Unfortunately, this means that total number of textures is one less than provided by hardware.
	// Oh well.
	const int result = ++previousPoint;
	assert( result < getMaxBindingPoints() );
	return result;
}

int CShaderProgramOwner::getMaxBindingPoints()
{
	static int result = 0;
	if( result == 0 ) {
		gl::GetIntegerv( gl::MAX_COMBINED_TEXTURE_IMAGE_UNITS, &result );
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////

CShaderProgram CShaderProgramSwitcher::currentShaderProgram;

CShaderProgramSwitcher::CShaderProgramSwitcher( CShaderProgram program ) :
	prevProgram( currentShaderProgram )
{
	assert( program.IsLinked() );
	SetCurrentShaderProgram( program );
}

CShaderProgramSwitcher::~CShaderProgramSwitcher()
{
	SetCurrentShaderProgram( prevProgram );
}

void CShaderProgramSwitcher::SetCurrentShaderProgram( CShaderProgram program )
{
	gl::UseProgram( program.GetId() );
	currentShaderProgram = program;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
