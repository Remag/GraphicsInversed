#pragma once
#include <DrawEnums.h>
#include <GlBuffer.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Mechanism for mapping buffer objects onto the process address space.
// All access to a buffer is prohibited during the mapping.
class GINAPI CBufferMapper {
public:
	// Map buffer for writing or reading/writing.
	// mapOperation is a callable that takes an CArrayView<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>> of mapped entities. It also takes args... as additional arguments.
	// The mapping operation might be performed multiple times due to possible buffer corruption.
	template <TBufferType target, class FirstBufferType, class SecondBufferType, class MapAction, class... RestBufferTypes, class... Args>
	CBufferMapper( TBufferWriteMappingMode mapMode, GinInternal::CTypedGlBufferData<target, FirstBufferType, SecondBufferType, RestBufferTypes...> bufferObject, 
		MapAction mapOperation, Args&&... args );
	// mapOperation takes CArrayView<BufferType> of mapped entities.
	template <TBufferType target, class BufferType, class MapAction, class... Args>
	CBufferMapper( TBufferWriteMappingMode mapMode, GinInternal::CTypedGlBufferData<target, BufferType> bufferObject, MapAction mapOperation, Args&&... args );
	// mapOperation takes CArrayView<BYTE> of mapped entities.
	template <TBufferType target, class MapAction, class... Args>
	CBufferMapper( TBufferWriteMappingMode mapMode, CRawGlBuffer<target> bufferObject, MapAction mapOperation, Args&&... args );

	// Map buffer for reading.
	template <TBufferType target, class FirstBufferType, class SecondBufferType, class MapAction, class... RestBufferTypes, class... Args>
	CBufferMapper( GinInternal::CTypedGlBufferData<target, FirstBufferType, SecondBufferType, RestBufferTypes...> bufferObject, MapAction mapOperation, Args&&... args );
	template <TBufferType target, class BufferType, class MapAction, class... Args>
	CBufferMapper( GinInternal::CTypedGlBufferData<target, BufferType> bufferObject, MapAction mapOperation, Args&&... args );
	template <TBufferType target, class MapAction, class... Args>
	CBufferMapper( CRawGlBuffer<target> bufferObject, MapAction mapOperation, Args&&... args );

private:
	static const unsigned readOnlyConstant = 0x88B8;	// gl::READ_ONLY.

	BYTE* mapBuffer( int bufferId, int bufferReadWriteMode, TBufferType bufferCopyTarget ) const;
	bool unmapBuffer( int bufferId, TBufferType bufferCopyTarget ) const;

	// Copying is prohibited.
	CBufferMapper( CBufferMapper& ) = delete;
	void operator=( CBufferMapper& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <TBufferType target, class FirstBufferType, class SecondBufferType, class MapAction, class... RestBufferTypes, class... Args>
CBufferMapper::CBufferMapper( TBufferWriteMappingMode mapMode, GinInternal::CTypedGlBufferData<target, FirstBufferType, SecondBufferType, RestBufferTypes...> bufferObject, 
	MapAction mapOperation, Args&&... args )
{
	const int bufferSize = bufferObject.GetBufferSize();
	static const int argCount = Types::FunctionInfo<MapAction>::ArgCount;
	static_assert( argCount > 0, "Last argument of mapOperation must be CArrayView<BufferType>" );
	typedef Types::FunctionInfo<MapAction>::template ArgTypeAt<argCount - 1> TArray;
	static_assert( Types::IsSame<TArray, CArrayBuffer<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>>>::Result, "Last argument of mapOperation must be CArrayBuffer<CTuple>" );
	const int tupleSize = sizeof( CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...> );
	assert( bufferSize % tupleSize == 0 );
	const int arraySize = bufferSize / tupleSize;
	do {
		BYTE* buffer = mapBuffer( bufferObject.GetId(), mapMode, BT_CopyWrite );
		Invoke( mapOperation, forward<Args>( args )..., CArrayBuffer<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>>( 
			reinterpret_cast<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>*>( buffer ), arraySize ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

template <TBufferType target, class BufferType, class MapAction, class... Args>
CBufferMapper::CBufferMapper( TBufferWriteMappingMode mapMode, GinInternal::CTypedGlBufferData<target, BufferType> bufferObject, MapAction mapOperation, Args&&... args )
{
	const int bufferSize = bufferObject.GetBufferSize();
	static const int argCount = Types::FunctionInfo<MapAction>::ArgCount;
	static_assert( argCount > 0, "Last argument of mapOperation must be CArrayView<BufferType>" );
	typedef Types::FunctionInfo<MapAction>::template ArgTypeAt<argCount - 1> TArray;
	static_assert( Types::IsSame<TArray, CArrayBuffer<BufferType>>::Result, "Last argument of mapOperation must be CArrayBuffer<BufferType>" );
	assert( bufferSize % sizeof( BufferType ) == 0 );
	const int arraySize = bufferSize / sizeof( BufferType );
	do {
		BYTE* buffer = mapBuffer( bufferObject.GetId(), mapMode, BT_CopyWrite );
		Invoke( mapOperation, forward<Args>( args )..., CArrayBuffer<BufferType>( reinterpret_cast<BufferType*>( buffer ), arraySize ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

template <TBufferType target, class MapAction, class... Args>
CBufferMapper::CBufferMapper( TBufferWriteMappingMode mapMode, CRawGlBuffer<target> bufferObject, MapAction mapOperation, Args&&... args )
{
	do {
		BYTE* buffer = mapBuffer( bufferObject.GetId(), mapMode, BT_CopyWrite );
		Invoke( mapOperation, forward<Args>( args )..., CArrayBuffer<BYTE>( buffer, bufferObject.GetBufferSize() ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

template <TBufferType target, class FirstBufferType, class SecondBufferType, class MapAction, class... RestBufferTypes, class... Args>
CBufferMapper::CBufferMapper( GinInternal::CTypedGlBufferData<target, FirstBufferType, SecondBufferType, RestBufferTypes...> bufferObject, MapAction mapOperation, Args&&... args )
{
	const int bufferSize = bufferObject.GetBufferSize();
	static const int argCount = Types::FunctionInfo<MapAction>::ArgCount;
	static_assert( argCount > 0, "Last argument of mapOperation must be CArrayView<BufferType>" );
	typedef Types::FunctionInfo<MapAction>::template ArgTypeAt<argCount - 1> TArray;
	static_assert( Types::IsSame<TArray, CArrayView<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>>>::Result, "Last argument of mapOperation must be CArrayView<CTuple>" );
	const int tupleSize = sizeof( CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...> );
	assert( bufferSize % tupleSize == 0 );
	const int arraySize = bufferSize / tupleSize;
	do {
		const BYTE* buffer = mapBuffer( bufferObject.GetId(), readOnlyConstant, BT_CopyRead );
		Invoke( mapOperation, forward<Args>( args )..., CArrayView<CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>>( 
			reinterpret_cast<const CTuple<FirstBufferType, SecondBufferType, RestBufferTypes...>*>( buffer ), arraySize ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

template <TBufferType target, class BufferType, class MapAction, class... Args>
CBufferMapper::CBufferMapper( GinInternal::CTypedGlBufferData<target, BufferType> bufferObject, MapAction mapOperation, Args&&... args )
{
	const int bufferSize = bufferObject.GetBufferSize();
	static const int argCount = Types::FunctionInfo<MapAction>::ArgCount;
	static_assert( argCount > 0, "Last argument of mapOperation must be CArrayView<BufferType>" );
	typedef Types::FunctionInfo<MapAction>::template ArgTypeAt<argCount - 1> TArray;
	static_assert( Types::IsSame<TArray, CArrayView<BufferType>>::Result, "Last argument of mapOperation must be CArrayView<BufferType>" );
	assert( bufferSize % sizeof( BufferType ) == 0 );
	const int arraySize = bufferSize / sizeof( BufferType );
	do {
		const BYTE* buffer = mapBuffer( bufferObject.GetId(), readOnlyConstant, BT_CopyRead );
		Invoke( mapOperation, forward<Args>( args )..., CArrayView<BufferType>( reinterpret_cast<const BufferType*>( buffer ), arraySize ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

template <TBufferType target, class MapAction, class... Args>
CBufferMapper::CBufferMapper( CRawGlBuffer<target> bufferObject, MapAction mapOperation, Args&&... args )
{
	do {
		const BYTE* buffer = mapBuffer( bufferObject.GetId(), readOnlyConstant, BT_CopyRead );
		Invoke( mapOperation, forward<Args>( args )..., CArrayView<BYTE>( buffer, bufferObject.GetBufferSize() ) );
	} while( !unmapBuffer( bufferObject.GetId(), BT_CopyWrite ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

