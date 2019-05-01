#include <common.h>
#pragma hdrstop

#include <MaterialDatabase.h>
#include <GinComponents.h>

namespace Gin {

const CUnicodeView CMtlFileException::generalMtlFileError = L"MTL error while parsing string %1.\r\nFile name: %0.";
//////////////////////////////////////////////////////////////////////////

CMaterialDatabase::~CMaterialDatabase()
{
	Reset();
}

void CMaterialDatabase::Reset()
{
	loadedFiles.Empty();
	loadedMaterials.Empty();
}

void CMaterialDatabase::LoadFile( CUnicodeString fileName )
{
	if( loadedFiles.HasValue( fileName ) ) {
		return;
	}
	createMaterialData( fileName );
	loadedFiles.Set( move( fileName ) );
}

static const CStringView newMtlName = "newmtl ";
static const CStringView diffuseColorName = "Kd ";
static const CStringView specularColorName = "Ks ";
static const CStringView specularExpName = "Ns ";
static const CStringView ambientColorName = "Ka ";
static const CStringView illumModelName = "illum ";
static const char commentLetter = '#';
static const char newMtlLetter = 'n';
static const char colorLetter = 'K';
static const char expLetter = 'N';
static const char dissolveLetter = 'd';
static const char illumModelLetter = 'i';
void CMaterialDatabase::createMaterialData( CUnicodeView fileName )
{
	CString fileContents = File::ReadText( fileName );

	CString currentMtlName;
	TMaterialOwner currentMat;
	for( auto str : fileContents.SplitByAction( SplitLinesAction ) ) {
		if( str.IsEmpty() ) {
			continue;
		}
		switch( str.First() ) {
			case commentLetter:
				break;
			case newMtlLetter:
				parseNewMaterial( currentMat, currentMtlName, str, fileName );
				break;
			case colorLetter:
				parseMtlColor( currentMat, str, fileName );
				break;
			case expLetter:
				parseMtlExponent( currentMat, str, fileName );
				break;
			case dissolveLetter:
				parseMtlDissolve( currentMat, str, fileName );
				break;
			case illumModelLetter:
				parseMtlIlluminationModel( currentMat, str, fileName );
				break;
		}
	}
	addNewMaterial( currentMat, currentMtlName, fileName );
}

void CMaterialDatabase::parseNewMaterial( TMaterialOwner& currentMtl, CString& currentName, CStringPart mtlStr, CUnicodeView fileName )
{
	checkMtl( mtlStr.HasPrefix( newMtlName ), mtlStr, fileName );
	if( !currentMtl.IsNull() ) {
		addNewMaterial( currentMtl, currentName, fileName );
	}

	assert( currentMtl.IsNull() );
	assert( currentName.IsEmpty() );
	currentName = mtlStr.Mid( newMtlName.Length() );
	currentMtl = CreateInlineEntity<Material::ComponentClass>();
}

void CMaterialDatabase::addNewMaterial( TMaterialOwner& currentMtl, CString& currentName, CUnicodeView fileName )
{
	assert( !currentName.IsEmpty() );
	checkMtl( !loadedMaterials.Has( currentName ), currentName, fileName );
	loadedMaterials.Add( move( currentName ), move( currentMtl ) );
}

static const char diffuseColorLetter = 'd';
static const char specularColorLetter = 's';
static const char ambientColorLetter = 'a';
void CMaterialDatabase::parseMtlColor( TMaterialOwner& currentMtl, CStringPart mtlStr, CUnicodeView fileName )
{
	const Material::CMaterialComponent<CVector3<float>>* targetComponent = nullptr;
	switch( mtlStr[1] ) {
		case diffuseColorLetter:
			targetComponent = &Material::DiffuseColor;
			break;
		case specularColorLetter:
			targetComponent = &Material::SpecularColor;
			break;
		case ambientColorLetter:
			targetComponent = &Material::AmbientColor;
			break;
		default:
			checkMtl( false, mtlStr, fileName );
	}

	assert( targetComponent != nullptr );
	const auto colorStr = mtlStr.Mid( diffuseColorName.Length() );
	const auto colorValue = Value<float, 3>( colorStr, ' ' );
	checkMtl( colorValue.IsValid(), mtlStr, fileName );
	checkMtl( !currentMtl.HasValue( *targetComponent ), mtlStr, fileName );
	currentMtl.SetValue( *targetComponent, SRGBToLinear( *colorValue ) );
}

void CMaterialDatabase::parseMtlExponent( TMaterialOwner& currentMtl, CStringPart mtlStr, CUnicodeView fileName )
{
	const auto expStr = mtlStr.Mid( specularExpName.Length() );
	const auto expValue = Value<float>( expStr );
	checkMtl( expValue.IsValid(), mtlStr, fileName );
	checkMtl( !currentMtl.HasValue( Material::SpecularExponent ), mtlStr, fileName );
	currentMtl.SetValue( Material::SpecularExponent, *expValue );
}

void CMaterialDatabase::parseMtlDissolve( TMaterialOwner& currentMtl, CStringPart mtlStr, CUnicodeView fileName )
{
	const auto dissolveStr = mtlStr.Mid( 2 );
	const auto dissolveValue = Value<float>( dissolveStr );
	checkMtl( dissolveValue.IsValid(), mtlStr, fileName );
	checkMtl( !currentMtl.HasValue( Material::DissolveValue ), mtlStr, fileName );
	currentMtl.SetValue( Material::DissolveValue, *dissolveValue );
}

void CMaterialDatabase::parseMtlIlluminationModel( TMaterialOwner& currentMtl, CStringPart mtlStr, CUnicodeView fileName )
{
	const auto modelStr = mtlStr.Mid( illumModelName.Length() );
	const auto modelIntValue = Value<int>( modelStr );
	checkMtl( modelIntValue.IsValid(), mtlStr, fileName );
	const TIlluminationModel modelValue = TIlluminationModel( *modelIntValue );
	checkMtl( !currentMtl.HasValue( Material::IlluminationModel ), mtlStr, fileName );
	currentMtl.SetValue( Material::IlluminationModel, modelValue );
}

void CMaterialDatabase::checkMtl( bool condition, CStringPart mtlStr, CUnicodeView fileName )
{
	if( !condition ) {
		throw CMtlFileException( fileName, UnicodeStr( mtlStr ) );
	}
}

TMaterialRef CMaterialDatabase::GetOrCreateMaterial( CStringView materialName )
{
	auto& result = loadedMaterials.GetOrCreate( materialName ).Value();
	if( result.IsNull() ) {
		result = CreateInlineEntity<Material::ComponentClass>();
	}

	return result;
}

TMaterialConstRef CMaterialDatabase::GetMaterial( CStringView materialName ) const
{
	auto result = loadedMaterials.Get( materialName );
	return result != nullptr ? *result : TMaterialConstRef();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
