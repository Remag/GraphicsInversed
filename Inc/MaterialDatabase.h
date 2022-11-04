#pragma once
#include <GinDefs.h>
#include <GinComponents.h>

namespace Gin {

// Illumination model that is supposed to be used when rendering the given material.
enum TIlluminationModel {
	IM_NoAmbient,
	IM_DiffuseOnly,
	IM_DiffuseSpecular
};

//////////////////////////////////////////////////////////////////////////

// Exception occurred while trying to extract data from an MTL file.
class GINAPI CMtlFileException : public CFileWrapperException {
public:
	CMtlFileException( CStringPart fileName, CStringPart additionalInfo ) : CFileWrapperException( fileName, additionalInfo ) {}

	virtual CString GetMessageTemplate() const override
		{ return Str( generalMtlFileError ); }
	
private:
	static const CStringView generalMtlFileError;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for creating materials from a given .mtl file.
class GINAPI CMaterialDatabase {
public:
	CMaterialDatabase() = default;
	~CMaterialDatabase();

	// Clear all the loaded data.
	void Reset();

	// Load all material data from the given file.
	void LoadFile( CString fileName );

	// Get or create a material reference with the given name.
	TMaterialRef GetOrCreateMaterial( CStringView materialName );

	// Get a given material. Return a null reference if it is not loaded in the database.
	TMaterialConstRef GetMaterial( CStringView materialName ) const;

private:
	// Set of loaded files.
	CHashTable<CString> loadedFiles;
	// Relation between material names and materials.
	CMap<CString, TMaterialOwner> loadedMaterials;

	void createMaterialData( CStringView fileName );
	void parseNewMaterial( TMaterialOwner& currentMtl, CString& currentName, CStringPart mtlStr, CStringView fileName );
	void addNewMaterial( TMaterialOwner& currentMtl, CString& currentName, CStringView fileName );
	void parseMtlColor( TMaterialOwner& currentMtl, CStringPart mtlStr, CStringView fileName );
	void parseMtlExponent( TMaterialOwner& currentMtl, CStringPart mtlStr, CStringView fileName );
	void parseMtlDissolve( TMaterialOwner& currentMtl, CStringPart mtlStr, CStringView fileName );
	void parseMtlIlluminationModel( TMaterialOwner& currentMtl, CStringPart mtlStr, CStringView fileName );
	void checkMtl( bool condition, CStringPart mtlStr, CStringView fileName );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

