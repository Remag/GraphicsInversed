#pragma once
#include <DrawEnums.h>
#include <GlContextManager.h>
#include <GinGlobals.h>
#include <GinError.h>

namespace Gin {

namespace GinInternal {

// General buffer information.
template <TBufferType target>
class CGlBufferData {
public:
	explicit CGlBufferData( int id ) : bufferId( id ) {}

	operator CRawGlBuffer<target>() const;

	unsigned GetId() const
		{ return bufferId; }
	int GetBufferSize() const;
	bool HasData() const
		{ return GetBufferSize() > 0; }

private:
	int bufferId;
};

//////////////////////////////////////////////////////////////////////////

// Common operations on buffer data.
class GINAPI CGlBufferOperations {
public:
	// Get the size of the specified buffer in bytes.
	static int GetBufferSize( int bufferId, TBufferType target );
	// Allocate data of a given size. All the previous data is detached from the buffer.
	static void ReserveBuffer( int bufferId, int size, TBufferType bufferTarget, TBufferUsageHint usageHint );
	// Set the already allocated data.
	static void SetBuffer( int bufferId, CArrayView<BYTE> data, TBufferType bufferTarget, int offset );
	// Combined call to ReserveBuffer and SetBuffer.
	static void CreateBuffer( int bufferId, CArrayView<BYTE> data, TBufferType bufferTarget, TBufferUsageHint usageHint );

	static unsigned CreateBufferId();
	static void FreeBufferId( int bufferId );
};

//////////////////////////////////////////////////////////////////////////

template <TBufferType target>
int CGlBufferData<target>::GetBufferSize() const
{
	return CGlBufferOperations::GetBufferSize( GetId(), target );
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// Base, untyped OpenGL buffer object.
template <TBufferType target>
class CRawGlBuffer : public GinInternal::CGlBufferData<target> {
public:
	explicit CRawGlBuffer( unsigned id ) : GinInternal::CGlBufferData<target>( id ) {}

	template <class... Types>
	CGlBuffer<target, Types...> Typify() const
		{ return CGlBuffer<target, Types...>( this->GetId() ); }
};

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// Typified buffer data.
template <TBufferType target, class... Types>
class CTypedGlBufferData {
public:
	explicit CTypedGlBufferData( int id ) : bufferData( id ) {}

	operator CRawGlBuffer<target>() const
		{ return CRawGlBuffer<target>( GetId() ); }

	unsigned GetId() const
		{ return bufferData.GetId(); }
	int GetBufferSize() const
		{ return bufferData.GetBufferSize(); }
	bool HasData() const
		{ return bufferData.HasData(); }

	int ElemCount() const;

	void ReserveBuffer( int elemCount, TBufferUsageHint usageHint );

	void SetBuffer( CArrayView<CTuple<Types...>> data, int offset );
	void CreateBuffer( CArrayView<CTuple<Types...>> data, TBufferUsageHint hint );

protected:
	CGlBufferData<target> getBufferData()
		{ return bufferData; }

private:
	CGlBufferData<target> bufferData;
};

// Buffer data specialization for buffers containing a single type.
template <TBufferType target, class T>
class CTypedGlBufferData<target, T> {
public:
	explicit CTypedGlBufferData( int id ) : bufferData( id ) {}

	operator CRawGlBuffer<target>() const
		{ return CRawGlBuffer<target>( GetId() ); }

	unsigned GetId() const
		{ return bufferData.GetId(); }
	int GetBufferSize() const
		{ return bufferData.GetBufferSize(); }
	bool HasData() const
		{ return bufferData.HasData(); }

	int ElemCount() const;

	void ReserveBuffer( int elemCount, TBufferUsageHint usageHint );

	void SetBuffer( CArrayView<T> data, int offset = 0 )
		{ CGlBufferOperations::SetBuffer( GetId(), static_cast<CArrayView<BYTE>>( data ), target, offset * sizeof( T ) ); }
	void CreateBuffer( CArrayView<T> data, TBufferUsageHint hint );
	
protected:
	CGlBufferData<target> getBufferData()
		{ return bufferData; }

private:
	CGlBufferData<target> bufferData;
};

//////////////////////////////////////////////////////////////////////////

template <TBufferType target>
CGlBufferData<target>::operator CRawGlBuffer<target>() const
{
	return CRawGlBuffer<target>( GetId() );
}

template <TBufferType target, class... Types>
int CTypedGlBufferData<target, Types...>::ElemCount() const
{
	const int elemSize = VarArgs::SizeOfAll<Types...>::Result;
	const int bufferSize = bufferData.GetBufferSize();
	assert( bufferSize % elemSize == 0 );
	return bufferSize / elemSize;
}

template <TBufferType target, class... Types>
void CTypedGlBufferData<target, Types...>::ReserveBuffer( int elemCount, TBufferUsageHint usageHint )
{
	const int newSize = elemCount * VarArgs::SizeOfAll<Types...>::Result;
	CGlBufferOperations::ReserveBuffer( GetId(), newSize, target, usageHint );
}

template <TBufferType target, class... Types>
void CTypedGlBufferData<target, Types...>::SetBuffer( CArrayView<CTuple<Types...>> data, int offset )
{
	const int elemSize = VarArgs::SizeOfAll<Types...>::Result;
	static_assert( sizeof( CTuple<Types...> ) == elemSize, "C type alignment differs from the OpenGL one." );
	CGlBufferOperations::SetBuffer( GetId(), static_cast<CArrayView<BYTE>>( data ), target, offset * elemSize );
}

template <TBufferType target, class... Types>
void CTypedGlBufferData<target, Types...>::CreateBuffer( CArrayView<CTuple<Types...>> data, TBufferUsageHint hint )
{
	static_assert( sizeof( CTuple<Types...> ) == VarArgs::SizeOfAll<Types...>::Result, "C type alignment differs from the OpenGL one." );
	const CArrayView<BYTE> rawData = static_cast<CArrayView<BYTE>>( data );
	CGlBufferOperations::CreateBuffer( GetId(), rawData, target, hint );
}

template <TBufferType target, class T>
int CTypedGlBufferData<target, T>::ElemCount() const
{
	const int elemSize = sizeof( T );
	assert( bufferData.GetBufferSize() % elemSize == 0 );
	return bufferData.GetBufferSize() / elemSize;
}

template <TBufferType target, class T>
void CTypedGlBufferData<target, T>::ReserveBuffer( int elemCount, TBufferUsageHint usageHint )
{
	const int newSize = elemCount * sizeof( T );
	CGlBufferOperations::ReserveBuffer( GetId(), newSize, target, usageHint );
}

template <TBufferType target, class T>
void CTypedGlBufferData<target, T>::CreateBuffer( CArrayView<T> data, TBufferUsageHint hint )
{
	const CArrayView<BYTE> rawData = static_cast<CArrayView<BYTE>>( data );
	CGlBufferOperations::CreateBuffer( GetId(), rawData, target, hint );
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// An OpenGL buffer object. Contains interleaved variables of given types.
// Doesn't own the underlying buffer id.
template <TBufferType target, class... Types>
class CGlBuffer : public GinInternal::CTypedGlBufferData<target, Types...> {
public:
	CGlBuffer() : GinInternal::CTypedGlBufferData<target, Types...>( NotFound ) {}
	explicit CGlBuffer( unsigned id ) : GinInternal::CTypedGlBufferData<target, Types...>( id ) {}
};

//////////////////////////////////////////////////////////////////////////

// OpenGL buffer object with runtime-defined type information.
template <TBufferType target>
class CDynamicGlBuffer : public GinInternal::CGlBufferData<target> {
public:
	// Types array is not copied and must persist through dynamic buffer's lifetime.
	CDynamicGlBuffer( CRawGlBuffer<target> baseBuffer, CArrayView<TGlType> types ) : GinInternal::CGlBufferData<target>( baseBuffer ), bufferTypes( types ) {}
	CDynamicGlBuffer( int bufferId, CArrayView<TGlType> types ) : GinInternal::CGlBufferData<target>( bufferId ), bufferTypes( types ) {}

	CArrayView<TGlType> GetTypeInfo() const
		{ return bufferTypes; }

private:
	CArrayView<TGlType> bufferTypes;
};

//////////////////////////////////////////////////////////////////////////

// A buffer identifier owner.
template <TBufferType target>
class CRawGlBufferOwner : public GinInternal::CGlBufferData<target> {
public:
	CRawGlBufferOwner() : GinInternal::CGlBufferData<target>( GinInternal::CGlBufferOperations::CreateBufferId() ) {}
	CRawGlBufferOwner( CRawGlBufferOwner&& other );
	CRawGlBufferOwner& operator=( CRawGlBufferOwner&& other );
	template <class... Types>
	CRawGlBufferOwner( CGlBufferOwner<target, Types...>&& other );

	static CRawGlBufferOwner CreateRawBuffer()
		{ return CRawGlBufferOwner( CRawCreationTag() ); }

	~CRawGlBufferOwner()
		{ GinInternal::CGlBufferOperations::FreeBufferId( this->GetId() ); }

	void CreateBuffer( CArrayView<BYTE> data, TBufferUsageHint hint );
		
	operator CRawGlBuffer<target>() const
		{ return CRawGlBuffer<target>( this->GetId() ); }

private:
	struct CRawCreationTag {};
	CRawGlBufferOwner( CRawCreationTag ) : GinInternal::CGlBufferData<target>( NotFound ) {}

	// Copying is prohibited.
	CRawGlBufferOwner( CRawGlBufferOwner& ) = delete;
	void operator=( CRawGlBufferOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <TBufferType target>
template <class... Types>
CRawGlBufferOwner<target>::CRawGlBufferOwner( CGlBufferOwner<target, Types...>&& other ) :
	GinInternal::CGlBufferData<target>( other.GetId() )
{
	other.GinInternal::CTypedGlBufferData<target, Types...>::operator=( GinInternal::CTypedGlBufferData<target, Types...>{ NotFound } );
}
	
template <TBufferType target>
CRawGlBufferOwner<target>::CRawGlBufferOwner( CRawGlBufferOwner<target>&& other ) :
	GinInternal::CGlBufferData<target>( other.GetId() )
{
	other.GinInternal::CGlBufferData<target>::operator=( GinInternal::CGlBufferData<target>{ NotFound } );
}

template <TBufferType target>
CRawGlBufferOwner<target>& CRawGlBufferOwner<target>::operator=( CRawGlBufferOwner<target>&& other )
{
	GinInternal::CGlBufferOperations::FreeBufferId( this->GetId() );

	GinInternal::CGlBufferData<target>::operator=( { other } );
	other.GinInternal::CGlBufferData<target>::operator=( GinInternal::CGlBufferData<target>{ NotFound } );
	return *this;
}

template <TBufferType target>
void CRawGlBufferOwner<target>::CreateBuffer( CArrayView<BYTE> rawData, TBufferUsageHint hint )
{
	GinInternal::CGlBufferOperations::CreateBuffer( this->GetId(), rawData, target, hint );
}

//////////////////////////////////////////////////////////////////////////

// Typed buffer owner.
template <TBufferType target, class... Types>
class CGlBufferOwner : public GinInternal::CTypedGlBufferData<target, Types...> {
public:
	CGlBufferOwner() : GinInternal::CTypedGlBufferData<target, Types...>( GinInternal::CGlBufferOperations::CreateBufferId() ) {}
	CGlBufferOwner( CGlBufferOwner&& other );
	CGlBufferOwner& operator=( CGlBufferOwner&& other );
	static CGlBufferOwner<target, Types...> CreateRawBuffer()
		{ return CGlBufferOwner<target, Types...>( CRawCreationTag() ); }

	~CGlBufferOwner()
		{ GinInternal::CGlBufferOperations::FreeBufferId( this->GetId() ); }
		
	CGlBuffer<target, Types...> View() const
		{ return CGlBuffer<target, Types...>( this->GetId() ); }
	operator CGlBuffer<target, Types...>() const
		{ return View(); }

private:
	struct CRawCreationTag {};
	CGlBufferOwner( CRawCreationTag ) : GinInternal::CTypedGlBufferData<target, Types...>( NotFound ) {}

	// Copying is prohibited.
	CGlBufferOwner( CGlBufferOwner& ) = delete;
	void operator=( CGlBufferOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <TBufferType target, class... Types>
CGlBufferOwner<target, Types...>::CGlBufferOwner( CGlBufferOwner<target, Types...>&& other ) :
	GinInternal::CTypedGlBufferData<target, Types...>( other.GetId() )
{
	other.GinInternal::CTypedGlBufferData<target, Types...>::operator=( GinInternal::CTypedGlBufferData<target, Types...>{ NotFound } );
}

template <TBufferType target, class... Types>
CGlBufferOwner<target, Types...>& CGlBufferOwner<target, Types...>::operator=( CGlBufferOwner<target, Types...>&& other )
{
	GinInternal::CGlBufferOperations::FreeBufferId( this->GetId() );

	GinInternal::CTypedGlBufferData<target, Types...>::operator=( { other } );
	other.GinInternal::CTypedGlBufferData<target, Types...>::operator=( GinInternal::CTypedGlBufferData<target, Types...>{ NotFound } );
	return *this;
}

//////////////////////////////////////////////////////////////////////////

// Class for binding buffer objects. Calls gl::BindBuffer in contructor and destructor.
class GINAPI CBufferObjectBinder {
public:
	CBufferObjectBinder( TBufferType _bufferType, int bufferId );
	template <TBufferType target>
	explicit CBufferObjectBinder( CRawGlBuffer<target> buffer ) : CBufferObjectBinder( target, buffer.GetId() ) {}
	template <TBufferType target>
	explicit CBufferObjectBinder( CDynamicGlBuffer<target> buffer ) : CBufferObjectBinder( target, buffer.GetId() ) {}
	template <TBufferType target, class... Types>
	explicit CBufferObjectBinder( GinInternal::CTypedGlBufferData<target, Types...> buffer ) : CBufferObjectBinder( target, buffer.GetId() ) {}
	template <TBufferType target, class... Types>
	explicit CBufferObjectBinder( CGlBuffer<target, Types...> buffer ) : CBufferObjectBinder( target, buffer.GetId() ) {}
	template <TBufferType target, class... Types>
	explicit CBufferObjectBinder( const CGlBufferOwner<target, Types...>& buffer ) : CBufferObjectBinder( target, buffer.GetId() ) {}
		
	~CBufferObjectBinder();

private:
	// Type of the bound buffer.
	TBufferType bufferType;

	// Copying is prohibited.
	CBufferObjectBinder( const CBufferObjectBinder& ) = delete;
	void operator=( const CBufferObjectBinder& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// Buffer conversion functions.
template <TBufferType newTarget, TBufferType oldTarget, class... Types>
CGlBuffer<newTarget, Types...> ConvertGlBuffer( CGlBuffer<oldTarget, Types...> buffer )
{
	return CGlBuffer<newTarget, Types...>( buffer.GetId() );
}

template <TBufferType newTarget, TBufferType oldTarget>
CDynamicGlBuffer<newTarget> ConvertGlBuffer( CDynamicGlBuffer<oldTarget> buffer )
{
	return CDynamicGlBuffer<newTarget>( buffer.GetId(), buffer.GetTypeInfo() );
}

template <TBufferType newTarget, TBufferType oldTarget>
CRawGlBuffer<newTarget> ConvertGlBuffer( CRawGlBuffer<oldTarget> buffer )
{
	return CRawGlBuffer<newTarget>( buffer.GetId() );
}

template <TBufferType newTarget, TBufferType oldTarget>
CRawGlBuffer<newTarget> ConvertGlBuffer( const CRawGlBufferOwner<oldTarget>& buffer )
{
	return CRawGlBuffer<newTarget>( buffer.GetId() );
}

template <TBufferType newTarget, TBufferType oldTarget, class... Types>
CGlBuffer<newTarget, Types...> ConvertGlBuffer( const CGlBufferOwner<oldTarget, Types...>& buffer )
{
	return CGlBuffer<newTarget, Types...>( buffer.GetId() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

