#include <common.h>
#pragma hdrstop

#include <UniformBlockUtils.h>
#include <GlBuffer.h>
#include <ShaderProgram.h>

namespace Gin {

namespace GinInternal {

//////////////////////////////////////////////////////////////////////////

int CUniformBlockOperations::getMaxUniformBlocks()
{
	int result;
	gl::GetIntegerv( UBGC_MaxUniformBufferBindings, &result );
	return result;
}

void CUniformBlockOperations::bindBuffer( CGlBuffer<BT_Uniform, BYTE> uniformBuffer, int bindingPointIndex, int )
{
	gl::BindBufferRange( BT_Uniform, bindingPointIndex, uniformBuffer.GetId(), 0, uniformBuffer.GetBufferSize() );
}

bool CUniformBlockOperations::checkUniformCount( CShaderProgram program, int blockIndex, int uniformCount )
{
	int realUniformCount;
	gl::GetActiveUniformBlockiv( program.GetId(), blockIndex, UBGC_ActiveBlockUniforms, &realUniformCount );
	return realUniformCount == uniformCount;
}

int CUniformBlockOperations::bindShaderProgram( CShaderProgram program, CStringView name, int bindingPointIndex )
{
	assert( program.IsLinked() );

	const int blockIndex = gl::GetUniformBlockIndex( program.GetId(), name.Ptr() );
	assert( blockIndex != UBGC_InvalidUniformIndex && bindingPointIndex != NotFound );

	gl::UniformBlockBinding( program.GetId(), blockIndex, bindingPointIndex );
	CheckGlError();
	return blockIndex;
}

void CUniformBlockOperations::fillUniformInformation( CShaderProgram program, unsigned* activeIndices, int* uniformTypes, int* layouts, int blockIndex, int uniformCount ) const
{
	gl::GetActiveUniformBlockiv( program.GetId(), blockIndex, UBGC_ActiveUniformIndices, reinterpret_cast<int*>( activeIndices ) );
	gl::GetActiveUniformsiv( program.GetId(), uniformCount, activeIndices, UBGC_UniformType, uniformTypes );
	gl::GetActiveUniformsiv( program.GetId(), uniformCount, activeIndices, UBGC_IsUniformRowMajor, layouts );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace GinInternal.

}	// namespace Gin.