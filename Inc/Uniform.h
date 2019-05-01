#pragma once
#include <UniformLocation.h>
#include <UniformUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Strongly typed uniform location.
template <class T>
class CUniform {
public:
	CUniform() = default;
	CUniform( const CUniformLocation& location );

	void Set( const T& newValue ) const;
	const CUniform& operator=( const T& newValue ) const;

private:
	CUniformLocation location;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
CUniform<T>::CUniform( const CUniformLocation& _location ) :
	location( _location )
{
}

template <class T>
const CUniform<T>& CUniform<T>::operator=( const T& newValue ) const
{
	Set( newValue );
	return *this;
}

template <class T>
void CUniform<T>::Set( const T& newValue ) const
{
	CShaderProgramSwitcher::GetCurrentShaderProgram().SetUniform( location, newValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

