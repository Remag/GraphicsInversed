#include <common.h>
#pragma hdrstop

#include <Mesh.h>
#include <ShaderProgram.h>
#include <GinTypes.h>

namespace Gin {

namespace GinInternal {

void CMeshCommonData::BindBuffer( CDynamicGlBuffer<BT_Array> buffer, CArrayView<int> locations )
{
	CBufferObjectBinder binder( buffer );
	CArrayView<TGlType> glTypes = buffer.GetTypeInfo();
	assert( glTypes.Size() == locations.Size() );
	int stride = 0;
	for( auto type : glTypes ) {
		stride += GinTypes::GetGlTypeInformation( type ).ByteSize;
	}
	int currentOffset = 0;
	for( int i = 0; i < locations.Size(); i++ ) {
		const auto typeInfo = GinTypes::GetGlTypeInformation( glTypes[i] );
		setVertexData( typeInfo.ElemCount, typeInfo.InnerMostType, locations[i], currentOffset, stride, typeInfo.ShouldNormalize );
		currentOffset += typeInfo.ByteSize;
	}
}

void CMeshCommonData::BindRawBuffer( CRawGlBuffer<BT_Array> buffer, int dataElemCount, TGlType glType, int location, int offset /*= 0*/, int stride /*= 0*/, bool shouldNormalize /*= false */ )
{
	CBufferObjectBinder binder( buffer );
	setVertexData( dataElemCount, glType, location, offset, stride, shouldNormalize );
}

void CMeshCommonData::BindRawBuffer( CRawGlBuffer<BT_Array> buffer, TGlType glType, int location, int offset /*= 0*/, int stride /*= 0 */ )
{
	const auto typeInfo = GinTypes::GetGlTypeInformation( glType );
	BindRawBuffer( buffer, typeInfo.ElemCount, typeInfo.InnerMostType, location, offset, stride, typeInfo.ShouldNormalize );
}

void CMeshCommonData::setVertexData( int dataElemCount, TGlType innermostType, int location, int offset, int stride, bool shouldNormalize )
{
	const BYTE normalizeValue = shouldNormalize ? numeric_cast<BYTE>( GB_True ) : numeric_cast<BYTE>( GB_False );
	gl::BindVertexArray( meshId );
	assert( !isLocationEnabled( location ) );

	gl::EnableVertexAttribArray( location );
	gl::VertexAttribPointer( location, dataElemCount, innermostType, normalizeValue, stride, reinterpret_cast<void*>( offset ) );
	gl::BindVertexArray( 0 );
	CheckGlError();
}

// Check that the given location has no defined attributes.
bool CMeshCommonData::isLocationEnabled( int location ) const
{
	int result;
	gl::GetVertexAttribiv( location, gl::VERTEX_ATTRIB_ARRAY_ENABLED, &result );
	CheckGlError();
	return result != 0;
}

bool CMeshCommonData::checkAllAttributePresence( CShaderProgram program ) const
{
	assert( program.IsLinked() );

	int programAttributeCount;
	gl::GetProgramiv( program.GetId(), gl::ACTIVE_ATTRIBUTES, &programAttributeCount );
	int maxAttributeLength;
	gl::GetProgramiv( program.GetId(), gl::ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength );

	CString attribName;
	attribName.ReserveBuffer( maxAttributeLength );

	for( int i = 0; i < programAttributeCount; i++ ) {
		// Get the information from the shader.
		int attributeSize;
		GLenum shaderType;
		auto nameBuffer = attribName.CreateRawBuffer( maxAttributeLength );
		int bufferSize;
		gl::GetActiveAttrib( program.GetId(), i, maxAttributeLength, &bufferSize, &attributeSize, &shaderType, nameBuffer );
		nameBuffer.Release( bufferSize );
		int location = gl::GetAttribLocation( program.GetId(), attribName.Ptr() );

		// All active attributes must be enabled in the VAO.
		if( !isLocationEnabled( location ) ) {
			return false;
		}

		// Type in the shader must correspond to the type defined in the VAO.
		int definedSize;
		gl::GetVertexAttribiv( location, gl::VERTEX_ATTRIB_ARRAY_SIZE, &definedSize );
		if( !checkShaderAndVaoTypes( shaderType, definedSize ) ) {
			return false;
		}
	}
	CheckGlError();
	return true;
}

bool CMeshCommonData::checkShaderAndVaoTypes( GLenum shaderType, int vaoSize ) const
{
	const GinTypes::CGlTypeInfo& info = GinTypes::GetGlTypeInformation( TGlType( shaderType ) );

	return info.ElemCount == vaoSize;
}

bool CMeshCommonData::hasElementBinding() const
{
	int elementObjectId;
	gl::GetIntegerv( gl::ELEMENT_ARRAY_BUFFER_BINDING , &elementObjectId );
	CheckGlError();
	return elementObjectId != 0;
}

void CMeshCommonData::preMeshDraw( CShaderProgram shader ) const
{
	assert( meshId != NotFound );
	assert( CShaderProgramSwitcher::GetCurrentShaderProgram().GetId() == shader.GetId() );
	assert( shader.CheckUniformsSet() );
	shader;

	gl::BindVertexArray( meshId );
	debug_assert( checkAllAttributePresence( shader ) );
}

void CMeshCommonData::postMeshDraw() const
{
	gl::BindVertexArray( 0 );
	CheckGlError();
}

int CMeshCommonData::createMeshId() const
{
	assert( GetGlContextManager().HasContext() );
	GLuint newId;
	gl::GenVertexArrays( 1, &newId );
	return newId;
}

void CMeshCommonData::freeMeshId()
{
	if( meshId != NotFound ) {
		assert( GetGlContextManager().HasContext() );
		GLuint deleteId = meshId;
		gl::DeleteVertexArrays( 1, &deleteId );
	}
}

//////////////////////////////////////////////////////////////////////////

void CSpecificMeshData<CArrayMeshTag>::Draw( CShaderProgram shader, int vertexCount ) const
{
	preMeshDraw( shader );
	gl::DrawArrays( drawMode, 0, vertexCount );
	postMeshDraw();
}

void CSpecificMeshData<CArrayMeshTag>::invalidate()
{
	clearMeshId();
	drawMode = MDM_Points;
}

void CSpecificMeshData<CQuadMeshTag>::Draw( CShaderProgram shader ) const
{
	preMeshDraw( shader );
	gl::DrawArrays( drawMode, 0, 4 );
	postMeshDraw();
}

void CSpecificMeshData<CQuadMeshTag>::invalidate()
{
	clearMeshId();
	drawMode = MDM_Points;
}

void CSpecificMeshData<CElementMeshTag>::Draw( CShaderProgram shader ) const
{
	const int elemCount = elementBuffer.ElemCount();
	preMeshDraw( shader );
	assert( hasElementBinding() );
	gl::DrawElements( drawMode, elemCount, GLT_UnsignedInt, 0 );
	postMeshDraw();
}

void CSpecificMeshData<CElementMeshTag>::Draw( CShaderProgram shader, int elementCount ) const
{
	assert( elementBuffer.ElemCount() >= elementCount );
	preMeshDraw( shader );
	assert( hasElementBinding() );
	gl::DrawElements( drawMode, elementCount, GLT_UnsignedInt, 0 );
	postMeshDraw();
}

void CSpecificMeshData<CElementMeshTag>::invalidate()
{
	clearMeshId();
	elementBuffer = CGlBuffer<BT_ElementArray, int>();
}

void CSpecificMeshData<CElementMeshTag>::initIndexData()
{
	gl::BindVertexArray( GetMeshId() );
	CBufferObjectBinder binder( elementBuffer );
	gl::BindVertexArray( 0 );
	CheckGlError();
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
