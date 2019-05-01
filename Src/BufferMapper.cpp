#include <common.h>
#pragma hdrstop

#include <BufferMapper.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

BYTE* CBufferMapper::mapBuffer( int bufferId, int bufferReadWriteMode, TBufferType bufferCopyTarget ) const
{
	CBufferObjectBinder binder( bufferCopyTarget, bufferId );
	CheckGlError();
	return reinterpret_cast<BYTE*>( gl::MapBuffer( bufferCopyTarget, bufferReadWriteMode ) );
}

// Release buffer mapping.
// This operation might fail due to buffer invalidation by the system.
// False return value indicates that the mapping operation must be redone.
bool CBufferMapper::unmapBuffer( int bufferId, TBufferType bufferCopyTarget ) const
{
	CBufferObjectBinder binder( bufferCopyTarget, bufferId ); 
	const bool unmapSuccessful = ( gl::UnmapBuffer( bufferCopyTarget ) == gl::TRUE_ );
	CheckGlError();
	return unmapSuccessful;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
