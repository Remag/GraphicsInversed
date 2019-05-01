#pragma once
#include <ParticleEmitter.h>
#include <GinTypes.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// A typed particle emitter. Particle variable types are specified at compile time.
// RealEmitter is the type of emitter that inherits from this class. It must contain a static method RealEmitter::GetInOutVariables().
// This method should return an array of integer/string pairs representing the variable in a transform feedback loop.
template <class RealEmitter, class... VarTypes>
class CBaseParticleEmitter : public IParticleEmitter {
public:
	static CArrayView<CParticleVariableData> GetParticleData();

	virtual CArrayView<CParticleVariableData> GetEmittedParticles() const override final
		{ return GetParticleData(); }
	virtual void FillRawParticleData( CArrayBuffer<BYTE> particleData, CRandomGenerator& rng ) override final;

	virtual void FillParticleData( CArrayBuffer<CTuple<VarTypes...>> particleData, CRandomGenerator& rng ) = 0;

private:
	static CArrayView<CParticleVariableData> createParticleData();

	static int getParticleSize()
		{ return calcParticleSize<VarTypes...>(); }
	template <class CurrentType, class NextType, class... RestTypes>
	static int calcParticleSize()
		{ return sizeof( CurrentType ) + calcParticleSize<NextType, RestTypes...>(); }
	template <class LastType>
	static int calcParticleSize()
		{ return sizeof( LastType ); }

	template <class FirstType, class SecondType, class... RestTypes>
	static void fillParticleVariableData( CParticleVariableData* currentData, const CPair<int, CStringView>* nameData );
	template <class LastType>
	static void fillParticleVariableData( CParticleVariableData* lastData, const CPair<int, CStringView>* lastName );
};

//////////////////////////////////////////////////////////////////////////

template <class RealEmitter, class... VarTypes>
CArrayView<CParticleVariableData> CBaseParticleEmitter<RealEmitter, VarTypes...>::GetParticleData()
{
	static CArrayView<CParticleVariableData> result = createParticleData();
	return result;
}

template <class RealEmitter, class... VarTypes>
CArrayView<CParticleVariableData> CBaseParticleEmitter<RealEmitter, VarTypes...>::createParticleData()
{
	static CStackArray<CParticleVariableData, sizeof...( VarTypes )> resultParticles;
	const CArrayView<CPair<int, CStringView>> inOutVariables( RealEmitter::GetInOutVariables() );
	assert( inOutVariables.Size() == sizeof...( VarTypes ) );
	fillParticleVariableData<VarTypes...>( resultParticles.Ptr(), inOutVariables.Ptr() );
	return resultParticles;
}

template <class RealEmitter, class... VarTypes>
template <class FirstType, class SecondType, class... RestTypes>
void CBaseParticleEmitter<RealEmitter, VarTypes...>::fillParticleVariableData( CParticleVariableData* currentData, const CPair<int, CStringView>* nameData )
{
	fillParticleVariableData<FirstType>( currentData, nameData );
	fillParticleVariableData<SecondType, RestTypes...>( currentData + 1, nameData + 1 );
}

template <class RealEmitter, class... VarTypes>
template <class LastType>
void CBaseParticleEmitter<RealEmitter, VarTypes...>::fillParticleVariableData( CParticleVariableData* lastData, const CPair<int, CStringView>* lastName )
{
	lastData->InLocation = lastName->First;
	lastData->OutName = lastName->Second;
	lastData->GlType = GinTypes::GlType<LastType>::Type;
}

template <class RealEmitter, class... VarTypes>
void CBaseParticleEmitter<RealEmitter, VarTypes...>::FillRawParticleData( CArrayBuffer<BYTE> particleData, CRandomGenerator& rng )
{
	const int particleSize = getParticleSize();
	assert( particleData.Size() % particleSize == 0 );
	const int particleCount = particleData.Size() / particleSize;
	CArrayBuffer<CTuple<VarTypes...>> tupleBuffer( reinterpret_cast<CTuple<VarTypes...>*>( particleData.Ptr() ), particleCount );
	FillParticleData( tupleBuffer, rng );
}

//////////////////////////////////////////////////////////////////////////

// Single-variable particle emitter fills particle data using an array of the needed argument instead of a tuple.
template <class RealEmitter, class T>
class CBaseParticleEmitter<RealEmitter, T> : public IParticleEmitter {
public:
	static CArrayView<CParticleVariableData> GetParticleData();

	virtual CArrayView<CParticleVariableData> GetEmittedParticles() const override final
		{ return GetParticleData(); }

	virtual void FillRawParticleData( CArrayBuffer<BYTE> particleData, CRandomGenerator& rng ) override final;

	virtual void FillParticleData( CArrayBuffer<T> particleData, CRandomGenerator& rng ) = 0;

private:
	static CParticleVariableData createParticleData();
};

//////////////////////////////////////////////////////////////////////////

template <class RealEmitter, class T>
CArrayView<CParticleVariableData> CBaseParticleEmitter<RealEmitter, T>::GetParticleData()
{
	static CParticleVariableData result = createParticleData();
	return CArrayView<CParticleVariableData>( result );
}

template <class RealEmitter, class T>
CParticleVariableData CBaseParticleEmitter<RealEmitter, T>::createParticleData()
{
	const auto& inOutVariable = RealEmitter::GetInOutVariables();
	CArrayView<CPair<int, CStringView>> inOutVariableArray( inOutVariable );
	assert( inOutVariableArray.Size() == 1 );
	auto variablePair = inOutVariableArray.First();
	return CParticleVariableData{ variablePair.First, variablePair.Second, GinTypes::GlType<T>::Type };
}

template <class RealEmitter, class T>
void CBaseParticleEmitter<RealEmitter, T>::FillRawParticleData( CArrayBuffer<BYTE> particleData, CRandomGenerator& rng )
{
	const int particleSize = sizeof( T );
	assert( particleData.Size() % particleSize == 0 );
	const int particleCount = particleData.Size() / particleSize;
	CArrayBuffer<T> typeBuffer( reinterpret_cast<T*>( particleData.Ptr() ), particleCount );
	FillParticleData( typeBuffer, rng );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

