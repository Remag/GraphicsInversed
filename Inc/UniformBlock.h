#pragma once
#include <UniformBlockUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// A block of uniforms. Represents a GLSL uniform block with the given layout.
// General class represents shared and packed layout.
template <TBlockLayout layout, class... Types>
class CUniformBlock : public GinInternal::CUniformBlockBase<Types...> {
public:
	template<int N>
	using TElemType = typename VarArgs::At<N, Types...>::Result;

	// Define an uninitialized uniform block.
	explicit CUniformBlock( const CString& name );

	static TBlockLayout Layout()
		{ return layout; }

	// Connect the block with the program that contains it.
	// Program must be linked.
	void Bind( CShaderProgram program );

	// Get the Nth element in a block.
	template <int N>
	void GetAt( TElemType<N>& result ) const;

	// Set the Nth element in a block.
	template <int N>
	void SetAt( const TElemType<N>& newValue );

	void SetAll( const Types&... args );

private:
	// Uniform information.
	struct CUniformData {
		// Offset in the block.
		int Offset;
		// Stride for array elements. 0 for non-arrays.
		int ArrayStride;
		// Stride for a matrix element. 0 for non-matrices.
		int MatrixStride;
	};
	CUniformData data[GinInternal::CUniformBlockBase<Types...>::uniformCount];

	void initUniformBlock( CShaderProgram program, int blockIndex );
	void fillParams( CShaderProgram program, int blockIndex );

	template<int N, class Arg0, class... Args>
	void setNext( const Arg0& arg, const Args&... args );
	template<int N, class Arg0>
	void setNext( const Arg0& arg );

	template<int N, class T>
	void getElem( T& result ) const;
	template<int N, class T, int arrSize>
	void getElem( T (&result)[arrSize] ) const;
	template<int N, int dimX, int dimY, TMatrixOrder matOrder>
	void getElem( CMatrix<float, dimX, dimY, matOrder>& result ) const;
	template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
	void getElem( CMatrix<float, dimX, dimY, matOrder> (&result)[arrSize] ) const;

	template<int N, class T>
	void setElem( const T& result );
	template<int N, class T, int arrSize>
	void setElem( const T (&result)[arrSize] );
	template<int N, int dimX, int dimY, TMatrixOrder matOrder>
	void setElem( const CMatrix<float, dimX, dimY, matOrder>& result );
	template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
	void setElem( const CMatrix<float, dimX, dimY, matOrder> (&result)[arrSize] );
};

//////////////////////////////////////////////////////////////////////////

// Uniform block specialization for the Std140 layout.
template<class... Types>
class CUniformBlock<BL_Std140, Types...> : public GinInternal::CUniformBlockBase<Types...> {
public:
	template<int N>
	using TElemType = typename GinInternal::CUniformBlockBase<Types...>::template TElemType<N>;

	// Define an uninitialized uniform block.
	explicit CUniformBlock( const CString& name );

	static TBlockLayout Layout()
		{ return GinInternal::CUniformBlockBase<Types...>::layout; }

	// Connect the block with the program that contains it.
	// Program must be linked.
	void Bind( CShaderProgram program );

	// Get the Nth element in a block.
	template <int N>
	void GetAt( TElemType<N>& result ) const;

	// Set the Nth element in a block.
	template <int N>
	void SetAt( const TElemType<N>& newValue );

	void SetAll( const Types&... args );

private:
	template<int N, class Arg0, class... Args>
	void setNext( const Arg0& arg, const Args&... args );
	template<int N, class Arg0>
	void setNext( const Arg0& arg );

	static bool checkStd140Layout( CShaderProgram program, int blockIndex );
	template<int N>
	static bool checkStd140Offset( int* offsets, Relib::Types::FalseType finishMarker );
	template<int N>
	static bool checkStd140Offset( int* offsets, Relib::Types::TrueType finishMarker );

	template<int N, class T>
	void getElem( T& result ) const;
	template<int N, class T, int arrSize>
	void getElem( T (&result)[arrSize] ) const;
	template<int N, int dimX, int dimY, TMatrixOrder matOrder>
	void getElem( CMatrix<float, dimX, dimY, matOrder>& result ) const;
	template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
	void getElem( CMatrix<float, dimX, dimY, matOrder> (&result)[arrSize] ) const;

	template<int N, class T>
	void setElem( const T& result );
	template<int N, class T, int arrSize>
	void setElem( const T (&result)[arrSize] );
	template<int N, int dimX, int dimY, TMatrixOrder matOrder>
	void setElem( const CMatrix<float, dimX, dimY, matOrder>& result );
	template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
	void setElem( const CMatrix<float, dimX, dimY, matOrder> (&result)[arrSize] );
};

//////////////////////////////////////////////////////////////////////////

template <TBlockLayout layout, class... Types>
CUniformBlock<layout, Types...>::CUniformBlock( const CString& name ) :
	GinInternal::CUniformBlockBase<Types...>( name )
{
}

template <TBlockLayout layout, class... Types>
void CUniformBlock<layout, Types...>::Bind( CShaderProgram program )
{
	const int blockIndex = this->bindShaderProgram( program );
	// Reserve the buffer memory for the first bound program.
	if( this->isBufferEmpty() ) {
		initUniformBlock( program, blockIndex );
	}
	assert( this->checkUniformCount( program, blockIndex ) );
	assert(this->checkUniformBlockContents( program, blockIndex ) );
}

template <TBlockLayout layout, class... Types>
void CUniformBlock<layout, Types...>::initUniformBlock( CShaderProgram program, int blockIndex )
{
	// Get the buffer size.
	int blockSize;
	gl::GetActiveUniformBlockiv( program.GetId(), blockIndex, gl::UNIFORM_BLOCK_DATA_SIZE, &blockSize );
	// Bind the buffer and fill the offsets array.
	this->reserveBuffer( blockSize );
	this->bindBuffer( blockIndex );
	fillParams( program, blockIndex );
}

// Get the active index offsets and sizes.
template <TBlockLayout layout, class... Types>
void CUniformBlock<layout, Types...>::fillParams( CShaderProgram program, int blockIndex )
{
	unsigned activeIndexes[this->uniformCount];
	gl::GetActiveUniformBlockiv( program.GetId(), blockIndex, gl::UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, reinterpret_cast<int*>( activeIndexes ) );

	int offsets[this->uniformCount];
	int arrStrides[this->uniformCount];
	int matStrides[this->uniformCount];
	gl::GetActiveUniformsiv( program.GetId(), this->uniformCount, activeIndexes, gl::UNIFORM_OFFSET, offsets );
	gl::GetActiveUniformsiv( program.GetId(), this->uniformCount, activeIndexes, gl::UNIFORM_ARRAY_STRIDE, arrStrides );
	gl::GetActiveUniformsiv( program.GetId(), this->uniformCount, activeIndexes, gl::UNIFORM_MATRIX_STRIDE, matStrides );
	
	for( int i = 0; i < this->uniformCount; i++ ) {
		data[i].Offset = offsets[i];
		data[i].ArrayStride = arrStrides[i];
		data[i].MatrixStride = matStrides[i];
	}
	// A sanity insurance.
	// Indexes of the uniforms given by the program should be in ascending order, but no guarantee could be found.
	// If the case of non-consecutive indexes is possible, this class will most likely be rewritten.
	// Meanwhile, the sort below guarantees correct behavior for the end user.
	Sort::InSort( data, this->uniformCount, LessByAction( &CUniformData::Offset ) );
}

template <TBlockLayout layout, class... Types>
template <int N>
void CUniformBlock<layout, Types...>::GetAt( TElemType<N>& result ) const
{
	staticAssert( N < this->uniformCount );
	getElem<N>( result );
}

template<TBlockLayout layout, class... Types>
template<int N, class T>
void CUniformBlock<layout, Types...>::getElem( T& result ) const
{
	return getElemFromBuffer( result, data[N].Offset );
}

template<TBlockLayout layout, class... Types>
template<int N, class T, int arrSize>
void CUniformBlock<layout, Types...>::getElem( T( &result )[arrSize] ) const
{
	return getArrayFromBuffer( result, data[N].ArrayStride, data[N].Offset );
}

template<TBlockLayout layout, class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder>
void CUniformBlock<layout, Types...>::getElem( CMatrix<float, dimX, dimY, matOrder>& result ) const
{
	return getMatrixFromBuffer( result, data[N].MatrixStride, data[N].Offset );
}

template<TBlockLayout layout, class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
void CUniformBlock<layout, Types...>::getElem( CMatrix<float, dimX, dimY, matOrder>( &result )[arrSize] ) const
{
	const int stride = data[N].MatrixStride;
	const int size = matOrder == MO_ColumnMajor ? stride * dimX : stride * dimY;
	int offset = data[N].Offset;

	for( auto& mat : result ) {
		getMatrixFromBuffer( mat, stride, offset );
		offset += size;
	}
}

template <TBlockLayout layout, class... Types>
template <int N>
void CUniformBlock<layout, Types...>::SetAt( const TElemType<N>& newValue )
{
	staticAssert( N < this->uniformCount );
	setElem<N>( newValue );
}

template <TBlockLayout layout, class... Types>
template<int N, class T>
void CUniformBlock<layout, Types...>::setElem( const T& newValue )
{
	return setElemIntoBuffer( newValue, data[N].Offset );
}

template <TBlockLayout layout, class... Types>
template<int N, class T, int arrSize>
void CUniformBlock<layout, Types...>::setElem( const T (&newValue)[arrSize] )
{
	return setArrayIntoBuffer( newValue, data[N].ArrayStride, data[N].Offset );
}

template <TBlockLayout layout, class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder>
void CUniformBlock<layout, Types...>::setElem( const CMatrix<float, dimX, dimY, matOrder>& newValue )
{
	return setMatrixIntoBuffer( newValue, data[N].MatrixStride, data[N].Offset );
}

template <TBlockLayout layout, class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
void CUniformBlock<layout, Types...>::setElem( const CMatrix<float, dimX, dimY, matOrder>( &newValue )[arrSize] )
{
	const int stride = data[N].MatrixStride;
	const int size = matOrder == MO_ColumnMajor ? stride * dimX : stride * dimY;
	int offset = data[N].Offset;

	for( const auto& mat : newValue ) {
		setMatrixIntoBuffer( mat, stride, offset );
		offset += size;
	}
}

template <TBlockLayout layout, class... Types>
void CUniformBlock<layout, Types...>::SetAll( const Types&... args )
{
	setNext<0>( args... );
}

template <TBlockLayout layout, class... Types>
template<int N, class Arg0, class... Args>
void CUniformBlock<layout, Types...>::setNext( const Arg0& arg, const Args&... args )
{
	SetAt<N>( arg );
	setNext<N + 1 >( args... );
}

template <TBlockLayout layout, class... Types>
template<int N, class Arg0>
void CUniformBlock<layout, Types...>::setNext( const Arg0& arg )
{
	SetAt<N>( arg );
}

//////////////////////////////////////////////////////////////////////////

template<class... Types>
CUniformBlock<BL_Std140, Types...>::CUniformBlock( const CString& name ) :
	GinInternal::CUniformBlockBase<Types...>( name )
{
	typedef VarArgs::Last<Types...>::Result TLast;
	const int totalSize = GinInternal::Std140OffsetOfArgAt<sizeof...( Types ) - 1, Types...>::Result + GinInternal::Std140Info<TLast>::Size;
	this->reserveBuffer( totalSize );
}

template<class... Types>
void CUniformBlock<BL_Std140, Types...>::Bind( CShaderProgram program )
{
	const int blockIndex = this->bindShaderProgram( program );
	// Check that the amount of types is correct.
	assert( this->checkUniformCount( program, blockIndex ) );
	// Check that the types are specified correctly.
	assert( this->checkUniformBlockContents( program, blockIndex ) );
	// Check that the specified layout is the same as the real one.
	assert( checkStd140Layout( program, blockIndex ) );
	this->bindBuffer( blockIndex );
}

template<class... Types>
template <int N>
void CUniformBlock<BL_Std140, Types...>::GetAt( TElemType<N>& result ) const
{
	getElem<N>( result );
}

template<class... Types>
template<int N, class T>
void CUniformBlock<BL_Std140, Types...>::getElem( T& result ) const
{
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	getElemFromBuffer( result, offset );
}

template<class... Types>
template<int N, class T, int arrSize>
void CUniformBlock<BL_Std140, Types...>::getElem( T (&result)[arrSize] ) const
{
	const int stride = GinInternal::Std140Info<T[arrSize]>::ElemSize;
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	getArrayFromBuffer( result, stride, offset );
}

template<class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder>
void CUniformBlock<BL_Std140, Types...>::getElem( CMatrix<float, dimX, dimY, matOrder>& result ) const
{
	const int stride = GinInternal::Std140Info< CVector4<float> >::Size;
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	getMatrixFromBuffer( result, stride, offset );
}

template<class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
void CUniformBlock<BL_Std140, Types...>::getElem( CMatrix<float, dimX, dimY, matOrder> (&result)[arrSize] ) const
{
	const int stride = GinInternal::Std140Info< CVector4<float> >::Size;
	const int size = matOrder == MO_ColumnMajor ? stride * dimX : stride * dimY;
	int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;

	for( auto& mat : result ) {
		getMatrixFromBuffer( mat, stride, offset );
		offset += size;
	}
}

template<class... Types>
template <int N>
void CUniformBlock<BL_Std140, Types...>::SetAt( const TElemType<N>& newValue )
{
	setElem<N>( newValue );
}

template <class... Types>
template<int N, class T>
void CUniformBlock<BL_Std140, Types...>::setElem( const T& newValue )
{
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	setElemIntoBuffer( newValue, offset );
}

template <class... Types>
template<int N, class T, int arrSize>
void CUniformBlock<BL_Std140, Types...>::setElem( const T (&newValue)[arrSize] )
{
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	const int stride = GinInternal::Std140Info<T[arrSize]>::ElemSize;
	setArrayIntoBuffer( newValue, stride, offset );
}

template <class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder>
void CUniformBlock<BL_Std140, Types...>::setElem( const CMatrix<float, dimX, dimY, matOrder>& newValue )
{
	const int stride = GinInternal::Std140Info< CVector4<float> >::Size;
	const int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	setMatrixIntoBuffer( newValue, stride, offset );
}

template <class... Types>
template<int N, int dimX, int dimY, TMatrixOrder matOrder, int arrSize>
void CUniformBlock<BL_Std140, Types...>::setElem( const CMatrix<float, dimX, dimY, matOrder> (&newValue)[arrSize] )
{
	const int stride = GinInternal::Std140Info< CVector4<float> >::Size;
	const int size = matOrder == MO_ColumnMajor ? stride * dimX : stride * dimY;
	int offset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;

	for( const auto& mat : newValue ) {
		setMatrixIntoBuffer( mat, stride, offset );
		offset += size;
	}
}

template <class... Types>
void CUniformBlock<BL_Std140, Types...>::SetAll( const Types&... args )
{
	setNext<0>( args... );
}

template <class... Types>
template<int N, class Arg0, class... Args>
void CUniformBlock<BL_Std140, Types...>::setNext( const Arg0& arg, const Args&... args )
{
	SetAt<N>( arg );
	setNext<N + 1 >( args... );
}

template <class... Types>
template<int N, class Arg0>
void CUniformBlock<BL_Std140, Types...>::setNext( const Arg0& arg )
{
	SetAt<N>( arg );
}

// Validation methods.
template<class... Types>
bool CUniformBlock<BL_Std140, Types...>::checkStd140Layout( CShaderProgram program, int blockIndex )
{
	unsigned activeIndexes[this->uniformCount];
	gl::GetActiveUniformBlockiv( program.GetId(), blockIndex, gl::UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
		reinterpret_cast<int*>( activeIndexes ) );
	int offsets[this->uniformCount];
	gl::GetActiveUniformsiv( program.GetId(), this->uniformCount, activeIndexes, gl::UNIFORM_OFFSET, offsets );
	return checkStd140Offset<0>( offsets, Relib::Types::FalseType() );
}

template <class... Types>
template<int N>
bool CUniformBlock<BL_Std140, Types...>::checkStd140Offset( int* offsets, Relib::Types::FalseType finishMarker )
{
	const int stdOffset = GinInternal::Std140OffsetOfArgAt<N, Types...>::Result;
	if(  stdOffset != *offsets ) {
		return false;
	}
	typedef Relib::Types::Conditional<sizeof...( Types ) == N + 1, Types::TrueType, Types::FalseType>::Result TIsLast;
	return checkStd140Offset<N + 1>( offsets + 1, TIsLast() );
}

template <class... Types>
template<int N>
bool CUniformBlock<BL_Std140, Types...>::checkStd140Offset( int*, Relib::Types::TrueType finishMarker )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

