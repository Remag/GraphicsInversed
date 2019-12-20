#pragma once
#include <GinDefs.h>
#include <UniformLocation.h>
#include <UniformUtils.h>
#include <GinTypes.h>
#include <GinError.h>

namespace Gin {

struct CParticleVariableData;
//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// Common shader program operations.
class GINAPI CShaderProgramOperations {
public:
	// Has the program id been initialized.
	bool IsValid() const
		{ return programId != 0; }
	// Has the program been successfully linked.
	bool IsLinked() const;
	// Get raw program identifier.
	unsigned GetId() const
		{ return programId; }

protected:
	explicit CShaderProgramOperations( unsigned id ) : programId( id ) {}
	CShaderProgramOperations& operator=( const CShaderProgramOperations& ) = default;

	bool checkUniformsSet( CArrayView<CRawUniformData> allUniforms ) const;

	// Create a wrapper around a uniform with a given name.
	CUniformLocation getUniform( CStringView name, CArrayView<CRawUniformData> allUniforms ) const;
	CUniformLocation getUniform( int uniformPos ) const;
	
	// Set raw uniform value.
	void setRawUniform( CUniformLocation location, CConstRawBuffer value, CArrayView<CRawUniformData> allUniforms ) const;
	// Set uniform value of a given type.
	template <class T>
	void setUniform( CUniformLocation location, const T& value, CArrayView<CRawUniformData> allUniforms ) const;

	void setProgramId( unsigned newValue )
		{ programId = newValue; }

private:
	unsigned programId;
	
	// Error that is set when trying to draw with an unset uniform.
	static const CError unsetUniformError;

	const CRawUniformData& prepareUniformData( CUniformLocation location, CArrayView<CRawUniformData> allUniforms ) const;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
void CShaderProgramOperations::setUniform( CUniformLocation location, const T& value, CArrayView<CRawUniformData> activeUniforms ) const
{
	if( location.GetPosition() == NotFound ) {
		return;
	}
	const auto& uniformData = prepareUniformData( location, activeUniforms );
	assert( GinTypes::template GlType<T>::Type == uniformData.Type );
	GinInternal::SetUniformValue( uniformData, value );
	CheckGlError();
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// Shader program identifier wrapper.
class GINAPI CShaderProgram : public GinInternal::CShaderProgramOperations {
public:
	CShaderProgram() : CShaderProgramOperations( 0 ) {}
	CShaderProgram( unsigned id, CArrayView<CRawUniformData> uniforms ) : CShaderProgramOperations( id ), activeUniforms( uniforms ) {}

	// Check that all program uniforms have been set.
	bool CheckUniformsSet() const
		{ return checkUniformsSet( activeUniforms ); }

	// Get the amount of active uniforms.
	int GetUniformCount() const
		{ return activeUniforms.Size(); }

	// Get a uniform information at the given iteration position.
	const CRawUniformData& GetUniformData( int uniformPos ) const
		{ return activeUniforms[uniformPos]; }

	// Create a wrapper around a uniform with a given name.
	CUniformLocation GetUniform( CStringView name ) const
		{ return getUniform( name, activeUniforms ); }
	// Get a uniform wrapper by the iteration index.
	CUniformLocation GetUniform( int uniformPos ) const
		{ return getUniform( uniformPos ); }

	// Set raw uniform value with an undefined type.
	void SetRawUniform( CUniformLocation location, CConstRawBuffer rawValue ) const
		{ setRawUniform( location, rawValue, activeUniforms ); }
	// Set uniform of a given type.
	template <class T>
	void SetUniform( CUniformLocation location, const T& value ) const
		{ setUniform( location, value, activeUniforms ); }

private:
	CArrayView<CRawUniformData> activeUniforms;
};

//////////////////////////////////////////////////////////////////////////

// Data for shader program construction.
template <class ShaderClass>
class CShaderData {
public:
	template <class... ShaderType>
	CShaderData( const ShaderType&... shaders ) { fillShaderIds( shaders... ); }

	CArrayView<unsigned> GetIds() const
		{ return shaderIds; }

private:
	// Shaders that require compilation.
	CFlexibleArray<ShaderClass, 2> compiledShaders;
	// All identifiers.
	CFlexibleArray<unsigned, 2> shaderIds;

	template <class... ShaderType>
	void fillShaderIds( const ShaderClass& shader, const ShaderType&... rest )
		{ shaderIds.Add( shader.GetId() ); fillShaderIds( rest... ); }
	template <class... ShaderType>
	void fillShaderIds( CUnicodeView shaderPath, const ShaderType&... rest )
		{ fillShaderIds( compiledShaders.Add( ShaderClass( shaderPath ) ), rest... ); }
	void fillShaderIds() {}
};

//////////////////////////////////////////////////////////////////////////

// Vertex attribute layout information.
struct CVertexAttributeLayoutInfo {
	CString Name;
	int Location;

	CVertexAttributeLayoutInfo( CStringView name, int location ) : Name( name ), Location( location ) {}
};

// Correspondence between shader attribute names and their layout index.
class GINAPI CShaderLayoutInfo {
public:
	CShaderLayoutInfo( std::initializer_list<CVertexAttributeLayoutInfo> info );

	const int GetAttributeCount() const
		{ return attributeNames.Size(); }
	const auto& GetAttributeInfo( int pos ) const
		{ return attributeNames[pos]; }

private:
	CFlexibleArray<CVertexAttributeLayoutInfo, 3> attributeNames;

	// Copying is prohibited.
	CShaderLayoutInfo( CShaderLayoutInfo& ) = delete;
	void operator=( CShaderLayoutInfo& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Shader aggregation. Links shaders together into a program.
class GINAPI CShaderProgramOwner : public GinInternal::CShaderProgramOperations {
public:
	// Link a shader program with a given list of vertex and fragment shader data.
	// Shader data can be presented as an initializer list mixed of shader classes and shader file names.
	CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData );
	CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData, const CShaderLayoutInfo& layoutInfo );
	CShaderProgramOwner( CShaderProgramOwner&& other );
	~CShaderProgramOwner();

	operator CShaderProgram() const
		{ return CShaderProgram( GetId(), activeUniforms ); }

	// Check that all program uniforms have been set.
	bool CheckUniformsSet() const
		{ return checkUniformsSet( activeUniforms ); }

	// Get the amount of active uniforms.
	int GetUniformCount() const
		{ return activeUniforms.Size(); }

	// Get a uniform information at the given iteration position.
	const CRawUniformData& GetUniformData( int uniformPos ) const
		{ return activeUniforms[uniformPos]; }

	// Create a wrapper around a uniform with a given name.
	CUniformLocation GetUniform( CStringView name ) const
		{ return getUniform( name, activeUniforms ); }
	// Get a uniform wrapper by the iteration index.
	CUniformLocation GetUniform( int uniformPos ) const
		{ return getUniform( uniformPos ); }

	// Set raw uniform value with an undefined type.
	void SetRawUniform( CUniformLocation location, CConstRawBuffer rawValue ) const
		{ setRawUniform( location, rawValue, activeUniforms ); }
	// Set uniform of a given type.
	template <class T>
	void SetUniform( CUniformLocation location, const T& value ) const
		{ setUniform( location, value, activeUniforms ); }

protected:
	// Link a shader program with an additional transform feedback data.
	CShaderProgramOwner( const CShaderData<CVertexShader>& vertexData, const CShaderData<CFragmentShader>& fragmentData, CArrayView<CParticleVariableData> particleData );

private:
	// Attached shader ids.
	CArray<int> attachedShaders;
	// List of active user-defined uniforms.
	CArray<CRawUniformData> activeUniforms;

	// Error that is sent on shader linking failure.
	static const CError shaderLinkingError;

	void bindLayoutInfo( const CShaderLayoutInfo& layoutInfo );
	void linkProgram( CArrayView<unsigned> vertexIds, CArrayView<unsigned> fragmentIds );

	int getNextFreeBindingPoint( int& previousPoint ) const;
	static int getMaxBindingPoints();

	static unsigned createProgramId();
	void deleteProgram();

	CUnicodeString getLinkingLogMessage() const;

	void fillUniformInformation();

	// Copying is prohibited.
	CShaderProgramOwner( const CShaderProgramOwner& ) = delete;
	void operator=( const CShaderProgramOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Shader program switcher. Calls glUseProgram.
class GINAPI CShaderProgramSwitcher {
public:
	explicit CShaderProgramSwitcher( CShaderProgram program );
	~CShaderProgramSwitcher();

	static CShaderProgram GetCurrentShaderProgram()
		{ return currentShaderProgram; }
	static void SetCurrentShaderProgram( CShaderProgram program );

private:
	// Program that was switched last. The library assumes that the shader program is selected only using this switcher.
	static CShaderProgram currentShaderProgram;

	// Previous program that was selected.
	CShaderProgram prevProgram;

	// Copying is prohibited.
	CShaderProgramSwitcher( const CShaderProgramSwitcher& ) = delete;
	void operator=( const CShaderProgramSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

