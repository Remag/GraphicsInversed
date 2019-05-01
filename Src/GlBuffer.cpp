#include <common.h>
#pragma hdrstop

#include <GlBuffer.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

int CGlBufferOperations::GetBufferSize( int bufferId, TBufferType target )
{
	assert( bufferId != NotFound );
	CBufferObjectBinder binder( target, bufferId );
	int result = 0;
	gl::GetBufferParameteriv( target, gl::BUFFER_SIZE, &result );
	CheckGlError();
	return result;
}

void CGlBufferOperations::ReserveBuffer( int bufferId, int size, TBufferType bufferTarget, TBufferUsageHint usageHint )
{
	assert( bufferId != NotFound );
	assert( size >= 0 );
	assert( bufferTarget != BT_Undefined );
	CBufferObjectBinder binder( bufferTarget, bufferId );
	gl::BufferData( bufferTarget, size, nullptr, usageHint );
	CheckGlError();
}

void CGlBufferOperations::SetBuffer( int bufferId, CArrayView<BYTE> data, TBufferType bufferTarget, int offset )
{
	assert( bufferId != NotFound );
	assert( data.Size() + offset <= GetBufferSize( bufferId, bufferTarget ) );
	assert( bufferTarget != BT_Undefined );
	CBufferObjectBinder binder( bufferTarget, bufferId );
	gl::BufferSubData( bufferTarget, offset, data.Size(), data.Ptr() );
	CheckGlError();
}

void CGlBufferOperations::CreateBuffer( int bufferId, CArrayView<BYTE> data, TBufferType bufferTarget, TBufferUsageHint usageHint )
{
	assert( bufferId != NotFound );
	assert( bufferTarget != BT_Undefined );
	CBufferObjectBinder binder( bufferTarget, bufferId );
	gl::BufferData( bufferTarget, data.Size(), data.Ptr(), usageHint );
	CheckGlError();
}

unsigned CGlBufferOperations::CreateBufferId()
{
	assert( GetGlContextManager().HasContext() );
	GLuint newId;
	gl::GenBuffers( 1, &newId );
	return newId;
}

void CGlBufferOperations::FreeBufferId( int bufferId )
{
	if( bufferId == NotFound ) {
		return;
	}
	assert( GetGlContextManager().HasContext() );
	const GLuint deleteId = bufferId;
	gl::DeleteBuffers( 1, &deleteId );
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

CBufferObjectBinder::CBufferObjectBinder( TBufferType _bufferType, int bufferId ) :
	bufferType( _bufferType ) 
{
	gl::BindBuffer( _bufferType, bufferId );
}

CBufferObjectBinder::~CBufferObjectBinder()
{
	gl::BindBuffer( bufferType, 0 );
	CheckGlError();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
