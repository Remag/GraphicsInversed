#include <common.h>
#pragma hdrstop

#include <GinComponents.h>
#include <InputSettings.h>
#include <BoundUserInputAction.h>
#include <ConsoleSystem.h>
#include <DdsImage.h>

#pragma warning( disable : 4074 )
// Global data should be initialized as fast as possible. Next directive sets the highest priority to the current translation unit.
#pragma init_seg( compiler )

namespace Gin {

CArray<CPair<CStringView, TGinVirtualKey>> CInputSettings::keyNameCodePairs = CInputSettings::CreateKeyNamePairs();
CArray<CStringView> CInputSettings::codeToKeyName = CInputSettings::CreateCodeToKeyNameArray();
CMap<CStringView, TGinVirtualKey, CCaselessStringHash> CInputSettings::keyNameToCode = CInputSettings::CreateKeyNameToCodeMap();

extern const CError Err_GeneralGlError{ L"General OpenGL error! Error code: %0." };
const CUnicodeView CDdsException::generalDdsFileError = L"DDS parsing error: %1.\nFile name: %0";

namespace GinInternal {

// Binding point for uniform blocks.
CAtomic<int> CurrentUniformBindingPointIndex( 0 );

CMap<const type_info*, int> CActionTargetController::targetTypeToIndex;
CArray<void*> CActionTargetController::actionTargets;

// Default input translator.
extern const CInputTranslator DefaultTranslator{};

GINAPI CConsoleSystem GinGlobalConsoleSystem;

// Created input binding lists.
GINAPI CArray<CInputBinding*> ActiveFileKeyBindings;
GINAPI CArray<CSystemInputBinding*> ActiveBasicKeyBindings;

}	// namespace GinInternal.

// Material components.
namespace Material {

extern GINAPI const CMaterialComponent<CVector3<float>> AmbientColor{ "AmbientColor" };
extern GINAPI const CMaterialComponent<CVector3<float>> DiffuseColor{ "DiffuseColor" };
extern GINAPI const CMaterialComponent<CVector3<float>> SpecularColor{ "SpecularColor" };
extern GINAPI const CMaterialComponent<float> SpecularExponent{ "SpecularExponent" };
extern GINAPI const CMaterialComponent<float> DissolveValue{ "DissolveValue" };
extern GINAPI const CMaterialComponent<Gin::TIlluminationModel> IlluminationModel{ "IlluminationModel" };

}	// namespace Material.

// Light source components.
namespace LightSource {

extern GINAPI const CLightComponent<CVector3<float>> LightIntensity{ "LightIntensity" };
extern GINAPI const CLightComponent<CVector4<float>> LightVector{ "LightVector" };
extern GINAPI const CLightComponent<float> AmbientFactor{ "AmbientFactor" };

}	// namespace LightSource.

// Vertex shader parameter components.
namespace Vertex {

}

}	// namespace Gin.

// Export a hint variable to force the use of a high performance video card over the integrated graphics.
extern "C" {
	__declspec( dllexport ) DWORD NvOptimusEnablement = 0x00000001;	// NVIDIA
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;	// AMD
}