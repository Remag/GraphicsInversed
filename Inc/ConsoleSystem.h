#pragma once
#include <GinDefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// A basic console command. Provides interface for setting a typed value.
class IConsoleValue {
public:
	IConsoleValue() = default;
	virtual ~IConsoleValue() {}

	// Set a value given its string representation.
	virtual void SetConsoleValue( CStringPart valueStr ) = 0;
	// Get the value as string.
	virtual CString GetConsoleValueString() const = 0;

private:
	// Copying is prohibited.
	IConsoleValue( IConsoleValue& ) = delete;
	void operator=( IConsoleValue& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

// System for storing and setting console commands.
// A console command is an arbitrary string-value pair.
class CConsoleSystem {
public:
	CConsoleSystem() = default;

	// Get the entire dictionary for command iteration.
	const auto& GetCommandDictionary() const
		{ return nameToSetting; }

	IConsoleValue* TryGetConsoleCommand( CStringPart name );
	IConsoleValue& GetConsoleCommand( CStringPart name )
		{ return *nameToSetting[name]; }
	void AddConsoleCommand( CStringPart name, IConsoleValue& value )
		{ nameToSetting.Add( name, &value ); }

private:
	// Commands dictionary.
	CMap<CString, IConsoleValue*, CCaselessStringHash> nameToSetting;

	// Copying is prohibited.
	CConsoleSystem( CConsoleSystem& ) = delete;
	void operator=( CConsoleSystem& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

inline IConsoleValue* CConsoleSystem::TryGetConsoleCommand( CStringPart name )
{
	const auto resultPtr = nameToSetting.Get( name );
	return resultPtr == nullptr ? nullptr : *resultPtr;
}

//////////////////////////////////////////////////////////////////////////

namespace GinInternal {

extern GINAPI CConsoleSystem GinGlobalConsoleSystem;
// Class that returns a global Gin console system.
struct CGinConsoleProvider {
	static CConsoleSystem& GetConsoleSystem()
		{ return GinGlobalConsoleSystem; }
};

}	// namespace GinInternal.

// Access to global gin console system.
inline CConsoleSystem& GetGlobalConsoleSystem()
{
	return GinInternal::GinGlobalConsoleSystem;
}

//////////////////////////////////////////////////////////////////////////

// Templated command that uses string-to-value conversions to set the value.
template <class T, class ConsoleProvider = GinInternal::CGinConsoleProvider>
class CConsoleValue : public IConsoleValue {
public:
	CConsoleValue( CStringView name, T initialValue ) : consoleValue( move( initialValue ) )
		{ ConsoleProvider::GetConsoleSystem().AddConsoleCommand( name, *this ); }

	const T& GetValue() const
		{ return consoleValue; }
	void SetValue( T newValue )
		{ consoleValue = move( newValue ); }

	virtual void SetConsoleValue( CUnicodePart valueStr ) override final;
	virtual CString GetConsoleValueString() const override final
		{ return Str( consoleValue ); }

private:
	T consoleValue;
};

//////////////////////////////////////////////////////////////////////////

template <class T, class ConsoleProvider /*= GinInternal::CGinConsoleProvider*/>
void CConsoleValue<T, ConsoleProvider>::SetConsoleValue( CUnicodePart valueStr )
{
	const auto value = Value<T>( valueStr );
	if( !value.IsValid() ) {
		Log::Warning( L"Invalid console value: " + valueStr );
		return;
	}
	consoleValue = *value;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

