#pragma once
#include <GinDefs.h>
#include <GinComponents.h>
#include <Mesh.h>

namespace Gin {

class CMaterialDatabase;
class CModel;
//////////////////////////////////////////////////////////////////////////

// Exception occurred while trying to extract data from an OBJ file.
class GINAPI CObjFileException : public CFileWrapperException {
public:
	CObjFileException( CStringPart fileName, CStringPart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}

	virtual CString GetMessageTemplate() const override
		{ return Str( generalObjFileError ); }
	
private:
	static const CStringView generalObjFileError;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for creating models from a given .obj file.
class GINAPI CObjFile {
public:
	typedef CVector3<float> TVector3;
	typedef CVector3<int> TIntVector3;
	typedef CVector2<float> TVector2;

	CObjFile( CStringView fileName, CMaterialDatabase& materials );

	// Create a model from a single named object in the file.
	CModel CreateModel( CStringView name ) const;
	// Create a single model from the whole file. Object names are ignored.
	CModel CreateModel() const;
	 
private:
	const CString fileName;
	// File is preloaded and separated into typified arrays.
	CArray<TVector3> vertexArray;
	CArray<TVector3> normalArray;
	CArray<TVector2> textureArray;
	CArray<TIntVector3> facesArray;

	struct CNodeData {
		TMaterialConstRef Material;
		CInterval<int> FaceRange;

		CNodeData( int lowerFaceRange, TMaterialConstRef material ) : Material( material ) { FaceRange.SetLower( lowerFaceRange ); }
	};
	CArray<CNodeData> nodesArray;

	struct CNamedObjectData {
		CString Name;
		// Nodes of the named object.
		CInterval<int> NodeRange;

		CNamedObjectData( int lowerNodeRange, CString name ) : Name( move( name ) ) { NodeRange.SetLower( lowerNodeRange ); }
	};
	CArray<CNamedObjectData> namedObjects;

	static int findNextLine( const BYTE* fileData, int dataPos, int dataSize, int& nextPos );
	static int findSymbol( const BYTE* fileData, int dataPos, int dataSize, char symbol );
	void parseFileData( const BYTE* fileData, int dataSize, CMaterialDatabase& materials );
	void parseObjectName( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos );
	void parseGroupName( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos );
	void parseVertexAttribCommand( const BYTE* fileData, int dataPos, int lineNumber );
	CVector3<float> getVector3AttributeValue( const BYTE* fileData, int dataPos );
	CVector2<float> getVector2AttributeValue( const BYTE* fileData, int dataPos );
	void parseFace( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos );
	CVector3<int> getFaceTriplet( const BYTE* fileData, int dataPos );
	void parseUseMtl( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos, const CMaterialDatabase& materials );
	void parseMtlLib( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos, CMaterialDatabase& materials, CStringPart objPath );

	void checkObjFileError( bool result, CStringView err, int lineNumber );
	void checkUnknownCommand( bool result, int lineNumber );

	CModel createModel( CInterval<int> nodeRange ) const;
	void fillTriplets( CMap<TIntVector3, int>& uniqueTriplets, TVector3& resultSize, CArrayBuffer<CTuple<TVector3, TVector3, TVector2>> mappedBuffer ) const;
	static void addMinMaxVertex( CInterval<float>& coordRange, float newCoordinate );
	void fillIndices( CInterval<int> faceRange, const CMap<TIntVector3, int>& uniqueTriplets, CArrayBuffer<unsigned> mappedBuffer ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

