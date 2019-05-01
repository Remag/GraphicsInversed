#pragma once
#include <Shader.h>
#include <UniformUtils.h>
#include <ShaderProgram.h>
#include <UniformFilter.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mechanism that separates the shader program uniforms into classes based by the component associated with the uniform's name.
template <class... ComponentClasses>
class CUniformFilterOwner {
public:
	staticAssert( sizeof...( ComponentClasses ) > 0 );

	// Create a filter for existing components. Uniforms that do not correspond to an existing component are ignored.
	explicit CUniformFilterOwner( CShaderProgram _shader ) : shader( _shader ) { fillUniforms(); }

	CShaderProgram Shader() const
		{ return shader; }

	template <class ComponentClass>
	CUniformFilter<ComponentClass> GetFilter() const;
	template <class ComponentClass>
	operator CUniformFilter<ComponentClass>() const
		{ return GetFilter<ComponentClass>(); }

	// Fill uniform values using the given component class entity.
	// Shader program must be switched in advance.
	template <class ComponentClass>
	void FillUniforms( CInlineEntityConstRef<ComponentClass> entityRef ) const;

	// Add a newly created named component to the filter. It must not be present in the filter.
	template <class ComponentClass>
	void AddNewComponent( const CNamedInlineComponent& newComponent, int componentIndex );

private:
	CShaderProgram shader;
	// Uniforms separated by their component class.
	CStackArray<CArray<GinInternal::CUniformComponentData>, sizeof...( ComponentClasses )> uniformComponents;

	void fillUniforms();

	template <int currentPos, class LastClass>
	static int getComponentClassPos( const CNamedInlineComponent& component );
	template <int currentPos, class FirstClass, class SecondClass, class... RestClasses>
	static int getComponentClassPos( const CNamedInlineComponent& component );
};

//////////////////////////////////////////////////////////////////////////

template <class... ComponentClasses>
void CUniformFilterOwner<ComponentClasses...>::fillUniforms()
{
	assert( shader.IsLinked() );
	for( int i = 0; i < shader.GetUniformCount(); i++ ) {
		const auto& uniformData = shader.GetUniformData( i );
		auto component = CNamedInlineComponent::Get( uniformData.Name );
		if( component != nullptr ) {
			const int classPos = getComponentClassPos<0, ComponentClasses...>( *component );
			if( classPos != NotFound ) {
				uniformComponents[classPos].Add( *component, i );
			}
		}
	}
}

template <class... ComponentClasses>
template <int currentPos, class LastClass>
int CUniformFilterOwner<ComponentClasses...>::getComponentClassPos( const CNamedInlineComponent& component )
{
	return component.GetClassId() == GetComponentClassId<LastClass>() ? currentPos : NotFound;
}

template <class... ComponentClasses>
template <int currentPos, class FirstClass, class SecondClass, class... RestClasses>
int CUniformFilterOwner<ComponentClasses...>::getComponentClassPos( const CNamedInlineComponent& component )
{
	const int testPos = getComponentClassPos<currentPos, FirstClass>( component );
	return testPos != NotFound ? testPos : getComponentClassPos<currentPos + 1, SecondClass, RestClasses...>( component );
}

template <class... ComponentClasses>
template <class ComponentClass>
CUniformFilter<ComponentClass> CUniformFilterOwner<ComponentClasses...>::GetFilter() const
{
	const int classPos = VarArgs::TargetPos<ComponentClass, ComponentClasses...>::Result;
	return CUniformFilter<ComponentClass>( shader, uniformComponents[classPos] );
}

template <class... ComponentClasses>
template <class ComponentClass>
void CUniformFilterOwner<ComponentClasses...>::FillUniforms( CInlineEntityConstRef<ComponentClass> entityRef ) const
{
	assert( shader.GetId() == CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() );
	return GetFilter<ComponentClass>().FillUniforms( entityRef );
}

template <class... ComponentClasses>
template <class ComponentClass>
void CUniformFilterOwner<ComponentClasses...>::AddNewComponent( const CNamedInlineComponent& newComponent, int componentIndex )
{
	assert( newComponent.Name() == shader.GetUniformData( componentIndex ).Name );
	const int classPos = VarArgs::TargetPos<ComponentClass, ComponentClasses...>::Result;
	uniformComponents[classPos].Add( newComponent, componentIndex );
}

//////////////////////////////////////////////////////////////////////////

// Get an existing named component from a filter or create a new one using the data from the filter's shader program.
// A given name must be present in the filters shader program. New component is added to the filter.
// An index of the component in the filter is returned.
template <class TargetClass, class... FilterClasses>
int GetOrCreateGlComponent( CStringView name, CUniformFilterOwner<FilterClasses...>& filterOwner )
{
	const auto shader = filterOwner.Shader();
	const auto targetFilter = filterOwner.GetFilter<TargetClass>();
	const int componentCount = targetFilter.GetUniformCount();
	for( int i = 0; i < componentCount; i++ ) {
		const CStringView compName = targetFilter.GetComponent( i ).Name();
		if( compName == name ) {
			// Found an existing component.
			return i;
		}
	}

	// No component with the given name exists in the filter.
	// Find the necessary component information in the shader.
	const int uniformCount = shader.GetUniformCount();
	for( int i = 0; i < uniformCount; i++ ) {
		const auto& uniformData = shader.GetUniformData( i );
		if( uniformData.Name == name ) {
			const auto& typeData = GinTypes::GetGlTypeInformation( uniformData.Type );
			CNamedInlineComponent newComponent = CNamedInlineComponent::GetOrCreate<TargetClass>( name, typeData.ByteSize, typeData.CAlingment );
			filterOwner.AddNewComponent<TargetClass>( newComponent, i );
			return componentCount;			
		}
	}

	// Couldn't find the component in the shader.
	assert( false );
	return NotFound;
}

// Get or create a component by its position in the filter's shader program.
// Same restrictions as before apply.
template <class TargetClass, class... FilterClasses>
int GetOrCreateGlComponent( int uniformPos, CUniformFilterOwner<FilterClasses...>& filterOwner )
{
	const auto targetFilter = filterOwner.GetFilter<TargetClass>();
	const int componentCount = targetFilter.GetUniformCount();
	for( int i = 0; i < componentCount; i++ ) {
		const int compUniformPos = targetFilter.GetUniformIndex( i );
		if( uniformPos == compUniformPos ) {
			// Found an existing component.
			return i;
		}
	}

	const auto& uniformData = filterOwner.Shader().GetUniformData( uniformPos );
	const auto& typeData = GinTypes::GetGlTypeInformation( uniformData.Type );
	CNamedInlineComponent newComponent = CNamedInlineComponent::GetOrCreate<TargetClass>( uniformData.Name, typeData.ByteSize, typeData.CAlingment );
	filterOwner.AddNewComponent<TargetClass>( newComponent, uniformPos );
	return componentCount;	
}

// Common filters.
typedef CUniformFilterOwner<Material::ComponentClass, LightSource::ComponentClass, Vertex::ComponentClass> TModelUniformFilterOwner;
typedef CUniformFilterOwner<LightSource::ComponentClass> TLightUniformFilterOwner;
typedef CUniformFilter<LightSource::ComponentClass> TLightUniformFilterView;

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

