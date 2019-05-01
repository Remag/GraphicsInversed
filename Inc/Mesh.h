#pragma once
#include <DrawEnums.h>
#include <GinTypes.h>
#include <GlBuffer.h>
#include <ShaderProgram.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

// Common mesh operations.
class GINAPI CMeshCommonData {
public:
	int GetMeshId() const
		{ return meshId; }

	// Bind the data from a given buffer object to the mesh.
	// locations contain all the location identifiers in the vertex shader to bind.
	// Data in buffer is assumed to interleave.
	template <class... BufferTypes>
	void BindBuffer( GinInternal::CTypedGlBufferData<BT_Array, BufferTypes...> buffer, const CStackArray<int, sizeof...( BufferTypes )>& locations );

	// Bind a runtime-defined buffer.
	void BindBuffer( CDynamicGlBuffer<BT_Array> buffer, CArrayView<int> locations );

	// Set buffer location specific parameters.
	// T
	// location - location in the vertex shader to bind.
	// offset - byte offset of the first element.
	// stride - distance that needs to be added to an element to get to the next one.
	// glType - type of the bound data.
	// innermostType - type of the data elements.
	// dataElemCount - amount of sub-elements in each buffer element.
	void BindRawBuffer( CRawGlBuffer<BT_Array> buffer, int dataElemCount, TGlType innermostType, int location, int offset = 0, int stride = 0, bool shouldNormalize = false );
	void BindRawBuffer( CRawGlBuffer<BT_Array> buffer, TGlType glType, int location, int offset = 0, int stride = 0 );

protected:
	explicit CMeshCommonData( int id ) : meshId( id ) {}

	bool checkAllAttributePresence( CShaderProgram program ) const;
	bool checkShaderAndVaoTypes( unsigned shaderType, int vaoSize ) const;

	// Common operations before and after the draw call.
	void preMeshDraw( CShaderProgram shader ) const;
	void postMeshDraw() const;

	// VAO creation and destruction.
	int createMeshId() const;
	void freeMeshId();

	// Mesh id invalidation for move operations.
	void clearMeshId()
		{ meshId = NotFound; }

	bool hasElementBinding() const;

private:
	// Id of the mesh vertex array object.
	int meshId = NotFound;

	void setVertexData( int dataElemCount, TGlType innermostType, int location, int offset, int stride, bool shouldNormalize );
	bool isLocationEnabled( int location ) const;

	template <class CurrentType, class NextType, class... Types>
	void bindBuffer( int bufferOffset, int bufferStride, const int* currentLocation );
	template <class LastType>
	void bindBuffer( int bufferOffset, int bufferStride, const int* locationsEnd );
};

// Mesh operations for specific mesh types.
template <class MeshTypeTag>
class CSpecificMeshData : public CMeshCommonData {
public:
	using CMeshCommonData::CMeshCommonData;

	typedef MeshTypeTag TMeshTag;
};

struct CRawMeshTag{};
template <>
class GINAPI CSpecificMeshData<CRawMeshTag> : public CMeshCommonData {
public:
	using CMeshCommonData::CMeshCommonData;
	template <class OtherTag>
	CSpecificMeshData( CSpecificMeshData<OtherTag> other ) : CMeshCommonData( other.GetId() ) {}

	typedef CRawMeshTag TMeshTag;
};

struct CArrayMeshTag{};
template <>
class GINAPI CSpecificMeshData<CArrayMeshTag> : public CMeshCommonData {
public:
	CSpecificMeshData( int id, TMeshDrawMode mode ) : CMeshCommonData( id ), drawMode( mode ) {}

	typedef CArrayMeshTag TMeshTag;

	TMeshDrawMode GetDrawMode() const
		{ return drawMode; }
	void SetDrawMode( TMeshDrawMode newValue )
		{ drawMode = newValue; }

	void Draw( CShaderProgram shader, int vertexCount ) const;

protected:
	void invalidate();

private:
	TMeshDrawMode drawMode = MDM_Points;
};

struct CQuadMeshTag{};
template <>
class GINAPI CSpecificMeshData<CQuadMeshTag> : public CMeshCommonData {
public:
	explicit CSpecificMeshData( int id, TMeshDrawMode mode = MDM_TriangleStrip ) : CMeshCommonData( id ), drawMode( mode ) {}

	typedef CQuadMeshTag TMeshTag;

	operator CSpecificMeshData<CArrayMeshTag>()
		{ return CSpecificMeshData<CArrayMeshTag>( GetMeshId(), drawMode ); }

	TMeshDrawMode GetDrawMode() const
		{ return drawMode; }
	void SetDrawMode( TMeshDrawMode newValue )
		{ drawMode = newValue; }

	void Draw( CShaderProgram shader ) const;

protected:
	void invalidate();

private:
	TMeshDrawMode drawMode = MDM_TriangleStrip;
};

struct CElementMeshTag{};
template <>
class GINAPI CSpecificMeshData<CElementMeshTag> : public CMeshCommonData {
public:
	CSpecificMeshData( int id, TMeshDrawMode mode ) : CMeshCommonData( id ), drawMode( mode ) {}
	CSpecificMeshData( int id, TMeshDrawMode mode, CGlBuffer<BT_ElementArray, int> buffer ) : CMeshCommonData( id ), drawMode( mode ), elementBuffer( buffer )  { initIndexData(); }

	typedef CElementMeshTag TMeshTag;
	
	TMeshDrawMode GetDrawMode() const
		{ return drawMode; }
	void SetDrawMode( TMeshDrawMode newValue )
		{ drawMode = newValue; }

	void SetIndexBuffer( CGlBuffer<BT_ElementArray, int> buffer )
		{ elementBuffer = buffer; initIndexData(); }

	void Draw( CShaderProgram shader ) const;
	void Draw( CShaderProgram shader, int elementCount ) const;

protected:
	void invalidate();

private:
	TMeshDrawMode drawMode = MDM_Points;
	CGlBuffer<BT_ElementArray, int> elementBuffer;

	void initIndexData();
};

//////////////////////////////////////////////////////////////////////////

template <class... BufferTypes>
void CMeshCommonData::BindBuffer( GinInternal::CTypedGlBufferData<BT_Array, BufferTypes...> buffer, const CStackArray<int, sizeof...( BufferTypes )>& locations )
{
	CBufferObjectBinder binder( buffer );
	bindBuffer<BufferTypes...>( 0, VarArgs::SizeOfAll<BufferTypes...>::Result, locations.Ptr() );
}

template <class CurrentType, class NextType, class... Types>
void CMeshCommonData::bindBuffer( int bufferOffset, int bufferStride, const int* currentLocation )
{
	bindBuffer<CurrentType>( bufferOffset, bufferStride, currentLocation );
	bindBuffer<NextType, Types...>( bufferOffset + sizeof( CurrentType ), bufferStride, currentLocation + 1 );
}

template <class LastType>
void CMeshCommonData::bindBuffer( int bufferOffset, int bufferStride, const int* locationsEnd )
{
	const int elemCount = GinTypes::GlType<LastType>::ElemCount;
	const TGlType innermostType = GinTypes::GlType<LastType>::InnermostGlType;
	const bool shouldNormalize = GinTypes::GlType<LastType>::ShouldNormalize;
	setVertexData( elemCount, innermostType, *locationsEnd, bufferOffset, bufferStride, shouldNormalize );
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

using CRawMesh = GinInternal::CSpecificMeshData<GinInternal::CRawMeshTag>;
using CArrayMesh = GinInternal::CSpecificMeshData<GinInternal::CArrayMeshTag>;
using CQuadMesh = GinInternal::CSpecificMeshData<GinInternal::CQuadMeshTag>;
using CElementMesh = GinInternal::CSpecificMeshData<GinInternal::CElementMeshTag>;

//////////////////////////////////////////////////////////////////////////

// Owning mesh. Creates a VAO on construction, frees it on destruction.
template <class MeshType>
class CMeshOwner : public GinInternal::CSpecificMeshData<typename MeshType::TMeshTag> {
public:
	template <class... ConstructorArgs>
	explicit CMeshOwner( ConstructorArgs&&... args ) : GinInternal::CSpecificMeshData<typename MeshType::TMeshTag>( this->createMeshId(), forward<ConstructorArgs>( args )... ) {}

	CMeshOwner( CMeshOwner&& other );
	CMeshOwner& operator=( CMeshOwner&& other );
	static CMeshOwner<MeshType> CreateRawMesh()
		{ return CMeshOwner<MeshType>( CRawCreationTag() ); }

	~CMeshOwner()
		{ this->freeMeshId(); }

	// Raw owner needs access to invalidate the moved-from mesh.
	friend class CRawMeshOwner;

private:
	struct CRawCreationTag {};
	CMeshOwner( CRawCreationTag ) : GinInternal::CSpecificMeshData<typename MeshType::TMeshTag>( NotFound, MDM_Points ) {}

	// Copying is prohibited.
	CMeshOwner( CMeshOwner& ) = delete;
	void operator=( CMeshOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <class MeshType>
CMeshOwner<MeshType>::CMeshOwner( CMeshOwner<MeshType>&& other ) :
	GinInternal::CSpecificMeshData<typename MeshType::TMeshTag>( move( other ) )
{
	other.invalidate();
}

template <class MeshType>
CMeshOwner<MeshType>& CMeshOwner<MeshType>::operator=( CMeshOwner<MeshType>&& other )
{
	this->freeMeshId();
	GinInternal::CSpecificMeshData<typename MeshType::TMeshTag>::operator=( move( other ) );
	other.invalidate();
	return *this;
}

//////////////////////////////////////////////////////////////////////////

// VAO identifier owner without the mesh information.
class CRawMeshOwner : public GinInternal::CMeshCommonData {
public:
	CRawMeshOwner() : CMeshCommonData( this->createMeshId() ) {}

	template <class MeshType>
	explicit CRawMeshOwner( CMeshOwner<MeshType>&& other );
	CRawMeshOwner( CRawMeshOwner&& other );
	CRawMeshOwner& operator=( CRawMeshOwner&& other );
	static CRawMeshOwner CreateRawMesh()
		{ return CRawMeshOwner( CRawCreationTag() ); }

	~CRawMeshOwner()
		{ this->freeMeshId(); }

private:
	struct CRawCreationTag {};
	CRawMeshOwner( CRawCreationTag ) : CMeshCommonData( NotFound ) {}

	// Copying is prohibited.
	CRawMeshOwner( CRawMeshOwner& ) = delete;
	void operator=( CRawMeshOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <class MeshType>
CRawMeshOwner::CRawMeshOwner( CMeshOwner<MeshType>&& other ) :
	CMeshCommonData( other.GetMeshId() )
{
	other.invalidate();
}

inline CRawMeshOwner::CRawMeshOwner( CRawMeshOwner&& other ) :
	CMeshCommonData( move( other ) )
{
	other.clearMeshId();
}

inline CRawMeshOwner& CRawMeshOwner::operator=( CRawMeshOwner&& other )
{
	this->freeMeshId();
	CMeshCommonData::operator=( move( other ) );
	other.clearMeshId();
	return *this;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

