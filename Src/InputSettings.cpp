#include <common.h>
#pragma hdrstop

#include <InputSettings.h>
#include <InputHandler.h>
#include <GinGlobals.h>
#include <ControlScheme.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CInputSettings::CInputSettings( CUnicodeView _fileName ) :
	fileName( _fileName ),
	fileData( parseCfgFile() )
{
}

CArray<CInputTranslatorData> CInputSettings::parseCfgFile() const
{
	CArray<CInputTranslatorData> result;
	CFileReader settingsReader( fileName, FCM_CreateOrOpen );
	const auto fileStr = File::ReadText( settingsReader );

	CInputTranslatorData* currentData = &result.Add();
	for( auto str : fileStr.Split( L'\n' ) ) {
		const auto trimmedStr = str.TrimSpaces();
		if( shouldSkip( trimmedStr ) ) {
			continue;
		} else if( isSectionName( trimmedStr ) ) {
			currentData = &result.Add();
			currentData->Name = parseSectionName( trimmedStr );
		} else {
			parseKeyValuePair( trimmedStr, *currentData );
		}
	}

	return result;
}

const auto commentSymbol = '/';
bool CInputSettings::shouldSkip( CStringPart str ) const
{
	const bool isComment = str.Length() >= 2 && str[0] == commentSymbol && str[2] == commentSymbol;
	return str.IsEmpty() || isComment;
}

bool CInputSettings::isSectionName( CStringPart str ) const
{
	return str[0] == '[';
}

const CUnicodeView unknownConfigStr = L"Invalid string in configuration file: %0.\r\nFile name: %1";
CStringPart CInputSettings::parseSectionName( CStringPart str ) const
{
	const auto endCommentPos = findEndCommentPos( str );
	auto endPos = str.Find( L']', 1 );
	if( endPos == NotFound || endPos > endCommentPos ) {
		unknownStrWarning( str );
		endPos = endCommentPos;
	}

	return str.Mid( 1, endPos - 1 ).TrimSpaces();
}

void CInputSettings::unknownStrWarning( CStringPart str ) const
{
	Log::Warning( unknownConfigStr.SubstParam( str, fileName ), this );
}

int CInputSettings::findEndCommentPos( CStringPart str )
{
	const int length = str.Length();
	for( int i = str.Find( commentSymbol ); i != NotFound; i = str.Find( commentSymbol, i + 1 ) ) {
		const int nextPos = i + 1;
		if( nextPos < length && str[nextPos] == commentSymbol ) {
			return i;
		}
	}

	return length;
}

void CInputSettings::parseKeyValuePair( CStringPart str, CInputTranslatorData& result ) const
{
	const auto endCommentPos = findEndCommentPos( str );
	const auto separatorPos = str.Find( L'=' );
	if( separatorPos == NotFound || separatorPos > endCommentPos ) {
		unknownStrWarning( str );
		return;
	}

	const auto keyName = str.Left( separatorPos ).TrimSpaces();
	const auto newKey = ParseKeyCombination( keyName );
	if( !newKey.IsValid() ) {
		unknownStrWarning( str );
		return;
	}

	const auto actionName = str.Mid( separatorPos + 1, endCommentPos - separatorPos - 1 ).TrimSpaces();
	result.KeyActionPairs.Add( *newKey, actionName );
}

CInputSettings::~CInputSettings()
{
	Save();
}

CArray<CPair<CStringView, TGinVirtualKey>> CInputSettings::CreateKeyNamePairs()
{
	static CArrayView<CPair<CStringView, TGinVirtualKey>> keyValuePairs {
		{ "NoKey", GVK_Null },
		{ "Null", GVK_Null },
		{ "LMouse", GVK_LMouse },
		{ "RMouse", GVK_RMouse },
		{ "MMouse", GVK_MMouse },
		{ "Mouse4", GVK_Mouse4 },
		{ "Mouse5", GVK_Mouse5 },
		{ "LDClick", GVK_LDClick },
		{ "RDClick", GVK_RDClick },
		{ "MDClick", GVK_MDClick },
		{ "WheelDown", GVK_WheelDown },
		{ "WheelUp", GVK_WheelUp },
		{ "Esc", GVK_Escape },
		{ "Space", GVK_Space },
		{ "LShift", GVK_LShift },
		{ "RShift", GVK_RShift },
		{ "LCtrl", GVK_LCtrl },
		{ "RCtrl", GVK_RCtrl },
		{ "LAlt", GVK_LAlt },
		{ "RAlt", GVK_RAlt },
		{ "Enter", GVK_Enter },
		{ "Backspace", GVK_Backspace },
		{ "Delete", GVK_Delete },
		{ "Home", GVK_Home },
		{ "Insert", GVK_Insert },
		{ "End", GVK_End },
		{ "Tab", GVK_Tab },
		{ "Left", GVK_Left },
		{ "Up", GVK_Up },
		{ "Right", GVK_Right },
		{ "Down", GVK_Down },
		{ "Num0", GVK_Num0 },
		{ "Num1", GVK_Num1 },
		{ "Num2", GVK_Num2 },
		{ "Num3", GVK_Num3 },
		{ "Num4", GVK_Num4 },
		{ "Num5", GVK_Num5 },
		{ "Num6", GVK_Num6 },
		{ "Num7", GVK_Num7 },
		{ "Num8", GVK_Num8 },
		{ "Num9", GVK_Num9 },
		{ "F1", GVK_F1 },
		{ "F2", GVK_F2 },
		{ "F3", GVK_F3 },
		{ "F4", GVK_F4 },
		{ "F5", GVK_F5 },
		{ "F6", GVK_F6 },
		{ "F7", GVK_F7 },
		{ "F8", GVK_F8 },
		{ "F9", GVK_F9 },
		{ "F10", GVK_F10 },
		{ "F11", GVK_F11 },
		{ "F12", GVK_F12 },
		{ "A", GVK_A },
		{ "B", GVK_B },
		{ "C", GVK_C },
		{ "D", GVK_D },
		{ "E", GVK_E },
		{ "F", GVK_F },
		{ "G", GVK_G },
		{ "H", GVK_H },
		{ "I", GVK_I },
		{ "J", GVK_J },
		{ "K", GVK_K },
		{ "L", GVK_L },
		{ "M", GVK_M },
		{ "N", GVK_N },
		{ "O", GVK_O },
		{ "P", GVK_P },
		{ "Q", GVK_Q },
		{ "R", GVK_R },
		{ "S", GVK_S },
		{ "T", GVK_T },
		{ "U", GVK_U },
		{ "V", GVK_V },
		{ "W", GVK_W },
		{ "X", GVK_X },
		{ "Y", GVK_Y },
		{ "Z", GVK_Z },
		{ "0", GVK_0 },
		{ "1", GVK_1 },
		{ "2", GVK_2 },
		{ "3", GVK_3 },
		{ "4", GVK_4 },
		{ "5", GVK_5 },
		{ "6", GVK_6 },
		{ "7", GVK_7 },
		{ "8", GVK_8 },
		{ "9", GVK_9 },
		{ "`", GVK_Tilde },
		{ "~", GVK_Tilde },
		{ "Minus", GVK_Minus },
		{ "Equal", GVK_Equal },
		{ "\\", GVK_BackSlash },
		{ "'", GVK_SingleQuote },
		{ ";", GVK_Semicolon },
		{ "[", GVK_OpenBracket },
		{ "]", GVK_CloseBracket },
		{ "/", GVK_Slash },
		{ ".", GVK_Period },
		{ ",", GVK_Comma },
	};

	return CopyTo<CArray<CPair<CStringView, TGinVirtualKey>>>( keyValuePairs );
}

CArray<CStringView> CInputSettings::CreateCodeToKeyNameArray()
{
	CArray<CStringView> result;
	for( auto keyValue : keyNameCodePairs ) {
		const int code = keyValue.Second;
		if( code >= result.Size() ) {
			result.IncreaseSize( code + 1 );
		}
		result[code] = keyValue.First;
	}

	return result;
}

CMap<CStringView, TGinVirtualKey, CCaselessStringHash> CInputSettings::CreateKeyNameToCodeMap()
{
	CMap<CStringView, TGinVirtualKey, CCaselessStringHash> result;
	result.ReserveBuffer( keyNameCodePairs.Size() );
	for( auto keyValue : keyNameCodePairs ) {
		result.Add( keyValue.First, keyValue.Second );
	}

	return result;
}

CString CInputSettings::GetKeyName( int keyCode )
{
	if( keyCode >= 0 && codeToKeyName.Size() > keyCode ) {
		CStringView keyName = codeToKeyName[keyCode];
		if( !keyName.IsEmpty() ) {
			return Str( keyName );
		}
	}

	auto codeStr = Str( keyCode );
	return "$(" + move( codeStr ) + ')';
}

CString CInputSettings::GetFullKeyName( CKeyCombination key )
{
	CString result;
	const auto modifier = key.KeyModifier;
	if( key.AdditionalKey != 0 ) {
		result += GetKeyName( key.AdditionalKey );
		if( modifier == KMT_Undefined ) {
			result += '*';
		}
	}

	if( modifier == KMT_Press ) {
		result += '+';
	} else if( modifier == KMT_Release ) {
		result += '-';
	}
	result += GetKeyName( key.MainKey );
	return result;
}

CKeyCombination CInputSettings::createKeyCombination( TGinVirtualKey mainKey, TKeyModifierType modifier, TGinVirtualKey additionalKey )
{
	return CKeyCombination{ mainKey, modifier, additionalKey };
}

bool CInputSettings::isSameKey( CKeyCombination left, CKeyCombination right )
{
	return left.MainKey == right.MainKey && left.AdditionalKey == right.AdditionalKey && left.KeyModifier == right.KeyModifier;
}

TGinVirtualKey CInputSettings::GetKeyCode( CStringPart key )
{
	// Check for a direct code string.
	if( key[0] == L'$' && key[1] == L'(' ) {
		const int codeEnd = key.Find( L')', 2 );
		if( codeEnd != NotFound ) {
			const auto codeStr = key.Mid( 2, codeEnd - 2 ).TrimSpaces();
			const auto codeValue = Value<int>( codeStr );
			if( codeValue.IsValid() ) {
				return TGinVirtualKey( *codeValue );
			}
		}
	} 
		
	// Check for a user-readable code name.
	auto result = keyNameToCode.Get( key );
	return result == nullptr ? GVK_Undefined : *result;
}

COptional<CKeyCombination> CInputSettings::ParseKeyCombination( CStringPart keyStr )
{
	const auto additionalEndPos = keyStr.FindOneOf( "*+-" );
	CKeyCombination result;
	if( additionalEndPos != NotFound ) {
		const auto additionalStr = keyStr.Left( additionalEndPos ).TrimRight();
		if( !additionalStr.IsEmpty() ) {
			const auto code = GetKeyCode( additionalStr );
			result.AdditionalKey = code;
			if( code == NotFound ) {
				return COptional<CKeyCombination>();
			}
			result.AdditionalKey = code;
			keyStr = keyStr.Mid( additionalEndPos );
		} else {
			result.AdditionalKey = GVK_Null;
		}
	} else {
		result.AdditionalKey = GVK_Null;
	}

	if( keyStr.IsEmpty() ) {
		return COptional<CKeyCombination>();
	}

	const auto firstCh = keyStr[0];
	if( firstCh == L'+' ) {
		result.KeyModifier = KMT_Press;
		keyStr = keyStr.Mid( 1 ).TrimLeft();
	} else if( firstCh == L'-' ) {
		result.KeyModifier = KMT_Release;
		keyStr = keyStr.Mid( 1 ).TrimLeft();
	} else if( firstCh == L'*' ) {
		result.KeyModifier = KMT_Undefined;
		keyStr = keyStr.Mid( 1 ).TrimLeft();
	} else {
		result.KeyModifier = KMT_Undefined;
	}

	const auto mainCode = GetKeyCode( keyStr );
	result.MainKey = mainCode;
	return mainCode == NotFound ? COptional<CKeyCombination>{} : CreateOptional( result );
}

void CInputSettings::Save()
{
	CString resultStr;
	
	assert( fileData[0].Name.IsEmpty() );
	saveKeyActionPairs( fileData[0], resultStr );

	for( int i = 1; i < fileData.Size(); i++ ) {
		resultStr += createSectionNameStr( fileData[i].Name );
		saveKeyActionPairs( fileData[i], resultStr );
	}

	File::WriteText( fileName, resultStr );
}

CString CInputSettings::createSectionNameStr( CStringPart section )
{
	return '[' + section + "]\r\n";
}

void CInputSettings::saveKeyActionPairs( CInputTranslatorData& data, CString& result )
{
	for( const auto& keyValuePair : data.KeyActionPairs ) {
		saveKeyActionPair( keyValuePair.First, keyValuePair.Second, result );
	}
}

void CInputSettings::saveKeyActionPair( CKeyCombination key, CStringPart action, CString& result )
{
	result += GetFullKeyName( key );
	result += " = ";
	result += action;
	result += "\r\n";
}

const CInputTranslator& CInputSettings::GetInputTranslator( CStringPart name ) const
{
	const auto resultPtr = translatorTable.Get( name );
	return resultPtr == nullptr ? emptyTranslator : resultPtr->GetTranslator();
}

void CInputSettings::Empty( CStringPart segmentName )
{
	isModified = true;
	auto& translatorData = findTranslatorData( segmentName );
	translatorData.KeyActionPairs.Empty();
	auto& controlScheme = translatorTable.GetOrCreate( segmentName ).Value();
	controlScheme.Empty();
}

CInputTranslatorData& CInputSettings::findTranslatorData( CStringPart name )
{
	for( auto& data : fileData ) {
		if( data.Name == name ) {
			return data;
		}
	}

	auto& newData = fileData.Add();
	newData.Name = name;
	return newData;
}

CKeyCombination CInputSettings::RegisterPrimaryFileAction( CKeyCombination defaultKey, CStringPart actionName, CStringPart segmentName )
{
	auto& controlScheme = translatorTable.GetOrCreate( segmentName ).Value();
	auto& translatorData = findTranslatorData( segmentName );
	for( const auto& keyAction : translatorData.KeyActionPairs ) {
		if( actionName == keyAction.Second ) {
			tryCreateActionFromData( keyAction.First, actionName, controlScheme );
			return keyAction.First;
		}
	}

	if( defaultKey.MainKey != GVK_Null ) {
		createActionFromData( defaultKey, actionName, controlScheme );
	}
	// Create an empty primary action even for null keys, to distinguish between primary and secondary actions.
	createDynamicFileAction( defaultKey, actionName, translatorData );
	return defaultKey;
}

CKeyCombination CInputSettings::RegisterSecondaryFileAction( CKeyCombination defaultKey, CStringPart actionName, CStringPart segmentName )
{
	auto& controlScheme = translatorTable.GetOrCreate( segmentName ).Value();
	auto& translatorData = findTranslatorData( segmentName );
	int matchCount = 0;
	for( const auto& keyAction : translatorData.KeyActionPairs ) {
		if( actionName == keyAction.Second && ( ++matchCount == 2 ) ) {
			tryCreateActionFromData( keyAction.First, actionName, controlScheme );
			return keyAction.First;
		}
	}

	if( defaultKey.MainKey != GVK_Null ) {
		createActionFromData( defaultKey, actionName, controlScheme );
		createDynamicFileAction( defaultKey, actionName, translatorData );
	}
	return defaultKey;
}

void CInputSettings::createDynamicFileAction( CKeyCombination keyCombination, CStringPart actionName, CInputTranslatorData& translatorData )
{
	isModified = true;
	translatorData.KeyActionPairs.Add( keyCombination, actionName );
}

void CInputSettings::tryCreateActionFromData( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const
{
	if( key.MainKey != GVK_Null ) {
		createActionFromData( key, actionName, controlScheme );
	}
}

void CInputSettings::createActionFromData( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const
{
	if( key.KeyModifier == KMT_Undefined ) {
		key.KeyModifier = KMT_Press;
		if( !tryCreateUserAction( key, '+' + actionName, controlScheme ) ) {
			createUserAction( key, actionName, controlScheme );
		} 
		key.KeyModifier = KMT_Release;
		tryCreateUserAction( key, '-' + actionName, controlScheme );
	} else {
		createUserAction( key, actionName, controlScheme );
	}
}

bool CInputSettings::tryCreateUserAction( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const
{
	auto unicodeAction = UnicodeStr( actionName );
	if( IsExternalName( unicodeAction ) ) {
		auto newAction = createInputAction( unicodeAction );
		addControlSchemeAction( key, move( newAction ), controlScheme );
		return true;
	} else {
		return false;
	}
}

CPtrOwner<TUserAction> CInputSettings::createInputAction( CUnicodePart actionName ) const
{
	return CreateUniqueObject<TUserAction>( actionName );
}

const CUnicodeView unknownActionWarning = L"Unknown input action: %0";
void CInputSettings::createUserAction( CKeyCombination key, CStringPart actionName, CControlScheme& controlScheme ) const
{
	if( !tryCreateUserAction( key, actionName, controlScheme ) ) {
		const auto warningStr = unknownActionWarning.SubstParam( actionName );
		Log::Warning( warningStr, this );
	}
}

void CInputSettings::addControlSchemeAction( CKeyCombination key, CPtrOwner<TUserAction> action, CControlScheme& controlScheme ) const
{
	assert( key.KeyModifier == KMT_Press || key.KeyModifier == KMT_Release );
	controlScheme.AddAction( key, move( action ) );
}

void CInputSettings::AddBasicAction( CKeyCombination keyCombination, CPtrOwner<TUserAction> action, CStringPart segmentName )
{
	assert( keyCombination.KeyModifier != KMT_Undefined );
	auto& controlScheme = translatorTable.GetOrCreate( segmentName ).Value();
	addControlSchemeAction( keyCombination, move( action ), controlScheme );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
