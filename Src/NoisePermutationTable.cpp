#include <common.h>
#pragma hdrstop

#include <NoisePermutationTable.h>
#include <TextureBinder.h>

namespace Gin {

namespace GinInternal {

CSamplerObject CBaseNoiseTable::tableSampler;

const CVector2<int> CBaseNoiseTable::permTableSize{ permTableRowSize, permTableRowSize };

// Possible gradient values.
const CStackArray<CVector3<BYTE>, 12> CBaseNoiseTable::allowedGradients {
	CVector3<BYTE>{ BYTE( 255 ), BYTE( 255 ), BYTE( 128 ) },
	CVector3<BYTE>{ BYTE( 0 ), BYTE( 255 ), BYTE( 128 ) },
	CVector3<BYTE>{ BYTE( 255 ), BYTE( 0 ), BYTE( 128 ) },
	CVector3<BYTE>{ BYTE( 0 ), BYTE( 0 ), BYTE( 128 ) },
	CVector3<BYTE>{ BYTE( 255 ), BYTE( 128 ), BYTE( 255 ) },
	CVector3<BYTE>{ BYTE( 255 ), BYTE( 128 ), BYTE( 0 ) },
	CVector3<BYTE>{ BYTE( 0 ), BYTE( 128 ), BYTE( 255 ) },
	CVector3<BYTE>{ BYTE( 0 ), BYTE( 128 ), BYTE( 0 ) },
	CVector3<BYTE>{ BYTE( 128 ), BYTE( 255 ), BYTE( 255 ) },
	CVector3<BYTE>{ BYTE( 128 ), BYTE( 255 ), BYTE( 0 ) },
	CVector3<BYTE>{ BYTE( 128 ), BYTE( 0 ), BYTE( 255 ) },
	CVector3<BYTE>{ BYTE( 128 ), BYTE( 0 ), BYTE( 0 ) }
};
//////////////////////////////////////////////////////////////////////////

CSamplerObject CBaseNoiseTable::getOrCreateSampler()
{
	if( !tableSampler.IsValid() ) {
		initSamplerObject();
	}
	assert( tableSampler.IsValid() );
	return tableSampler;
}

void CBaseNoiseTable::initSamplerObject()
{
	static CSamplerOwner noiseSampler;
	noiseSampler.SetMagFilter( TF_Nearest );
	noiseSampler.SetMinFilter( TF_Nearest );
	noiseSampler.SetTextureWrap( TWD_WrapS, TWM_Repeat );
	noiseSampler.SetTextureWrap( TWD_WrapT, TWM_Repeat );
	tableSampler = noiseSampler;
}

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

CNoisePermutationTable2D::CNoisePermutationTable2D() :
	tableTexture( getOrCreateSampler() )
{
	CTextureBinder binder( tableTexture );
	tableTexture.SetBuffer( permTableSize );
}

void CNoisePermutationTable2D::FillTable( CRandomGenerator& rng )
{
	static const int permTableArea = permTableRowSize * permTableRowSize;
	CVector3<BYTE> permData[permTableArea];
	for( int i = 0; i < permTableArea; i++ ) {
		const int gradIndex = rng.RandomNumber( 0, AllowedGradients().Size() - 1 );
		permData[i].X() = AllowedGradients()[gradIndex].X();
		permData[i].Y() = AllowedGradients()[gradIndex].Y();
		permData[i].Z() = numeric_cast<BYTE>( rng.RandomNumber( 0, UCHAR_MAX ) );
	}

	CTextureBinder binder( tableTexture );
	tableTexture.SetSubData( CVector2<int>{}, permData, permTableSize, 0, TF_RGB, TDT_UnsignedByte );
}

//////////////////////////////////////////////////////////////////////////

CNoisePermutationTable3D::CNoisePermutationTable3D() :
	tableTexture( getOrCreateSampler() )
{
	CTextureBinder binder( tableTexture );
	tableTexture.SetBuffer( permTableSize );
}

void CNoisePermutationTable3D::FillTable( CRandomGenerator& rng )
{
	static const int perTableArea = permTableRowSize * permTableRowSize;
	CVector4<BYTE> permData[perTableArea];
	for( int i = 0; i < perTableArea; i++ ) {
		const int gradIndex = rng.RandomNumber( 0, AllowedGradients().Size() - 1 );
		permData[i].X() = AllowedGradients()[gradIndex].X();
		permData[i].Y() = AllowedGradients()[gradIndex].Y();
		permData[i].Z() = AllowedGradients()[gradIndex].Z();
		permData[i].W() = numeric_cast<BYTE>( rng.RandomNumber( 0, UCHAR_MAX ) );

	}

	CTextureBinder binder( tableTexture );
	tableTexture.SetSubData( CVector2<int>{}, permData, permTableSize, 0, TF_RGBA, TDT_UnsignedByte );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
