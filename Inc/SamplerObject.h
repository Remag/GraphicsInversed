#pragma once
#include <Gindefs.h>
#include <DrawEnums.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Class containing texture settings. Can be bound to a texture.
class GINAPI CSamplerObject {
public:
	CSamplerObject() = default;
	explicit CSamplerObject( unsigned id ) : samplerId( id ) {}
 
	bool IsValid() const
		{ return samplerId != 0; }
	int GetId() const
		{ return samplerId; }

private:
	unsigned samplerId = 0;
};

//////////////////////////////////////////////////////////////////////////

// Sampler id owning wrapper.
class GINAPI CSamplerOwner {
public:
	CSamplerOwner();
	CSamplerOwner( CSamplerOwner&& other );
	CSamplerOwner& operator=( CSamplerOwner&& other );

	~CSamplerOwner();

	bool IsValid() const
		{ return samplerId != 0; }
	int GetId() const
		{ return samplerId; }

	operator CSamplerObject() const
		{ return CSamplerObject( GetId() ); }

	void SetMinFilter( TTextureFilter value );
	void SetMagFilter( TTextureFilter value );

	// Set the wrap mode for the given dimension.
	void SetTextureWrap( TTextureWrapDim dim, TTextureWrapMode mode );

	void DeleteSampler();

private:
	unsigned samplerId = 0;
	static unsigned createSampler();

	// Copying is prohibited.
	CSamplerOwner( CSamplerOwner& ) = delete;
	void operator=( CSamplerOwner& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

