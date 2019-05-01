#pragma once
#include <TextureOwner.h>
#include <TextureWrappers.h>

namespace Gin {

namespace GinInternal {

class GINAPI CBaseNoiseTable {
public:
	static int TableRowSize()
		{ return permTableRowSize; }
	static const CStackArray<CVector3<BYTE>, 12>& AllowedGradients()
		{ return allowedGradients; }

protected:
	static const int permTableRowSize = 256;
	static const CVector2<int> permTableSize;

	static CSamplerObject getOrCreateSampler();
	
private:
	static CSamplerObject tableSampler;

	static const CStackArray<CVector3<BYTE>, 12> allowedGradients;
	static void initSamplerObject();
};

}	// namespace GinInternal.

//////////////////////////////////////////////////////////////////////////

// Texture containing random gradient values for the gradient noise.
class GINAPI CNoisePermutationTable2D : public GinInternal::CBaseNoiseTable {
public:
	// Create an empty table.
	CNoisePermutationTable2D();

	CTexture<TBT_Texture2, TGF_RGB> GetTable() const
		{ return tableTexture; }

	// Fill table for usage with 2D gradient noise. Old values are discarded.
	// Red and green components are filled with one of 12 gradient values.
	// Blue component is filled with random numbers.
	void FillTable( CRandomGenerator& rng );

private:
	CTextureOwner<TBT_Texture2, TGF_RGB> tableTexture;
};

// Texture containing random gradient values for the gradient noise.
class GINAPI CNoisePermutationTable3D : public GinInternal::CBaseNoiseTable {
public:
	// Create an empty table.
	CNoisePermutationTable3D();

	CTexture<TBT_Texture2, TGF_RGBA> GetTable() const
		{ return tableTexture; }

	// Fill table for usage with 3D gradient noise. Old values are discarded.
	// Red, green and blue components are filled with one of 12 gradient values.
	// Alpha component is filled with random numbers.
	void FillTable( CRandomGenerator& rng );
	
private:
	CTextureOwner<TBT_Texture2, TGF_RGBA> tableTexture;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

