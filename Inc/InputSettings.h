#pragma once
#include <InputUtils.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

enum TKeyModifierType {
	KMT_None,
	KMT_Press,
	KMT_Release,
	KMT_Hotkey,
	KMT_EnumCount
};

// A set of pressed or released keys.
struct CKeyCombination {
	// Main key virtual code.
	int MainKey = 0;
	// Optional key that is required to be pressed when the main key code is triggered.
	int AdditionalKey = 0;
	// Explicit key modifier if preset.
	TKeyModifierType KeyModifier = KMT_None;

	CKeyCombination() = default;
	CKeyCombination( int mainKey, int additionalKey, TKeyModifierType keyModifier ) : MainKey( mainKey ), AdditionalKey( additionalKey ), KeyModifier( keyModifier ) {}
};

// Raw data that is used by input settings to create an input translator.
struct CInputTranslatorData {
	CUnicodeString Name;
	CArray<CPair<CKeyCombination, CUnicodeString>> KeyActionPairs;

	CInputTranslatorData() = default;
	CInputTranslatorData( const CInputTranslatorData& other, const CExplicitCopyTag& ) : Name( copy( other.Name ) ), KeyActionPairs( copy( other.KeyActionPairs ) ) {}
};

//////////////////////////////////////////////////////////////////////////

// File with input settings.
// A file is a number of lists of key value pairs separated by equality sign.
// Lists are separated by named section in brackets, each section is a translator name.
// The first list may have no section name, it represents keys common to all translator.
// A key represents a combination of pressed or released keys on a keyboard.
// A value is an external action name.
// Translators are created and filled in constructor.
// All changes are automatically saved in destructor.
class GINAPI CInputSettings {
public:
	explicit CInputSettings( CUnicodeView fileName );
	~CInputSettings();

	bool IsModified() const
		{ return isModified; }

	static CArray<CPair<CUnicodeView, int>> CreateKeyNamePairs();
	static CArray<CUnicodeView> CreateCodeToKeyNameArray();
	static CMap<CUnicodeView, int, CIniKeyHashStrategy> CreateKeyNameToCodeMap();
	
	// Get a string representation of the given key.
	static CUnicodeString GetKeyName( int keyCode );
	// Get a string representation of the given code and the key pressed state.
	static CUnicodeString GetFullKeyName( int keyCode, bool isDown );
	// Get the key virtual code given its string representation.
	static int GetKeyCode( CUnicodePart keyName );
	static COptional<CKeyCombination> ParseKeyCombination( CUnicodePart combinationStr );

	// Settings export and import for rollback possibility.
	CArray<CInputTranslatorData> ExportSettings() const;
	void ImportSettings( CArray<CInputTranslatorData> data );

	// Full path to the initialization file.
	CUnicodeView Name() const
		{ return fileName; }

	// Save changes to the input file.
	void Save();

	const CInputTranslator& GetTranslator( CUnicodePart name ) const;

	CArrayView<CPair<CKeyCombination, CUnicodeString>> GetKeyValuePairs( CUnicodePart translatorName ) const;

	// Action manipulation.
	void DeleteAllActions( int keyCode, bool isDown, CUnicodePart translatorName );
	void DeleteSingleAction( int keyCode, bool isDown, CUnicodePart actionName, CUnicodePart translatorName );
	void AddAction( int keyCode, bool isDown, CUnicodePart actionName, CUnicodePart translatorName );

private:
	CUnicodeString fileName;
	CArray<CInputTranslatorData> fileData;

	// Section name to translator dictionary.
	CMap<CUnicodeString, CInputTranslator> translatorTable;
	// Has the translator table been modified.
	bool isModified = false;

	// Dictionaries with key string representations.
	static CArray<CPair<CUnicodeView, int>> keyNameCodePairs;
	static CMap<CUnicodeView, int, CIniKeyHashStrategy> keyNameToCode;
	static CArray<CUnicodeView> codeToKeyName;

	CArray<CInputTranslatorData> parseCfgFile() const;
	bool shouldSkip( CUnicodePart str ) const;
	bool isSectionName( CUnicodePart str ) const;
	CUnicodePart parseSectionName( CUnicodePart str ) const;
	void parseKeyValuePair( CUnicodePart str, CInputTranslatorData& result ) const;
	static int findEndCommentPos( CUnicodePart str );
	void unknownStrWarning( CUnicodePart str ) const;

	void initializeTranslators();
	void fillTranslator( const CInputTranslatorData& source, CInputTranslator& translator ) const;

	void createActionFromData( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const;
	bool tryCreateUserHotkey( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const;
	bool tryCreateUserAction( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const;
	void createUserHotkey( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const;
	void createUserAction( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const;
	void addTranslatorHotkey( CKeyCombination key, CPtrOwner<TUserAction> action, CInputTranslator& translator ) const;
	void addTranslatorAction( CKeyCombination key, CPtrOwner<TUserAction> action, CInputTranslator& translator ) const;

	static void deleteEqualKeys( CInputTranslatorData& target, int pos, CKeyCombination key );
	static void restoreInputAction( CInputTranslatorData& target, int pos, CKeyCombination key, CUnicodePart actionName, CPtrOwner<TUserAction>& result );
	static void deleteActionPart( CKeyCombination& targetKey, CUnicodeString& targetName, bool deleteDown );
	CInputTranslatorData& findTranslatorData( CUnicodePart name );
	const CInputTranslatorData& findTranslatorData( CUnicodePart name ) const
		{ return const_cast<CInputSettings*>( this )->findTranslatorData( name ); }

	static CPtrOwner<TUserAction> filterSpecificAction( CPtrOwner<TUserAction> action, CKeyCombination keyToFilter );
	static CPtrOwner<TUserAction> mergeActions( CPtrOwner<TUserAction> newAction, CPtrOwner<TUserAction> oldAction );
	static CPtrOwner<TUserAction> createNewAction( CUnicodePart name, CKeyCombination key );
	static void addActionData( CKeyCombination key, CUnicodePart action, CInputTranslatorData& targetData );

	static CUnicodeString createSectionNameStr( CUnicodeView section );
	static void saveKeyActionPairs( CInputTranslatorData& data, CUnicodeString& result );
	static void saveKeyActionPair( CKeyCombination key, CUnicodeView action, CUnicodeString& result );

	static CKeyCombination createKeyCombination( int mainKey, TKeyModifierType modifier, int additionalKey );
	static CUnicodeString getFullKeyString( int code, bool isDown );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

