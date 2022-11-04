#include <common.h>
#pragma hdrstop

#include <ObjFile.h>
#include <GlBuffer.h>
#include <BufferMapper.h>
#include <Model.h>
#include <MaterialDatabase.h>

namespace Gin {

const CStringView CObjFileException::generalObjFileError = "OBJ parsing error: %1.\r\nFile name: %0.";
//////////////////////////////////////////////////////////////////////////

CObjFile::CObjFile( CStringView _fileName, CMaterialDatabase& _materials ) :
	fileName( _fileName )
{
	CArray<BYTE> fileData;
	CFileReader file( _fileName, FCM_OpenExisting );
	const int length = file.GetLength32();
	fileData.IncreaseSizeNoInitialize( length + 1 );
	file.Read( fileData.Ptr(), fileData.Size() );
	// Add zero in the end to guarantee correct behavior for value conversion functions.
	fileData.Last() = 0;
	parseFileData( fileData.Ptr(), length, _materials );
}

const char objNamePrefix = 'o';
const char groupNamePrefix = 'g';
const char facePrefix = 'f';
const char vertexAttribFirstLetter = 'v';
const char usemtlFirstLetter = 'u';
const char mtlLibFirstLetter = 'm';
const char commentFirstLetter = '#';
const char commandDelimiter = ' ';
void CObjFile::parseFileData( const BYTE* fileData, int dataSize, CMaterialDatabase& materials )
{
	const auto objPath = FileSystem::GetDrivePath( fileName );

	// Initiate the first object with an empty name.
	namedObjects.Add( 0, CString() );

	// Parse the file data.
	int currentPos = 0;
	int lineNumber = 1;
	while( currentPos < dataSize ) {
		int nextPos;
		const int newLine = findNextLine( fileData, currentPos, dataSize, nextPos );
		switch( fileData[currentPos] ) {
			case '\r':
			case '\n':
			case commentFirstLetter:
			case 's':
				break;
			case objNamePrefix:
				parseObjectName( fileData, currentPos, lineNumber, newLine );
				break;
			case groupNamePrefix:
				parseGroupName( fileData, currentPos, lineNumber, newLine );
				break;
			case facePrefix:
				parseFace( fileData, currentPos, lineNumber, newLine );
				break;
			case vertexAttribFirstLetter:
				parseVertexAttribCommand( fileData, currentPos, lineNumber );
				break;
			case usemtlFirstLetter:
				parseUseMtl( fileData, currentPos, lineNumber, newLine, materials );
				break;
			case mtlLibFirstLetter:
				parseMtlLib( fileData, currentPos, lineNumber, newLine, materials, objPath );
				break;
			default:
				checkUnknownCommand( false, lineNumber );
		}
		currentPos = nextPos;
		lineNumber++;
	}

	// Close the final ranges.
	if( !nodesArray.IsEmpty() ) {
		nodesArray.Last().FaceRange.SetUpper( facesArray.Size() );
	}
	namedObjects.Last().NodeRange.SetUpper( nodesArray.Size() );
}

void CObjFile::parseObjectName( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos )
{
	checkUnknownCommand( fileData[dataPos + 1] == commandDelimiter, lineNumber );

	const int nameStart = dataPos + 2;
	CString name( reinterpret_cast<const char*>( fileData + nameStart ), lineEndPos - nameStart );
	namedObjects.Last().NodeRange.SetUpper( nodesArray.Size() );
	namedObjects.Add( nodesArray.Size(), move( name ) );
}

int CObjFile::findNextLine( const BYTE* fileData, int dataPos, int dataSize, int& nextPos )
{
	int currentPos = dataPos;
	for( ;; ) {
		if( currentPos >= dataSize ) {
			nextPos = currentPos;
			break;
		}
		if( fileData[currentPos] == '\n' ) {
			nextPos = currentPos + 1;
			break;
		}
		if( fileData[currentPos] == '\r' ) {
			if( currentPos + 1 < dataSize && fileData[currentPos + 1] == '\n' ) {
				nextPos = currentPos + 2;
			} else {
				nextPos = currentPos + 1;
			}
			break;
		}
		currentPos++;
	}
	return currentPos;
}

void CObjFile::parseGroupName( const BYTE* fileData, int dataPos, int lineNumber, int )
{
	checkUnknownCommand( fileData[dataPos + 1] == commandDelimiter, lineNumber );
}

const char vertexFollowup = ' ';
const char normalFollowup = 'n';
const char textureFollowup = 't';
void CObjFile::parseVertexAttribCommand( const BYTE* fileData, int dataPos, int lineNumber )
{
	switch( fileData[dataPos + 1] ) {
		case vertexFollowup:
			vertexArray.Add( getVector3AttributeValue( fileData, dataPos + 2 ) );
			break;
		case normalFollowup:
			normalArray.Add( getVector3AttributeValue( fileData, dataPos + 2 ) );
			break;
		case textureFollowup:
			textureArray.Add( getVector2AttributeValue( fileData, dataPos + 2 ) );
			break;
		default:
			checkUnknownCommand( false, lineNumber );
	}
}

CVector3<float> CObjFile::getVector3AttributeValue( const BYTE* fileData, int dataPos )
{
	CVector3<float> result;
	char* endPos;
	result.X() = strtof( reinterpret_cast<const char*>( fileData + dataPos ), &endPos );
	result.Y() = strtof( endPos + 1, &endPos );
	result.Z() = strtof( endPos + 1, nullptr );
	return result;
}

CVector2<float> CObjFile::getVector2AttributeValue( const BYTE* fileData, int dataPos )
{
	CVector2<float> result;
	char* endPos;
	result.X() = strtof( reinterpret_cast<const char*>( fileData + dataPos ), &endPos );
	result.Y() = strtof( endPos + 1, &endPos );
	return result;
}

const CStringView noMaterialFaceError = "Face command with no set material. Line %0";
const CStringView unsupportedFaceCommand = "Unsupported number of triplets. Line %0";
void CObjFile::parseFace( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos )
{
	checkUnknownCommand( fileData[dataPos + 1] == commandDelimiter, lineNumber );
	checkObjFileError( !nodesArray.IsEmpty(), noMaterialFaceError, lineNumber );

	const int tripletStart = dataPos + 2;
	
	const int firstTripletEnd = findSymbol( fileData, tripletStart, lineEndPos, commandDelimiter );
	const auto triplet1 = getFaceTriplet( fileData, tripletStart );
	const int secondTripletEnd = findSymbol( fileData, firstTripletEnd + 1, lineEndPos, commandDelimiter );
	const auto triplet2 = getFaceTriplet( fileData, firstTripletEnd + 1 );
	const int thirdTripletEnd = findSymbol( fileData, secondTripletEnd + 1, lineEndPos, commandDelimiter );
	const auto triplet3 = getFaceTriplet( fileData, secondTripletEnd + 1 );
	facesArray.Add( triplet1 );
	facesArray.Add( triplet2 );
	facesArray.Add( triplet3 );

	if( thirdTripletEnd != lineEndPos ) {
		const int fourthTripletEnd = findSymbol( fileData, thirdTripletEnd + 1, lineEndPos, commandDelimiter );
		const auto triplet4 = getFaceTriplet( fileData, thirdTripletEnd + 1 );
		facesArray.Add( triplet1 );
		facesArray.Add( triplet3 );
		facesArray.Add( triplet4 );
		checkObjFileError( fourthTripletEnd == lineEndPos, unsupportedFaceCommand, lineNumber );
	}
}

int CObjFile::findSymbol( const BYTE* fileData, int dataPos, int dataSize, char symbol )
{
	int currentPos;
	for( currentPos = dataPos; currentPos < dataSize; currentPos++ ) {
		if( fileData[currentPos] == symbol ) {
			break;
		}
	}
	return currentPos;
}

const char tripletDelimiter = '/';
CVector3<int> CObjFile::getFaceTriplet( const BYTE* fileData, int dataPos )
{
	CVector3<int> result;
	char* endPos;
	result.X() = strtol( reinterpret_cast<const char*>( fileData + dataPos ), &endPos, 10 );
	result.Y() = strtol( endPos + 1, &endPos, 10 );
	result.Z() = strtol( endPos + 1, nullptr, 10 );
	return result;
}

const CStringView useMtlCommand = "usemtl ";
const CStringView unknownMaterialError = "Unknown material referenced at line %0";
void CObjFile::parseUseMtl( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos, const CMaterialDatabase& materials )
{
	const char* fileStr = reinterpret_cast<const char*>( fileData + dataPos );
	const CStringPart mtlFilePrefix( fileStr, useMtlCommand.Length() );
	checkUnknownCommand( mtlFilePrefix == useMtlCommand, lineNumber );
	const CString materialName( fileStr + useMtlCommand.Length(), lineEndPos - useMtlCommand.Length() - dataPos );
	TMaterialConstRef newMaterial = materials.GetMaterial( materialName );
	checkObjFileError( !newMaterial.IsNull(), unknownMaterialError, lineNumber );

	if( !nodesArray.IsEmpty() ) {
		nodesArray.Last().FaceRange.SetUpper( facesArray.Size() );
	}
	nodesArray.Add( facesArray.Size(), newMaterial );
}

const CStringView mtlLibCommand = "mtllib ";
void CObjFile::parseMtlLib( const BYTE* fileData, int dataPos, int lineNumber, int lineEndPos, CMaterialDatabase& materials, CStringPart objPath )
{
	const char* fileStr = reinterpret_cast<const char*>( fileData + dataPos );
	const CStringPart mtlFilePrefix( fileStr, useMtlCommand.Length() );

	checkUnknownCommand( mtlFilePrefix == mtlLibCommand, lineNumber );
	const CStringPart libName( fileStr + mtlLibCommand.Length(), lineEndPos - mtlLibCommand.Length() - dataPos );
	checkUnknownCommand( !libName.IsEmpty(), lineNumber );
	materials.LoadFile( FileSystem::MergeName( objPath, libName ) );
}

void CObjFile::checkObjFileError( bool result, CStringView err, int lineNumber )
{
	if( !result ) {
		throw CObjFileException( fileName, err.SubstParam( lineNumber ) );
	}
}

const CStringView unknownCommandError = "Unknown command at line %0";
void CObjFile::checkUnknownCommand( bool result, int lineNumber )
{
	checkObjFileError( result, unknownCommandError, lineNumber );
}

const CStringView nameNotFoundError = "Object with the name \"%0\" not found";
CModel CObjFile::CreateModel( CStringView name ) const
{
	// Skip strings until the named object is found.
	for( const auto& namedObject : namedObjects ) {
		if( namedObject.Name == name ) {
			return createModel( namedObject.NodeRange );
		}
	}
	throw CObjFileException( fileName, nameNotFoundError.SubstParam( name ) );
}

CModel CObjFile::CreateModel() const
{
	return createModel( CInterval<int>( 0 , nodesArray.Size() ) );
}

CModel CObjFile::createModel( CInterval<int> nodeRange ) const
{
	CGlBufferOwner<BT_Array, TVector3, TVector3, TVector2> vertexAttributes;

	// Find all the unique vertex attribute triplets.
	CMap<CVector3<int>, int> uniqueTriplets;
	for( int nodePos : nodeRange ) {
		for( int facePos : nodesArray[nodePos].FaceRange ) {
			uniqueTriplets.Set( facesArray[facePos], NotFound );
		}
	}

	// Fill vertex attribute with triplets.
	vertexAttributes.ReserveBuffer( uniqueTriplets.Size(), BUH_StaticDraw );
	CVector3<float> modelSize;
	CBufferMapper( BWMM_Write, vertexAttributes, &CObjFile::fillTriplets, *this, uniqueTriplets, modelSize );
	CModel result( move( vertexAttributes ), modelSize );

	// Create nodes one by one.
	for( int nodePos : nodeRange ) {
		CGlBufferOwner<BT_ElementArray, int> indices;
		const auto faceRange = nodesArray[nodePos].FaceRange;
		indices.ReserveBuffer( faceRange.GetUpper() - faceRange.GetLower(), BUH_StaticDraw );
		CBufferMapper( BWMM_Write, indices, &CObjFile::fillIndices, *this, faceRange, uniqueTriplets );
		CModelNodeData nodeData( move( indices ) );
		nodeData.Material = nodesArray[nodePos].Material;
		const auto vertices = result.GetVertexAttributes().Typify<TVector3, TVector3, TVector2>();
		nodeData.Mesh.BindBuffer( vertices, { 0, 1, 2 } );
		result.AddNode( move( nodeData ) );
	}

	return result;
}

void CObjFile::fillTriplets( CMap<TIntVector3, int>& uniqueTriplets, TVector3& resultSize, CArrayBuffer<CTuple<TVector3, TVector3, TVector2>> mappedBuffer ) const
{
	int tripletPos = 0;
	CStackArray<CInterval<float>, 3> minMaxCoords;
	for( auto& tripletPair : uniqueTriplets ) {
		tripletPair.Value() = tripletPos;
		const auto triplet = tripletPair.Key();
		const auto vertex = vertexArray[triplet.X() - 1];
		mappedBuffer[tripletPos].Set<0>( vertex );
		mappedBuffer[tripletPos].Set<1>( normalArray[triplet.Z() - 1] );
		mappedBuffer[tripletPos].Set<2>( textureArray[triplet.Y() - 1] );
		tripletPos++;
		addMinMaxVertex( minMaxCoords[0], vertex.X() );
		addMinMaxVertex( minMaxCoords[1], vertex.Y() );
		addMinMaxVertex( minMaxCoords[2], vertex.Z() );
	}

	resultSize.X() = minMaxCoords[0].GetUpper() - minMaxCoords[0].GetLower();
	resultSize.Y() = minMaxCoords[1].GetUpper() - minMaxCoords[1].GetLower();
	resultSize.Z() = minMaxCoords[2].GetUpper() - minMaxCoords[2].GetLower();
}

void CObjFile::addMinMaxVertex( CInterval<float>& coordRange, float newCoordinate )
{
	if( coordRange.GetLower() > newCoordinate ) {
		coordRange.SetLower( newCoordinate );
	} else if( coordRange.GetUpper() < newCoordinate ) {
		coordRange.SetUpper( newCoordinate );
	}
}

void CObjFile::fillIndices( CInterval<int> faceRange, const CMap<TIntVector3, int>& uniqueTriplets, CArrayBuffer<int> mappedBuffer ) const
{
	int bufferPos = 0;
	for( int facePos : faceRange ) {
		mappedBuffer[bufferPos] = uniqueTriplets[facesArray[facePos]];
		bufferPos++;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
