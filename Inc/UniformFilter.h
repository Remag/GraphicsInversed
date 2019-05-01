#pragma once
#include <Gindefs.h>

namespace Gin {

namespace GinInternal {

struct CUniformComponentData {
	CNamedInlineComponent Component;
	int UniformIndex;

	CUniformComponentData( const CNamedInlineComponent& component, int uniformIndex ) : Component( component ), UniformIndex( uniformIndex ) {}
};

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// Mechanism for operating uniforms based on given components.
template <class ComponentClass>
class CUniformFilter {
public:
	// Create an uninitialized filter.
	CUniformFilter() = default;
	// Normally created from the filter owner.
	CUniformFilter( CShaderProgram _shader, CArrayView<GinInternal::CUniformComponentData> _uniformData ) : shader( _shader ), uniformData( _uniformData ) {}

	bool IsValid() const
		{ return shader.IsValid(); }
	CShaderProgram Shader() const
		{ return shader; }

	// Component iteration methods.
	int GetUniformCount() const;
	const CNamedInlineComponent& GetComponent( int pos ) const;
	const CRawUniformData& GetUniformData( int pos ) const;
	int GetUniformIndex( int pos ) const;

	// Fill uniform values using the given component class entity.
	// Shader program must be switched in advance.
	void FillUniforms( CInlineEntityConstRef<ComponentClass> entityRef ) const;

private:
	CShaderProgram shader;
	CArrayView<GinInternal::CUniformComponentData> uniformData;
};

//////////////////////////////////////////////////////////////////////////

template <class ComponentClass>
int CUniformFilter<ComponentClass>::GetUniformCount() const
{
	return uniformData.Size();
}

template <class ComponentClass>
const CNamedInlineComponent& CUniformFilter<ComponentClass>::GetComponent( int pos ) const
{
	return uniformData[pos].Component;
}

template <class ComponentClass>
const CRawUniformData& CUniformFilter<ComponentClass>::GetUniformData( int pos ) const
{
	return shader.GetUniformData( GetUniformIndex( pos ) );
}

template <class ComponentClass>
int CUniformFilter<ComponentClass>::GetUniformIndex( int pos ) const
{
	return uniformData[pos].UniformIndex;
}

template <class ComponentClass>
void CUniformFilter<ComponentClass>::FillUniforms( CInlineEntityConstRef<ComponentClass> entityRef ) const
{
	assert( shader.GetId() == CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() );
	for( const auto& component : uniformData ) {
		const auto uniformLocation = shader.GetUniform( component.UniformIndex );
		shader.SetRawUniform( uniformLocation, entityRef.GetValue( component.Component ) );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

