#include <common.h>
#pragma hdrstop

#include <InputSettings.h>
#include <InputHandler.h>
#include <GinGlobals.h>
#include <UtilityUserInputActions.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CInputSettings::CInputSettings( CUnicodeView _fileName ) :
	fileName( _fileName ),
	fileData( parseCfgFile() )
{
	initializeTranslators();
}

CArray<CInputTranslatorData> CInputSettings::parseCfgFile() const
{
	CArray<CInputTranslatorData> result;
	const auto fileStr = File::ReadUnicodeText( fileName );

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

const auto commentSymbol = L'/';
bool CInputSettings::shouldSkip( CUnicodePart str ) const
{
	const bool isComment = str.Length() >= 2 && str[0] == commentSymbol && str[2] == commentSymbol;
	return str.IsEmpty() || isComment;
}

bool CInputSettings::isSectionName( CUnicodePart str ) const
{
	return str[0] == L'[';
}

const CUnicodeView unknownConfigStr = L"Invalid string in configuration file: %0.\r\nFile name: %1";
CUnicodePart CInputSettings::parseSectionName( CUnicodePart str ) const
{
	const auto endCommentPos = findEndCommentPos( str );
	auto endPos = str.Find( L']', 1 );
	if( endPos == NotFound || endPos > endCommentPos ) {
		unknownStrWarning( str );
		endPos = endCommentPos;
	}

	return str.Mid( 1, endPos - 1 ).TrimSpaces();
}

void CInputSettings::unknownStrWarning( CUnicodePart str ) const
{
	Log::Warning( unknownConfigStr.SubstParam( str, fileName ), this );
}

int CInputSettings::findEndCommentPos( CUnicodePart str )
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

void CInputSettings::parseKeyValuePair( CUnicodePart str, CInputTranslatorData& result ) const
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

void CInputSettings::initializeTranslators()
{
	for( auto& translator : translatorTable ) {
		translator.Value().Empty();
	}

	const auto& commonData = fileData[0];
	assert( commonData.Name.IsEmpty() );

	for( int i = 1; i < fileData.Size(); i++ ) {
		auto& translator = translatorTable.GetOrCreate( copy( fileData[i].Name ) ).Value();
		fillTranslator( commonData, translator );
		fillTranslator( fileData[i], translator );
	}
}

void CInputSettings::fillTranslator( const CInputTranslatorData& source, CInputTranslator& translator ) const
{
	const int actionCount = source.KeyActionPairs.Size();
	for( int i = 0; i < actionCount; i++ ) {
		const auto key = source.KeyActionPairs[i].First;
		const CUnicodeView name = source.KeyActionPairs[i].Second;
		createActionFromData( key, name, translator );
	}
}

void CInputSettings::createActionFromData( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const
{
	if( key.KeyModifier == KMT_None ) {
		key.KeyModifier = KMT_Press;
		if( !tryCreateUserAction( key, L'+' + actionName, translator ) ) {
			createUserAction( key, actionName, translator );
		} 
		key.KeyModifier = KMT_Release;
		tryCreateUserAction( key, L'-' + actionName, translator );
	} else if( key.KeyModifier == KMT_Hotkey ) {
		createUserHotkey( key, actionName, translator );
	} else {
		createUserAction( key, actionName, translator );
	}
}

bool CInputSettings::tryCreateUserHotkey( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const
{
	if( IsExternalName( actionName ) ) {
		auto resultAction = createNewAction( actionName, key );
		addTranslatorHotkey( key, move( resultAction ), translator );
		return true;
	} else {
		return false;
	}
}

bool CInputSettings::tryCreateUserAction( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const
{
	if( IsExternalName( actionName ) ) {
		auto resultAction = createNewAction( actionName, key );
		addTranslatorAction( key, move( resultAction ), translator );
		return true;
	} else {
		return false;
	}
}

const CUnicodeView unknownActionWarning = L"Unknown input action: %0";
void CInputSettings::createUserHotkey( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const
{
	if( !tryCreateUserHotkey( key, actionName, translator ) ) {
		const auto warningStr = unknownActionWarning.SubstParam( actionName );
		Log::Warning( warningStr, this );
	}
}

void CInputSettings::createUserAction( CKeyCombination key, CUnicodeView actionName, CInputTranslator& translator ) const
{
	if( !tryCreateUserAction( key, actionName, translator ) ) {
		const auto warningStr = unknownActionWarning.SubstParam( actionName );
		Log::Warning( warningStr, this );
	}
}

const CUnicodeView invalidHotkeyMsg = L"Hotkey failed to register.";
void CInputSettings::addTranslatorHotkey( CKeyCombination key, CPtrOwner<TUserAction> action, CInputTranslator& translator ) const
{
	assert( key.KeyModifier == KMT_Hotkey );
	auto oldAction = translator.DetachHotkeyAction( key.MainKey );
	translator.SetHotkeyAction( key.MainKey, mergeActions( move( action ), move( oldAction ) ) );
}

void CInputSettings::addTranslatorAction( CKeyCombination key, CPtrOwner<TUserAction> action, CInputTranslator& translator ) const
{
	assert( key.KeyModifier == KMT_Press || key.KeyModifier == KMT_Release );
	const bool isDown = key.KeyModifier == KMT_Press;
	auto oldAction = translator.DetachAction( key.MainKey, isDown );
	translator.SetAction( key.MainKey, isDown, mergeActions( move( action ), move( oldAction ) ) );
}

CInputSettings::~CInputSettings()
{
	Save();
}

CArray<CPair<CUnicodeView, int>> CInputSettings::CreateKeyNamePairs()
{
	static CArrayView<CPair<CUnicodeView, int>> keyValuePairs {
		{ L"NoKey", 0 },
		{ L"Null", 0 },
		{ L"LMouse", VK_LBUTTON },
		{ L"RMouse", VK_RBUTTON },
		{ L"MMouse", VK_MBUTTON },
		{ L"Mouse4", VK_XBUTTON1 },
		{ L"Mouse5", VK_XBUTTON2 },
		{ L"LDClick", 0x3A },
		{ L"RDClick", 0x3B },
		{ L"MDClick", 0x3C },
		{ L"WheelDown", 0x0E },
		{ L"WheelUp", 0x0F },
		{ L"Esc", VK_ESCAPE },
		{ L"Space", VK_SPACE },
		{ L"LShift", VK_LSHIFT },
		{ L"RShift", VK_RSHIFT },
		{ L"Shift", VK_SHIFT },
		{ L"LCtrl", VK_LCONTROL },
		{ L"RCtrl", VK_RCONTROL },
		{ L"Ctrl", VK_CONTROL },
		{ L"Enter", VK_RETURN },
		{ L"Backspace", VK_BACK },
		{ L"Delete", VK_DELETE },
		{ L"Home", VK_HOME },
		{ L"End", VK_END },
		{ L"Tab", VK_TAB },
		{ L"Left", VK_LEFT },
		{ L"Up", VK_UP },
		{ L"Right", VK_RIGHT },
		{ L"Down", VK_DOWN },
		{ L"Num0", VK_NUMPAD0 },
		{ L"Num1", VK_NUMPAD1 },
		{ L"Num2", VK_NUMPAD2 },
		{ L"Num3", VK_NUMPAD3 },
		{ L"Num4", VK_NUMPAD4 },
		{ L"Num5", VK_NUMPAD5 },
		{ L"Num6", VK_NUMPAD6 },
		{ L"Num7", VK_NUMPAD7 },
		{ L"Num8", VK_NUMPAD8 },
		{ L"Num9", VK_NUMPAD9 },
		{ L"F1", VK_F1 },
		{ L"F2", VK_F2 },
		{ L"F3", VK_F3 },
		{ L"F4", VK_F4 },
		{ L"F5", VK_F5 },
		{ L"F6", VK_F6 },
		{ L"F7", VK_F7 },
		{ L"F8", VK_F8 },
		{ L"F9", VK_F9 },
		{ L"F10", VK_F10 },
		{ L"F11", VK_F11 },
		{ L"F12", VK_F12 },
		{ L"A", 0x41 },
		{ L"B", 0x42 },
		{ L"C", 0x43 },
		{ L"D", 0x44 },
		{ L"E", 0x45 },
		{ L"F", 0x46 },
		{ L"G", 0x47 },
		{ L"H", 0x48 },
		{ L"I", 0x49 },
		{ L"J", 0x4A },
		{ L"K", 0x4B },
		{ L"L", 0x4C },
		{ L"M", 0x4D },
		{ L"N", 0x4E },
		{ L"O", 0x4F },
		{ L"P", 0x50 },
		{ L"Q", 0x51 },
		{ L"R", 0x52 },
		{ L"S", 0x53 },
		{ L"T", 0x54 },
		{ L"U", 0x55 },
		{ L"V", 0x56 },
		{ L"W", 0x57 },
		{ L"X", 0x58 },
		{ L"Y", 0x59 },
		{ L"Z", 0x5A },
		{ L"0", 0x30 },
		{ L"1", 0x31 },
		{ L"2", 0x32 },
		{ L"3", 0x33 },
		{ L"4", 0x34 },
		{ L"5", 0x35 },
		{ L"6", 0x36 },
		{ L"7", 0x37 },
		{ L"8", 0x38 },
		{ L"9", 0x39 },
		{ L"`", VK_OEM_3 },
		{ L"~", VK_OEM_3 },
		{ L"Minus", VK_OEM_MINUS },
		{ L"Equal", VK_OEM_PLUS },
		{ L"\\", VK_OEM_5 },
		{ L"'", VK_OEM_7 },
		{ L";", VK_OEM_1 },
		{ L"[", VK_OEM_4 },
		{ L"]", VK_OEM_6 },
		{ L"/", VK_OEM_2 },
		{ L".", VK_OEM_PERIOD },
		{ L",", VK_OEM_COMMA },
	};

	return CopyTo<CArray<CPair<CUnicodeView, int>>>( keyValuePairs );
}

CArray<CUnicodeView> CInputSettings::CreateCodeToKeyNameArray()
{
	CArray<CUnicodeView> result;
	for( auto keyValue : keyNameCodePairs ) {
		const int code = keyValue.Second;
		if( code >= result.Size() ) {
			result.IncreaseSize( code + 1 );
		}
		result[code] = keyValue.First;
	}

	return result;
}

CMap<CUnicodeView, int, CIniKeyHashStrategy> CInputSettings::CreateKeyNameToCodeMap()
{
	CMap<CUnicodeView, int, CIniKeyHashStrategy> result;
	result.ReserveBuffer( keyNameCodePairs.Size() );
	for( auto keyValue : keyNameCodePairs ) {
		result.Add( keyValue.First, keyValue.Second );
	}

	return result;
}

CUnicodeString CInputSettings::GetKeyName( int keyCode )
{
	if( keyCode >= 0 && codeToKeyName.Size() > keyCode ) {
		CUnicodeView keyName = codeToKeyName[keyCode];
		if( !keyName.IsEmpty() ) {
			return UnicodeStr( keyName );
		}
	}

	CUnicodeString codeStr = UnicodeStr( keyCode );
	return L"$(" + move( codeStr ) + L')';
}

CUnicodeString CInputSettings::GetFullKeyName( int keyCode, bool isDown )
{
	return getFullKeyString( keyCode, isDown );
}

CKeyCombination CInputSettings::createKeyCombination( int mainKey, TKeyModifierType modifier, int additionalKey )
{
	return CKeyCombination{ mainKey, additionalKey, modifier };
}

int CInputSettings::GetKeyCode( CUnicodePart key )
{
	// Check for a direct code string.
	if( key[0] == L'$' && key[1] == L'(' ) {
		const int codeEnd = key.Find( L')', 2 );
		if( codeEnd != NotFound ) {
			const auto codeStr = key.Mid( 2, codeEnd - 2 ).TrimSpaces();
			const auto codeValue = Value<int>( codeStr );
			if( codeValue.IsValid() ) {
				return *codeValue;
			}
		}
	} 
		
	// Check for a user-readable code name.
	auto result = keyNameToCode.Get( key );
	return result == nullptr ? NotFound : *result;
}

COptional<CKeyCombination> CInputSettings::ParseKeyCombination( CUnicodePart keyStr )
{
	const auto additionalEndPos = keyStr.FindOneOf( L"*+-@" );
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
			result.AdditionalKey = 0;
		}
	} else {
		result.AdditionalKey = 0;
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
		result.KeyModifier = KMT_None;
		keyStr = keyStr.Mid( 1 ).TrimLeft();
	} else if( firstCh == L'@' ) {
		result.KeyModifier = KMT_Hotkey;
		keyStr = keyStr.Mid( 1 ).TrimLeft();
	} else {
		result.KeyModifier = KMT_None;
	}

	const auto mainCode = GetKeyCode( keyStr );
	result.MainKey = mainCode;
	return mainCode == NotFound ? COptional<CKeyCombination>{} : CreateOptional( result );
}

CArray<CInputTranslatorData> CInputSettings::ExportSettings() const
{
	return copy( fileData );
}

void CInputSettings::ImportSettings( CArray<CInputTranslatorData> data )
{
	fileData = move( data );
	initializeTranslators();
}

void CInputSettings::Save()
{
	CUnicodeString resultStr;
	
	assert( fileData[0].Name.IsEmpty() );
	saveKeyActionPairs( fileData[0], resultStr );

	for( int i = 1; i < fileData.Size(); i++ ) {
		resultStr += createSectionNameStr( fileData[i].Name );
		saveKeyActionPairs( fileData[i], resultStr );
	}

	File::WriteUnicodeText( fileName, resultStr );
}

CUnicodeString CInputSettings::createSectionNameStr( CUnicodeView section )
{
	return L'[' + section + L"]\r\n";
}

void CInputSettings::saveKeyActionPairs( CInputTranslatorData& data, CUnicodeString& result )
{
	for( const auto& keyValuePair : data.KeyActionPairs ) {
		saveKeyActionPair( keyValuePair.First, keyValuePair.Second, result );
	}
}

void CInputSettings::saveKeyActionPair( CKeyCombination key, CUnicodeView action, CUnicodeString& result )
{
	const auto modifier = key.KeyModifier;
	if( key.AdditionalKey != 0 ) {
		result += GetKeyName( key.AdditionalKey );
		if( modifier == KMT_None ) {
			result += L'*';
		}
	}

	if( modifier == KMT_Press ) {
		result += L"+";
	} else if( modifier == KMT_Release ) {
		result += L"-";
	} else if( modifier == KMT_Hotkey ) {
		result += L"@";
	}
	result += GetKeyName( key.MainKey );

	result += L" = ";
	result += action;
	result += L"\r\n";
}

CUnicodeString CInputSettings::getFullKeyString( int code, bool isDown )
{
	const CUnicodeView prefix = isDown ? L"+" : L"-";
	return prefix + GetKeyName( code );
}

const CInputTranslator& CInputSettings::GetTranslator( CUnicodePart name ) const
{
	const auto resultPtr = translatorTable.Get( name );
	const CUnicodeView emptyStr;
	return resultPtr == nullptr ? translatorTable[emptyStr] : *resultPtr;
}

CArrayView<CPair<CKeyCombination, CUnicodeString>> CInputSettings::GetKeyValuePairs( CUnicodePart translatorName ) const
{
	return findTranslatorData( translatorName ).KeyActionPairs;
}

void CInputSettings::DeleteAllActions( int keyCode, bool isDown, CUnicodePart translatorName )
{
	isModified = true;
	auto& translator = translatorTable.GetOrCreate( translatorName ).Value();
	auto result = translator.DetachAction( keyCode, isDown );
	const auto keyCombination = createKeyCombination( keyCode, isDown ? KMT_Press : KMT_Release, 0 );
	auto filteredAction = filterSpecificAction( move( result ), keyCombination );
	translator.SetAction( keyCode, isDown, move( filteredAction ) );

	auto& translatorData = findTranslatorData( translatorName );
	for( int i = translatorData.KeyActionPairs.Size() - 1; i >= 0; i-- ) {
		deleteEqualKeys( translatorData, i, keyCombination );
	}
}

CPtrOwner<TUserAction> CInputSettings::filterSpecificAction( CPtrOwner<TUserAction> action, CKeyCombination keyToFilter )
{
	// Filter both actions in the combined actions.
	auto combinedAction = dynamic_cast<CCombinedUserInputAction*>( action.Ptr() );
	if( combinedAction != nullptr ) {
		auto newFirst = filterSpecificAction( combinedAction->DetachFirst(), keyToFilter );
		auto newSecond = filterSpecificAction( combinedAction->DetachSecond(), keyToFilter );
		if( newFirst == nullptr ) {
			return newSecond;
		} else if( newSecond == nullptr ) {
			return newFirst;
		} else {
			combinedAction->SetFirst( move( newFirst ) );
			combinedAction->SetSecond( move( newSecond ) );
			return action;
		}
	}

	// Remove an additional key action if the keys match.
	const auto additionalKeyAction = dynamic_cast<CAdditionalKeyAction*>( action.Ptr() );
	if( additionalKeyAction != nullptr ) {
		const int additionalKey = additionalKeyAction->GetAdditionalKey();
		return keyToFilter.AdditionalKey == additionalKey ? nullptr : move( action );
	}

	// No settings specific actions detected - check if a simple action needs to be removed and do it if necessary.
	return keyToFilter.AdditionalKey == 0 ? nullptr : move( action );
}

void CInputSettings::deleteEqualKeys( CInputTranslatorData& target, int pos, CKeyCombination key )
{
	assert( key.KeyModifier == KMT_Press || key.KeyModifier == KMT_Release );
	const bool isDown = key.KeyModifier == KMT_Press;
	auto& targetKey = target.KeyActionPairs[pos].First;
	auto& targetName = target.KeyActionPairs[pos].Second;

	if( key.MainKey != targetKey.MainKey || key.AdditionalKey != targetKey.AdditionalKey || targetKey.KeyModifier == KMT_Hotkey ) {
		return;
	}
	
	if( targetKey.KeyModifier != KMT_None ) {
		if( targetKey.KeyModifier == key.KeyModifier ) {
			target.KeyActionPairs.DeleteAt( pos );
		}
		return;
	}	

	deleteActionPart( targetKey, targetName, isDown );
}

void CInputSettings::DeleteSingleAction( int keyCode, bool isDown, CUnicodePart actionName, CUnicodePart translatorName )
{
	isModified = true;

	auto& translator = translatorTable.GetOrCreate( translatorName ).Value();
	translator.DetachAction( keyCode, isDown );
	const auto keyCombination = createKeyCombination( keyCode, isDown ? KMT_Press : KMT_Release, 0 );

	// Recreate the combined action without the specific action name.
	auto& translatorData = findTranslatorData( translatorName );
	CPtrOwner<TUserAction> restoredAction;
	for( int i = translatorData.KeyActionPairs.Size() - 1; i >= 0; i-- ) {
		restoreInputAction( translatorData, i, keyCombination, actionName, restoredAction );
	}

	translator.SetAction( keyCode, isDown, move( restoredAction ) );
}

void CInputSettings::restoreInputAction( CInputTranslatorData& target, int pos, CKeyCombination key, CUnicodePart actionName, CPtrOwner<TUserAction>& result )
{
	assert( key.KeyModifier == KMT_Press || key.KeyModifier == KMT_Release );
	const bool isDown = key.KeyModifier == KMT_Press;
	auto& targetKey = target.KeyActionPairs[pos].First;
	auto& targetName = target.KeyActionPairs[pos].Second;

	if( key.MainKey != targetKey.MainKey || key.AdditionalKey != targetKey.AdditionalKey || targetKey.KeyModifier == KMT_Hotkey ) {
		return;
	}
	
	if( targetKey.KeyModifier != KMT_None ) {
		if( targetKey.KeyModifier != key.KeyModifier ) {
			return;
		}

		if( actionName == targetName ) {
			target.KeyActionPairs.DeleteAt( pos );
		} else {
			auto mergedAction = mergeActions( createNewAction( targetName, targetKey ), move( result ) );
			result = move( mergedAction );
		}
		return;
	}	

	const auto deletedSign = isDown ? L'+' : L'-';
	const auto deletedName = deletedSign + targetName;
	if( deletedName != actionName ) {
		auto mergedAction = mergeActions( createNewAction( targetName, targetKey ), move( result ) );
		result = move( mergedAction );
		return;
	}

	deleteActionPart( targetKey, targetName, isDown );
}

void CInputSettings::deleteActionPart( CKeyCombination& targetKey, CUnicodeString& targetName, bool deleteDown )
{
	targetKey.KeyModifier = deleteDown ? KMT_Release : KMT_Press;
	const auto signCh = deleteDown ? L'-' : L'+';
	if( targetName[0] == L'-' || targetName[0] == L'+' ) {
		targetName[0] = signCh;
	} else {
		targetName = signCh + targetName;
	}
}

void CInputSettings::AddAction( int keyCode, bool isDown, CUnicodePart actionName, CUnicodePart translatorName )
{
	isModified = true;
	auto& translator = translatorTable.GetOrCreate( translatorName ).Value();
	const auto keyCombination = createKeyCombination( keyCode, isDown ? KMT_Press : KMT_Release, 0 );
	addTranslatorAction( keyCombination, createNewAction( actionName, keyCombination ), translator );

	auto& translatorData = findTranslatorData( translatorName );
	addActionData( keyCombination, actionName, translatorData );
}

void CInputSettings::addActionData( CKeyCombination key, CUnicodePart action, CInputTranslatorData& targetData )
{
	assert( key.KeyModifier == KMT_Press || key.KeyModifier == KMT_Release );
	const bool isDown = key.KeyModifier == KMT_Press;
	const auto targetCh = isDown ? L'+' : L'-';
	const auto firstCh = action[0];
	if( firstCh == targetCh ) {
		const auto oppositeCh = isDown ? L'-' : L'+';
		const int pairCount = targetData.KeyActionPairs.Size();
		for( int i = pairCount - 1; i >= 0; i-- ) {
			auto& keyValuePair = targetData.KeyActionPairs[i];
			const auto targetKey = keyValuePair.First;
			const auto targetName = keyValuePair.Second.Mid( 1 );
			// Merge the action with an existing one if they match.
			if(	targetKey.MainKey == key.MainKey
				&& targetKey.AdditionalKey == key.AdditionalKey
				&& targetKey.KeyModifier != key.KeyModifier 
				&& keyValuePair.Second[0] == oppositeCh 
				&& targetName == action.Mid( 1 ) )
			{
				keyValuePair.Second = targetName;
				keyValuePair.First.KeyModifier = KMT_None;
				return;
			}
		}
	}

	// Merging conditions were not met, simply add the action.
	targetData.KeyActionPairs.Add( key, action );
}

CInputTranslatorData& CInputSettings::findTranslatorData( CUnicodePart name )
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

CPtrOwner<TUserAction> CInputSettings::mergeActions( CPtrOwner<TUserAction> newAction, CPtrOwner<TUserAction> oldAction )
{
	assert( newAction != nullptr );
	return oldAction == nullptr ? move( newAction ) : ptr_static_cast<TUserAction>( CreateOwner<CCombinedUserInputAction>( move( oldAction ), move( newAction ) ) );
}

CPtrOwner<TUserAction> CInputSettings::createNewAction( CUnicodePart name, CKeyCombination key )
{
	auto newAction = CreateUniqueObject<TUserAction>( name );
	return key.AdditionalKey == 0 ? move( newAction ) : ptr_static_cast<TUserAction>( CreateOwner<CAdditionalKeyAction>( move( newAction ), key.AdditionalKey ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
