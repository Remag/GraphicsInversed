#pragma once
#include <Gindefs.h>

//////////////////////////////////////////////////////////////////////////

namespace Gin {

enum TIlluminationModel;

// Common material attributes.
namespace Material {

class ComponentClass : public CBaseComponentClass {};

template <class T>
using CMaterialComponent = CInlineComponent<T, ComponentClass>;

// Components.
extern GINAPI const CMaterialComponent<CVector3<float>> AmbientColor;
extern GINAPI const CMaterialComponent<CVector3<float>> DiffuseColor;
extern GINAPI const CMaterialComponent<CVector3<float>> SpecularColor;
extern GINAPI const CMaterialComponent<float> SpecularExponent;
extern GINAPI const CMaterialComponent<float> DissolveValue;
extern GINAPI const CMaterialComponent<Gin::TIlluminationModel> IlluminationModel;

}	// namespace Material.

// Common light source attributes.
namespace LightSource {

class ComponentClass : public CBaseComponentClass {};

template <class T>
using CLightComponent = CInlineComponent<T, ComponentClass>;

// Components.
extern GINAPI const CLightComponent<CVector3<float>> LightIntensity;
extern GINAPI const CLightComponent<CVector4<float>> LightVector;
extern GINAPI const CLightComponent<float> AmbientFactor;

}	// namespace LightSource.

// Common vertex shader attributes.
namespace Vertex {

class ComponentClass : public CBaseComponentClass {};

template <class T>
using CVertexComponent = CInlineComponent<T, ComponentClass>;

}

//////////////////////////////////////////////////////////////////////////

typedef CInlineEntityRef<Material::ComponentClass> TMaterialRef;
typedef CInlineEntityConstRef<Material::ComponentClass> TMaterialConstRef;
typedef CInlineEntityOwner<Material::ComponentClass> TMaterialOwner;
typedef CInlineEntityRef<LightSource::ComponentClass> TLightSourceRef;
typedef CInlineEntityConstRef<LightSource::ComponentClass> TLightSourceConstRef;
typedef CInlineEntityOwner<LightSource::ComponentClass> TLightSourceOwner;
typedef CInlineEntityRef<Vertex::ComponentClass> TVertexRef;
typedef CInlineEntityConstRef<Vertex::ComponentClass> TVertexConstRef;
typedef CInlineEntityOwner<Vertex::ComponentClass> TVertexOwner;

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
