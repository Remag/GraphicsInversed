#pragma once
// File containing utility structures and enumerations for DDS files support.

namespace Gin {

namespace DDS {

//////////////////////////////////////////////////////////////////////////

// DDS file pixel format. Taken from MSDN documentation.
struct CDdsPixelFormat {
	DWORD Size;
	DWORD Flags;
	DWORD FourCC;
	DWORD RGBBitCount;
	DWORD RBitMask;
	DWORD GBitMask;
	DWORD BBitMask;
	DWORD ABitMask;
};

// PixelFormat flags.
enum TDdsPixelFormatFlag {
	// Texture contains alpha data; dwRGBAlphaBitMask contains valid data.
	DPFF_AlphaPixels = 0x1,
	// Used in some older DDS files for alpha channel only uncompressed data
	// ( dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data ).
	DPFF_Alpha = 0x2,
	// Texture contains compressed RGB data; dwFourCC contains valid data.
	DPFF_FourCC = 0x4,
	// Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks ( dwRBitMask, dwGBitMask, dwBBitMask ) contain valid data.
	DPFF_RGB = 0x40,
	// Used in some older DDS files for YUV uncompressed data 
	// ( dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask ).
	DPFF_YUV = 0x200,
	// Used in some older DDS files for single channel color uncompressed data
	// ( dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask ).
	// Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file.
	DPFF_Luminance = 0x20000
};

// Possible compression types.
enum TDdsFourCCFlag {
	DCF_DXT1 = 0x31545844,
	DCF_DXT3 = 0x33545844,
	DCF_DXT5 = 0x35545844,
	DCF_DX10 = 0x30315844
};

//////////////////////////////////////////////////////////////////////////

// General header of a DDS file. Taken from MSDN documentation.
struct CDdsHeader {
	DWORD Size;
	DWORD Flags;
	DWORD Height;
	DWORD Width;
	DWORD PitchOrLinearSize;
	DWORD Depth;
	DWORD MipMapCount;
	DWORD Reserved1[11];
	CDdsPixelFormat PixelFormat;
	DWORD Caps;
	DWORD Caps2;
	DWORD Caps3;
	DWORD Caps4;
	DWORD Reserved2;
};

// DdsHeaderFlag flags.
enum TDdsHeaderFlag {
	// Required in every DDS file.
	DHF_Caps = 0x1,
	// Required in every DDS file.
	DHF_Height = 0x2,
	// Required in every DDS file.
	DHF_Width = 0x4,
	// Required when pitch is provided for an uncompressed texture.
	DHF_Pitch = 0x8,
	// Required in every DDS file.
	DHF_PixelFormat = 0x1000,
	// Required in a mipmapped texture.
	DHF_MimmapCount = 0x20000,
	// Required when pitch is provided for a compressed texture.	
	DHF_Linearize = 0x80000,
	// Required in a depth texture.
	DHF_Depth = 0x800000,
};

enum TDdsCaps1Flag {
	// Must be used on any file that contains more than one surface ( a mipmap, a cubic environment map, or mipmapped volume texture ).
	DC1F_Complex = 0x8,
	// Optional; should be used for a mipmap.
	DC1F_Mipmap = 0x400000,
	// Required.
	DC1F_Texture = 0x1000
};

enum TDdsCaps2Flag {
	DC2F_Cubemap = 0x200,
	DC2F_Cubemap_PositiveX = 0x400,
	DC2F_Cubemap_NegativeX = 0x800,
	DC2F_Cubemap_PositiveY = 0x1000,
	DC2F_Cubemap_NegativeY = 0x2000,
	DC2F_Cubemap_PositiveZ = 0x4000,
	DC2F_Cubemap_NegativeZ = 0x8000,
	DC2F_Cubemap_AllFaces = 0x400 | 0x800 | 0x1000 | 0x2000 | 0x4000 | 0x8000,
	DC2F_Volume = 0x200000
};

//////////////////////////////////////////////////////////////////////////

// Resource data formats.
enum TDXGIFormat {
	DXGIF_Unknown = 0,
	DXGIF_R32G32B32A32_TYPELESS,
	DXGIF_R32G32B32A32_FLOAT,
	DXGIF_R32G32B32A32_UINT,
	DXGIF_R32G32B32A32_SINT,
	DXGIF_R32G32B32_TYPELESS,
	DXGIF_R32G32B32_FLOAT,
	DXGIF_R32G32B32_UINT,
	DXGIF_R32G32B32_SINT,
	DXGIF_R16G16B16A16_TYPELESS,
	DXGIF_R16G16B16A16_FLOAT,
	DXGIF_R16G16B16A16_UNORM,
	DXGIF_R16G16B16A16_UINT,
	DXGIF_R16G16B16A16_SNORM,
	DXGIF_R16G16B16A16_SINT,
	DXGIF_R32G32_TYPELESS,
	DXGIF_R32G32_FLOAT,
	DXGIF_R32G32_UINT,
	DXGIF_R32G32_SINT,
	DXGIF_R32G8X24_TYPELESS,
	DXGIF_D32_FLOAT_S8X24_UINT,
	DXGIF_R32_FLOAT_X8X24_TYPELESS,
	DXGIF_X32_TYPELESS_G8X24_UINT,
	DXGIF_R10G10B10A2_TYPELESS,
	DXGIF_R10G10B10A2_UNORM,
	DXGIF_R10G10B10A2_UINT,
	DXGIF_R11G11B10_FLOAT,
	DXGIF_R8G8B8A8_TYPELESS,
	DXGIF_R8G8B8A8_UNORM,
	DXGIF_R8G8B8A8_UNORM_SRGB,
	DXGIF_R8G8B8A8_UINT,
	DXGIF_R8G8B8A8_SNORM,
	DXGIF_R8G8B8A8_SINT,
	DXGIF_R16G16_TYPELESS,
	DXGIF_R16G16_FLOAT,
	DXGIF_R16G16_UNORM,
	DXGIF_R16G16_UINT,
	DXGIF_R16G16_SNORM,
	DXGIF_R16G16_SINT,
	DXGIF_R32_TYPELESS,
	DXGIF_D32_FLOAT,
	DXGIF_R32_FLOAT,
	DXGIF_R32_UINT,
	DXGIF_R32_SINT,
	DXGIF_R24G8_TYPELESS,
	DXGIF_D24_UNORM_S8_UINT,
	DXGIF_R24_UNORM_X8_TYPELESS,
	DXGIF_X24_TYPELESS_G8_UINT,
	DXGIF_R8G8_TYPELESS,
	DXGIF_R8G8_UNORM,
	DXGIF_R8G8_UINT,
	DXGIF_R8G8_SNORM,
	DXGIF_R8G8_SINT,
	DXGIF_R16_TYPELESS,
	DXGIF_R16_FLOAT,
	DXGIF_D16_UNORM,
	DXGIF_R16_UNORM,
	DXGIF_R16_UINT,
	DXGIF_R16_SNORM,
	DXGIF_R16_SINT,
	DXGIF_R8_TYPELESS,
	DXGIF_R8_UNORM,
	DXGIF_R8_UINT,
	DXGIF_R8_SNORM,
	DXGIF_R8_SINT,
	DXGIF_A8_UNORM,
	DXGIF_R1_UNORM,
	DXGIF_R9G9B9E5_SHAREDEXP,
	DXGIF_R8G8_B8G8_UNORM,
	DXGIF_G8R8_G8B8_UNORM,
	DXGIF_BC1_TYPELESS,
	DXGIF_BC1_UNORM,
	DXGIF_BC1_UNORM_SRGB,
	DXGIF_BC2_TYPELESS,
	DXGIF_BC2_UNORM,
	DXGIF_BC2_UNORM_SRGB,
	DXGIF_BC3_TYPELESS,
	DXGIF_BC3_UNORM,
	DXGIF_BC3_UNORM_SRGB,
	DXGIF_BC4_TYPELESS,
	DXGIF_BC4_UNORM,
	DXGIF_BC4_SNORM,
	DXGIF_BC5_TYPELESS,
	DXGIF_BC5_UNORM,
	DXGIF_BC5_SNORM,
	DXGIF_B5G6R5_UNORM,
	DXGIF_B5G5R5A1_UNORM,
	DXGIF_B8G8R8A8_UNORM,
	DXGIF_B8G8R8X8_UNORM,
	DXGIF_R10G10B10_XR_BIAS_A2_UNORM,
	DXGIF_B8G8R8A8_TYPELESS,
	DXGIF_B8G8R8A8_UNORM_SRGB,
	DXGIF_B8G8R8X8_TYPELESS,
	DXGIF_B8G8R8X8_UNORM_SRGB,
	DXGIF_BC6H_TYPELESS,
	DXGIF_BC6H_UF16,
	DXGIF_BC6H_SF16,
	DXGIF_BC7_TYPELESS,
	DXGIF_BC7_UNORM,
	DXGIF_BC7_UNORM_SRGB,
	DXGIF_AYUV,
	DXGIF_Y410,
	DXGIF_Y416,
	DXGIF_NV12,
	DXGIF_P010,
	DXGIF_P016,
	DXGIF_420_OPAQUE,
	DXGIF_YUY2,
	DXGIF_Y210,
	DXGIF_Y216,
	DXGIF_NV11,
	DXGIF_AI44,
	DXGIF_IA44,
	DXGIF_P8,
	DXGIF_A8P8,
	DXGIF_B4G4R4A4_UNORM,
	DXGIF_FORCE_UINT = 0xffffffffUL
};

enum TD3D10ResourceDimension {
	D3D10RD_Unknown,
	D3D10RD_Texture1D,
	D3D10RD_Texture2D,
	D3D10RD_Texture3D
};

// Optional header with additional DDS information. Taken from MSDN documentation.
struct CDxt10Header {
	TDXGIFormat DxgiFormat;
	TD3D10ResourceDimension ResourceDimension;
	UINT MiscFlag;
	UINT ArraySize;
	UINT MiscFlags2;
};

enum TDxtMiscFlag {
	// Indicates a 2D texture is a cube-map texture.
	DMF_TextureCube = 0x4
};

//////////////////////////////////////////////////////////////////////////

// Class that maps a DXGI format to values in a DxHeader.
struct CDXGIToBitMaskEntry {
	TDXGIFormat Format;
	DWORD Flags;
	DWORD RgbBitCount;
	DWORD RBitMask;
	DWORD GBitMask;
	DWORD BBitMask;
	DWORD ABitMask;
	DWORD FourCC;
};

}	// namespace DDS.

}	// namespace Gin.

