#pragma once
#include <InputUtils.h>
#include <ControlScheme.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Raw data that is used to create an input translator.
struct CInputTranslatorData {
	CString Name;
	CArray<CPair<CKeyCombination, CString>> KeyActionPairs;

	CInputTranslatorData() = default;
	CInputTranslatorData( const CInputTranslatorData& other, const CExplicitCopyTag& ) : Name( copy( other.Name ) ), KeyActionPairs( copy( other.KeyActionPairs ) ) {}
};

//////////////////////////////////////////////////////////////////////////

// File with input settings.
// A file is a number of lists of key value pairs separated by equality sign.
// Lists are separated by named section in brackets, each section is a control scheme name.
// A key represents a combination of pressed or released keys on a keyboard.
// A value is an external action name.
// All changes are automatically saved in destructor.
class GINAPI CInputSettings {
public:
	explicit CInputSettings( CUnicodeView fileName );
	~CInputSettings();

	static CArray<CPair<CStringView, TGinVirtualKey>> CreateKeyNamePairs();
	static CArray<CStringView> CreateCodeToKeyNameArray();
	static CMap<CStringView, TGinVirtualKey, CCaselessStringHash> CreateKeyNameToCodeMap();
	
	// Get a string representation of the given key.
	static CString GetKeyName( int keyCode );
	// Get a string representation of the given code and the key pressed state.
	static CString GetFullKeyName( CKeyCombination keyCombination );
	// Get the key virtual code given its string representation.
	static TGinVirtualKey GetKeyCode( CStringPart keyName );
	static COptional<CKeyCombination> ParseKeyCombination( CStringPart combinationStr );

	bool IsModified() const
	{ return isModified; }

	// Full path to the initialization file.
	CUnicodeView GetName() const
		{ return fileName; }

	// Save changes to the input file.
	void Save();

	const CInputTranslator& GetInputTranslator( CStringPart name ) const;

	// Delete all key bindings.
	void Empty( CStringPart segmentName );
	// Try setting the given action, using the key from file or default key, if no key was found. Returns the final key combination for the given action.
	// Primary file action is the first action in the list.
	CKeyCombination RegisterPrimaryFileAction( CKeyCombination defaultKey, CStringPart actionName, CStringPart segmentName );
	// Secondary file action is the second action in the list.
	CKeyCombination RegisterSecondaryFileAction( CKeyCombination defaultKey, CStringPart actionName, CStringPart segmentName );
	void AddBasicAction( CKeyCombination keyCombination, CPtrOwner<TUserAction> action, CStringPart segmentName );

private:
	CUnicodeString fileName;

	CArray<CInputTranslatorData> fileData;
	// Section name to translator dictionary.
	CMap<CString, CControlScheme> translatorTable;
	CInputTranslator emptyTranslator;
	// Has the translator table been modified.
	bool isModified = false;

	// Dictionaries with key string representations.
	static CArray<CPair<CStringView, TGinVirtualKey>> keyNameCodePairs;
	static CMap<CStringView, TGinVirtualKey, CCaselessStringHash> keyNameToCode;
	static CArray<CStringView> codeToKeyName;

	CArray<CInputTranslatorData> parseCfgFile() const;
	bool shouldSkip( CStringPart str ) const;
	bool isSectionName( CStringPart str ) const;
	CStringPart parseSectionName( CStringPart str ) const;
	void parseKeyValuePair( CStringPart str, CInputTranslatorData& result ) const;
	static int findEndCommentPos( CStringPart str );
	void unknownStrWarning( CStringPart str ) const;

	void tryCreateActionFromData( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const;
	void createActionFromData( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const;
	bool tryCreateUserAction( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const;
	CPtrOwner<TUserAction> createInputAction( CUnicodePart actionName ) const;
	void createUserAction( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const;
	void addControlSchemeAction( CKeyCombination key, CPtrOwner<TUserAction> action, CControlScheme& controlScheme ) const;

	void createDynamicFileAction( CKeyCombination keyCombination, CStringPart actionName, CInputTranslatorData& translatorData );

	CInputTranslatorData& findTranslatorData( CStringPart name );
	const CInputTranslatorData& findTranslatorData( CStringPart name ) const
		{ return const_cast<CInputSettings*>( this )->findTranslatorData( name ); }

	static CString createSectionNameStr( CStringPart section );
	static void saveKeyActionPairs( CInputTranslatorData& data, CString& result );
	static void saveKeyActionPair( CKeyCombination key, CStringPart action, CString& result );

	static CKeyCombination createKeyCombination( TGinVirtualKey mainKey, TKeyModifierType modifier, TGinVirtualKey additionalKey );
	static bool isSameKey( CKeyCombination left, CKeyCombination right );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

