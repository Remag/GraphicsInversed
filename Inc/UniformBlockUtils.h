// Utility classes and functions for the uniform block.

#pragma once
#include <DrawEnums.h>
#include <Gindefs.h>
#include <GinError.h>
#include <GinTypes.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

// Std140 size and alignment information for various type.
template <class Type>
struct Std140Info {
};

template<>
struct Std140Info<int> {
	static const int Size = 4;
	static const int BaseAlignment = 4;
};

template<>
struct Std140Info<unsigned> {
	static const int Size = 4;
	static const int BaseAlignment = 4;
};

template<>
struct Std140Info<float> {
	static const int Size = 4;
	static const int BaseAlignment = 4;
};

template<>
struct Std140Info< CVector2<int> > {
	static const int Size = 8;
	static const int BaseAlignment = 8;
};

template<>
struct Std140Info< CVector3<int> > {
	static const int Size = 12;
	static const int BaseAlignment = 16;
};

template<>
struct Std140Info< CVector4<int> > {
	static const int Size = 16;
	static const int BaseAlignment = 16;
};

template<>
struct Std140Info< CVector2<float> > {
	static const int Size = 8;
	static const int BaseAlignment = 8;
};

template<>
struct Std140Info< CVector3<float> > {
	static const int Size = 12;
	static const int BaseAlignment = 16;
};

template<>
struct Std140Info< CVector4<float> > {
	static const int Size = 16;
	static const int BaseAlignment = 16;
};

template <int dimX, int dimY>
struct Std140Info< CMatrix<float, dimX, dimY, MO_ColumnMajor> > {
	staticAssert( dimX <= 4 && dimY <= 4 );

	static const int Size = 16 * dimX;
	static const int BaseAlignment = 16;
};

template <int dimX, int dimY>
struct Std140Info< CMatrix<float, dimX, dimY, MO_RowMajor> > {
	staticAssert( dimX <= 4 && dimY <= 4 );

	static const int Size = 16 * dimY;
	static const int BaseAlignment = 16;
};

// Specialization for arrays.
template <class ElemType, int i>
struct Std140Info<ElemType[i]> {
private:
	static const int simpleSize = Std140Info<ElemType>::Size;
	static const int simpleAlignment = Std140Info<ElemType>::BaseAlignment;
	static const int vec4Size = Std140Info< CVector4<float> >::Size;
	static const int vec4Alignment = Std140Info< CVector4<float> >::BaseAlignment;

public:
	static const int ElemSize = ( ( simpleSize + vec4Size - 1 ) / vec4Size ) * vec4Size;
	static const int ElemBaseAlignment = ( ( simpleAlignment + vec4Alignment - 1 ) / vec4Alignment ) * vec4Alignment;
	static const int Size = ElemSize * i;
	static const int BaseAlignment = ElemBaseAlignment;
};

//////////////////////////////////////////////////////////////////////////

// Additional utility classed for variadic templates.

template <int N, int BaseOffset, class... Args>
struct Std140TotalOffsetOfArgAt;

template <int BaseOffset, class Arg0, class... Args>
struct Std140TotalOffsetOfArgAt<0, BaseOffset, Arg0, Args...> {
	static const int argAlignment = Std140Info<Arg0>::BaseAlignment;
	static const int Result = ( ( BaseOffset + argAlignment - 1 ) / argAlignment ) * argAlignment;
};

template <int N, int BaseOffset, class Arg0, class... Args>
struct Std140TotalOffsetOfArgAt<N, BaseOffset, Arg0, Args...> {
	static const int argAlignment = Std140Info<Arg0>::BaseAlignment;
	static const int argSize = Std140Info<Arg0>::Size;
	static const int argOffset = ( ( BaseOffset + argAlignment - 1 ) / argAlignment ) * argAlignment;
	static const int Result = Std140TotalOffsetOfArgAt<N - 1, argOffset + argSize, Args...>::Result;
};

// Offset of the Nth argument according to the std140 layout.
template <int N, class... Args>
struct Std140OffsetOfArgAt;

template <class... Args>
struct Std140OffsetOfArgAt<0, Args...> {
	static const int Result = 0;
};

template <int N, class Arg0, class... Args>
struct Std140OffsetOfArgAt<N, Arg0, Args...> {
	staticAssert( static_cast<unsigned>( N ) <= sizeof...( Args ) );
	static const int argSize = Std140Info<Arg0>::Size;
	static const int Result = Std140TotalOffsetOfArgAt<N - 1, argSize, Args...>::Result;
};

extern CAtomic<int> CurrentUniformBindingPointIndex;
//////////////////////////////////////////////////////////////////////////

enum TUniformBlockGlConstants {
	UBGC_MaxUniformBufferBindings = 0x8A2F,	// gl::MAX_UNIFORM_BUFFER_BINDINGS
	UBGC_InvalidUniformIndex = 0xFFFFFFFF,	// gl::INVALID_INDEX
	UBGC_ActiveUniformIndices = 0x8A43,	// gl::UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
	UBGC_ActiveBlockUniforms = 0x8A42,	// gl::UNIFORM_BLOCK_ACTIVE_UNIFORMS
	UBGC_UniformType = 0x8A37,	// gl::UNIFORM_TYPE
	UBGC_IsUniformRowMajor = 0x8A3E,	// gl::UNIFORM_IS_ROW_MAJOR
};

// OpenGL operations with uniform blocks.
class GINAPI CUniformBlockOperations {
protected:
	int getMaxUniformBlocks();
	// Bind the buffer to the binding point.
	void bindBuffer( CGlBuffer<BT_Uniform, BYTE> uniformBuffer, int bindingPointIndex, int blockIndex );
	// Check if the real block's uniform count in the shader matches the count, given by the user.
	bool checkUniformCount( CShaderProgram program, int blockIndex, int uniformCount );
	// Bind a shader program to the binding index.
	// Index of the block is returned.
	int bindShaderProgram( CShaderProgram program, CStringView name, int bindingPointIndex );

	void fillUniformInformation( CShaderProgram program, unsigned* activeIndices, int* uniformTypes, int* layouts, int blockIndex, int uniformCount ) const;
};

//////////////////////////////////////////////////////////////////////////

// Base class for the uniform block. Contains common functionality for the blocks with different layouts.
template <class... Types>
class CUniformBlockBase : public CUniformBlockOperations {
public:
	staticAssert( sizeof...( Types ) > 0 );
	
	template<int N>
	using TElemType = typename VarArgs::At<N, Types...>::Result;
	const CString& Name() const
		{ return name; }

protected:
	static const int uniformCount = sizeof...( Types );

	explicit CUniformBlockBase( const CString& name );

	bool isBufferEmpty() const
		{ return uniformBuffer.GetBufferSize() == 0; }
	// Reserve the given size of memory for the buffer.
	void reserveBuffer( int bufferSize );

	template<class Elem>
	void getElemFromBuffer( Elem& result, int offset ) const;
	template<class Elem>
	void setElemIntoBuffer( const Elem& newValue, int offset );

	template<class ArrElem, int arrSize>
	void getArrayFromBuffer( ArrElem (&result)[arrSize], int stride, int offset ) const;
	template<class ArrElem, int arrSize>
	void setArrayIntoBuffer( const ArrElem (&newValue)[arrSize], int stride, int offset );

	template<int dimX, int dimY>
	void getMatrixFromBuffer( CMatrix<float, dimX, dimY, MO_ColumnMajor>& result, int stride, int offset ) const;
	template<int dimX, int dimY>
	void setMatrixIntoBuffer( const CMatrix<float, dimX, dimY, MO_ColumnMajor>& newValue, int stride, int offset );
	template<int dimX, int dimY>
	void getMatrixFromBuffer( CMatrix<float, dimX, dimY, MO_RowMajor>& result, int stride, int offset ) const;
	template<int dimX, int dimY>
	void setMatrixIntoBuffer( const CMatrix<float, dimX, dimY, MO_RowMajor>& newValue, int stride, int offset );

	static bool checkUniformBlockContents( CShaderProgram program, int blockIndex );
	
private:
	// Name of the block.
	const CString name;
	// Buffer, corresponding to this block.
	CGlBufferOwner<BT_Uniform, BYTE> uniformBuffer;
	// Binding point for this block.
	int bindingPointIndex = NotFound;

	int createUniqueBindingPoint();

	template<class Type0, class... RestTypes>
	static bool checkUniformType( int* realTypes, Relib::Types::FalseType finishMarker );
	template<class... RestTypes>
	static bool checkUniformType( int* realTypes, Relib::Types::TrueType finishMarker );

	template<class Type0, class... RestTypes>
	static bool checkMatricesLayout( int* layouts, Relib::Types::FalseType finishMarker );
	template<class... RestTypes>
	static bool checkMatricesLayout( int* layouts, Relib::Types::TrueType finishMarker );

	template<class MatrixType>
	static bool checkMatrixLayout( int layout, Relib::Types::FalseType matrixMarker );
	template<class MatrixType>
	static bool checkMatrixLayout( int layout, Relib::Types::TrueType matrixMarker );
};

//////////////////////////////////////////////////////////////////////////

template <class... Types>
CUniformBlockBase<Types...>::CUniformBlockBase( const CString& _name ) :
	name( _name ),
	bindingPointIndex( createUniqueBindingPoint() )
{
	CheckGlError();
}

// Get the global binding point for the block.
template <class... Types>
int CUniformBlockBase<Types...>::createUniqueBindingPoint()
{
	const int result = CurrentUniformBindingPointIndex.PreIncrement();
	assert( result < getMaxUniformBlocks() );
	return result;
}

template <class... Types>
void CUniformBlockBase<Types...>::reserveBuffer( int bufferSize )
{
	uniformBuffer.SetBufferSize( bufferSize, BUH_StreamDraw );
}

// Basic getElem version for elements which size is equal in C and GLSL.
template <class... Types>
template<class Elem>
void CUniformBlockBase<Types...>::getElemFromBuffer( Elem& result, int offset ) const
{
	uniformBuffer.GetData( &result, sizeof( result ), offset );
}

// Basic setElem version for elements which size is equal in C and GLSL.
template <class... Types>
template<class Elem>
void CUniformBlockBase<Types...>::setElemIntoBuffer( const Elem& elem, int offset ) 
{
	uniformBuffer.SetData( &elem, sizeof( elem ), offset );
}

template <class... Types>
template<class ArrElem, int arrSize>
void CUniformBlockBase<Types...>::getArrayFromBuffer( ArrElem (&result)[arrSize], int stride, int offset ) const
{
	// Calculate the size of a gap between elements in a buffer.
	const int elemSize = sizeof( result[0] );
	assert( stride >= elemSize );
	// Read the array from the correct place in buffer, element by element.
	for( auto& elem : result ) {
		uniformBuffer.GetData( &elem, elemSize, offset );
		offset += stride;
	}
}

template <class... Types>
template<class ArrElem, int arrSize>
void CUniformBlockBase<Types...>::setArrayIntoBuffer( const ArrElem (&newValue)[arrSize], int stride, int offset )
{
	// Calculate the size of a gap between elements in a buffer.
	const int elemSize = sizeof( newValue[0] );
	assert( stride >= elemSize );
	// Read the array from the correct place in buffer, element by element.
	for( const auto& elem : newValue ) {
		uniformBuffer.SetData( &elem, elemSize, offset );
		offset += stride;
	}
}

template <class... Types>
template<int dimX, int dimY>
void CUniformBlockBase<Types...>::getMatrixFromBuffer( CMatrix<float, dimX, dimY, MO_ColumnMajor>& result, int stride, int offset ) const
{
	CVector<float, dimY> resultColumn = CVector<float, dimY>::CreateRawVector();
	const int columnSize = dimY * sizeof( float );
	assert( stride >= columnSize );
	for( int i = 0; i < dimX; i++ ) {
		uniformBuffer.GetData( resultColumn.Ptr(), columnSize, offset );
		result.SetColumn( i, resultColumn );
		offset += stride;
	}
}

template <class... Types>
template<int dimX, int dimY>
void CUniformBlockBase<Types...>::setMatrixIntoBuffer( const CMatrix<float, dimX, dimY, MO_ColumnMajor>& newValue, int stride, int offset )
{
	const int columnSize = dimY * sizeof( float );
	assert( stride >= columnSize );
	CVector<float, dimY> column = CVector<float, dimY>::CreateRawVector();

	for( int i = 0; i < dimX; i++ ) {
		newValue.GetColumn( i, column );
		uniformBuffer.SetData( column.Ptr(), columnSize, offset );
		offset += stride;
	}
}

template <class... Types>
template<int dimX, int dimY>
void CUniformBlockBase<Types...>::getMatrixFromBuffer( CMatrix<float, dimX, dimY, MO_RowMajor>& result, int stride, int offset ) const
{
	const int rowSize = dimX * sizeof( float );
	assert( stride >= rowSize );
	CVector<float, dimX> row = CVector<float, dimX>::CreateRawVector();

	for( int i = 0; i < dimY; i++ ) {
		uniformBuffer.GetData( row.Ptr(), rowSize, offset );
		result.SetRow( i, row );
		offset += stride;
	}
}

template <class... Types>
template<int dimX, int dimY>
void CUniformBlockBase<Types...>::setMatrixIntoBuffer( const CMatrix<float, dimX, dimY, MO_RowMajor>& newValue, int stride, int offset )
{
	const int rowSize = dimX * sizeof( float );
	assert( stride >= rowSize );
	CVector<float, dimX> row = CVector<float, dimX>::CreateRawVector();

	for( int i = 0; i < dimY; i++ ) {
		newValue.GetRow( i, row );
		uniformBuffer.SetData( row.Ptr(), rowSize, offset );
		offset += stride;
	}
}

// Check if the block in the program has the same size as the created buffer.
template <class... Types>
bool CUniformBlockBase<Types...>::checkUniformBlockContents( CShaderProgram program, int blockIndex ) 
{
	unsigned activeIndices[uniformCount];
	int uniformTypes[uniformCount];
	int layouts[uniformCount];

	fillUniformInformation( program, activeIndices, uniformTypes, layouts, blockIndex, uniformCount );
	return checkUniformType<Types...>( uniformTypes, Types::FalseType() ) && checkMatricesLayout<Types...>( layouts, Types::FalseType() );
}

template <class... TT>
template<class Type0, class... RestTypes>
bool CUniformBlockBase<TT...>::checkUniformType( int* realTypes, Relib::Types::FalseType finishMarker )
{
	if( GinTypes::GlType<Type0>::Type != *realTypes ) {
		return false;
	}
	typedef Types::Conditional<sizeof...( RestTypes ) == 0, Types::TrueType, Types::FalseType>::Result TIsLast;
	return checkUniformType<RestTypes...>( realTypes + 1, TIsLast() );
}

template <class... Types>
template<class... RestTypes>
bool CUniformBlockBase<Types...>::checkUniformType( int* realTypes, Relib::Types::TrueType finishMarker )
{
	return true;
}


template <class... TT>
template<class Type0, class... RestTypes>
bool CUniformBlockBase<TT...>::checkMatricesLayout( int* layouts, Relib::Types::FalseType finishMarker )
{
	if( !checkMatrixLayout<Type0>( *layouts, Types::IsMatrix<Type0>() ) ) {
		return false;
	}
	typedef Types::Conditional<sizeof...( RestTypes ) == 0, Types::TrueType, Types::FalseType>::Result TIsLast;
	return checkMatricesLayout<RestTypes...>( layouts + 1, TIsLast() );
}

template <class... Types>
template<class... RestTypes>
bool CUniformBlockBase<Types...>::checkMatricesLayout( int* layouts, Relib::Types::TrueType finishMarker )
{
	return true;
}

template <class... Types>
template<class MatrixType>
bool CUniformBlockBase<Types...>::checkMatrixLayout( int layout, Relib::Types::FalseType matrixMarker )
{
	return true;
}

template <class... Types>
template<class MatrixType>
bool CUniformBlockBase<Types...>::checkMatrixLayout( int layout, Relib::Types::TrueType matrixMarker )
{
	const bool isRealRowMajor = layout != 0;
	const bool isTypeRowMajor = MatrixType::MatrixOrder() == MO_RowMajor;
	return !isRealRowMajor ^ isTypeRowMajor;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


