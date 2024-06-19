// Drawing utility enumerations.
#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Common location for shader arguments.
enum TShaderAttributeLocations {
	SAL_Position = 0,	// location for vertex positions.
};

// Type of the OpenGL buffer.
enum TBufferType {
	BT_Undefined,
	BT_Array = 0x8892,	// gl::ARRAY_BUFFER
	BT_AtomicCounter = 0x92C0,	// gl::ATOMIC_COUNTER_BUFFER
	BT_CopyRead = 0x8F36,	// gl::COPY_READ_BUFFER
	BT_CopyWrite = 0x8F37,	// gl::COPY_WRITE_BUFFER
	BT_DrawIndirect = 0x8F3F,	// gl::DRAW_INDIRECT_BUFFER
	BT_DispatchIndirect = 0x90EE,	// gl::DISPATCH_INDIRECT_BUFFER
	BT_ElementArray = 0x8893,	// gl::ELEMENT_ARRAY_BUFFER 
	BT_PixelPack = 0x88EB,	// gl::PIXEL_PACK_BUFFER 
	BT_PixelUnpack = 0x88EC,	// gl::PIXEL_UNPACK_BUFFER
	BT_Query = 0x9192,	// gl::QUERY_BUFFER
	BT_ShaderStorage = 0x90D2,	// gl::SHADER_STORAGE_BUFFER 
	BT_Texture = 0x8C2A,	// gl::TEXTURE_BUFFER
	BT_TransformFeedback = 0x8C8E,	// gl::TRANSFORM_FEEDBACK_BUFFER
	BT_Uniform = 0x8A11	// gl::UNIFORM_BUFFER
};

// Way to use a buffer.
enum TBufferUsageHint {
	BUH_StaticDraw = 0x88E4,	// gl::STATIC_DRAW
	BUH_DynamicDraw = 0x88E8,	// gl::DYNAMIC_DRAW
	BUH_StreamDraw = 0x88E0,	// gl::STREAM_DRAW
	BUH_StaticRead = 0x88E5,	// gl::STATIC_READ
	BUH_DynamicRead = 0x88E9,	// gl::DYNAMIC_READ
	BUH_StreamRead = 0x88E1,	// gl::STREAM_READ
	BUH_StaticCopy = 0x88E6,	// gl::STATIC_COPY
	BUH_DynamicCopy = 0x88EA,	// gl::DYNAMIC_COPY
	BUH_StreamCopy = 0x88E2		// gl::STREAM_COPY
};

// Mapping mode to use when directly writing to a buffer.
enum TBufferWriteMappingMode {
	BWMM_Write = 0x88B9,	// gl::WRITE_ONLY
	BWMM_ReadWrite = 0x88BA	// gl::READ_WRITE
};

// The way to interpret mesh data.
enum TMeshDrawMode {
	MDM_Points = 0x0000,	// gl::POINTS
	MDM_LineStrip = 0x0003,	// gl::LINE_STRIP
	MDM_LineLoop = 0x0002,	// gl::LINE_LOOP
	MDM_Lines = 0x0001,	// gl::LINES
	MDM_LineStripAdjacency = 0x000B,	// gl::LINE_STRIP_ADJACENCY
	MDM_LinesAdjacency = 0x000A,	// gl::LINES_ADJACENCY
	MDM_TriangleStrip = 0x0005,	// gl::TRIANGLE_STRIP
	MDM_TriangleFan = 0x0006,	// gl::TRIANGLE_FAN
	MDM_Triangles = 0x0004,	// gl::TRIANGLES
	MDM_TriangleStripAdjacency = 0x000D,	// gl::TRIANGLE_STRIP_ADJACENCY
	MDM_TrianglesAdjacency = 0x000C,	// gl::TRIANGLES_ADJACENCY
	MDM_Patches = 0x000E	// gl::PATCHES
};

// Data types supported by OpenGL.
enum TGlType {
	GLT_Undefined = -1,
	GLT_UnsignedByte = 0x1401,	// gl::UNSIGNED_BYTE
	GLT_UnsignedShort = 0x1403,	// gl::UNSIGNED_SHORT
	GLT_Int = 0x1404,	// gl::INT
	GLT_UnsignedInt = 0x1405,	// gl::UNSIGNED_INT
	GLT_Float = 0x1406,	// gl::FLOAT
	GLT_Bool = 0x8B56,	// gl::BOOL
	GLT_Double = 0x140A,	// gl::DOUBLE
	GLT_Vec2Int = 0x8B53,	// gl::INT_VEC2
	GLT_Vec3Int = 0x8B54,	// gl::INT_VEC3
	GLT_Vec4Int = 0x8B55,	// gl::INT_VEC4
	GLT_Vec2Float = 0x8B50,	// gl::FLOAT_VEC2
	GLT_Vec3Float = 0x8B51,	// gl::FLOAT_VEC3
	GLT_Vec4Float = 0x8B52,	// gl::FLOAT_VEC4
	GLT_Mat2 = 0x8B5A,	// gl::FLOAT_MAT2
	GLT_Mat2x3 = 0x8B65,	// gl::FLOAT_MAT2x3
	GLT_Mat3x2 = 0x8B67,	// gl::FLOAT_MAT3x2
	GLT_Mat3 = 0x8B5B,	//gl::FLOAT_MAT3
	GLT_Mat3x4 = 0x8B68,	// gl::FLOAT_MAT3x4
	GLT_Mat4x3 = 0x8B6A,	// gl::FLOAT_MAT4x3
	GLT_Mat4 = 0x8B5C,	// gl::FLOAT_MAT4
	GLT_Texture1 = 0x8B5D,	// gl::SAMPLER_1D
	GLT_Texture2 = 0x8B5E,	// gl::SAMPLER_2D
	GLT_Texture3 = 0x8B5F,	// gl::SAMPLER_3D
	GLT_TextureCube = 0x8B60,	// gl::SAMPLER_CUBE
	GLT_TextureArray1 = 0x8DC0,	// gl::SAMPLER_1D_ARRAY
	GLT_TextureArray2 = 0x8DC1	// gl::SAMPLER_2D_ARRAY
};

//////////////////////////////////////////////////////////////////////////

// GLSL block layout.
enum TBlockLayout {
	BL_Std140,
	BL_Shared,
	BL_Packed
};

//////////////////////////////////////////////////////////////////////////

// Texture format used by OpenGL.
enum TTextureGlFormat {
	TGF_Red = 0x1903,	// gl::RED
	TGF_RG = 0x8227,	// gl::RG
	TGF_RGB = 0x1907,	// gl::RGB
	TGF_RGBA = 0x1908,	// gl::RGBA
	TGF_R8 = 0x8229,	// gl::R8
	TGF_R16 = 0x822A,	// gl::R16
	TGF_RG8 = 0x822B,	// gl::RG8
	TGF_RG16 = 0x822C,	// gl::RG16
	TGF_RGB8 = 0x8051,	// gl::RGB8
	TGF_RGBA8 = 0x8058,	// gl::RGBA8
	TGF_SRGB8 = 0x8C41,	// gl::SRGB8
	TGF_SRGBA8 = 0x8C43,	// gl::SRGB8_ALPHA8
	TGF_RGB32F = 0x8815,	// gl::RGB32F
	TGF_RGBA32F = 0x8814,	// gl::RGBA32F
	TGF_Depth = 0x1902,	// gl::DEPTH_COMPONENT
	TGF_Stencil = 0x1901,	// gl::STENCIL_INDEX
	TGF_DepthStencil = 0x84F9	// gl::DEPTH_STENCIL
};

// Cube face index.
enum TTextureCubeFace {
	TCF_PositiveX = 0x8515,	// gl::TEXTURE_CUBE_MAP_POSITIVE_X
	TCF_NegativeX = 0x8516,	// gl::TEXTURE_CUBE_MAP_NEGATIVE_X
	TCF_PositiveY = 0x8517,	// gl::TEXTURE_CUBE_MAP_POSITIVE_Y
	TCF_NegativeY = 0x8518,	// gl::TEXTURE_CUBE_MAP_NEGATIVE_Y
	TCF_PositiveZ = 0x8519,	// gl::TEXTURE_CUBE_MAP_POSITIVE_Z
	TCF_NegativeZ = 0x851A	// gl::TEXTURE_CUBE_MAP_NEGATIVE_Z
};

//////////////////////////////////////////////////////////////////////////

// Texture format, used by the CPU.
enum TTexelFormat {
	TF_Compressed = -1,
	TF_Red = 0x1903,	// gl::RED
	TF_RG = 0x8227,	// gl::RG
	TF_RGB = 0x1907,	// gl::RGB
	TF_BGR = 0x80E0,	// gl::BGR
	TF_RGBA = 0x1908,	// gl::RGBA
	TF_BGRA = 0x80E1,	// gl::BGRA
	TF_RedInt = 0x8D94,	// gl::RED_INTEGER
	TF_RGInt = 0x8228,	// gl::RG_INTEGER
	TF_RGBInt = 0x8D98,	// gl::RGB_INTEGER
	TF_BGRInt = 0x8D9A,	// gl::BGR_INTEGER
	TF_RGBAInt = 0x8D99,	// gl::RGBA_INTEGER
	TF_BGRAInt = 0x8D9B,	// gl::BGRA_INTEGER
	TF_StencilIndex = 0x1901,	// gl::STENCIL_INDEX
	TF_Depth = 0x1902,	// gl::DEPTH_COMPONENT
	TF_DepthStencil = 0x84F9	// gl::DEPTH_STENCIL
};

// Format of a pixel, used by the CPU.
enum TTexelDataType {
	TDT_Compressed = NotFound,
	TDT_UnsignedByte = 0x1401,	// gl::UNSIGNED_BYTE
	TDT_Byte = 0x1400,	// gl::BYTE
	TDT_UnsignedShort = 0x1403,	// gl::UNSIGNED_SHORT
	TDT_Short = 0x1402,	// gl::SHORT
	TDT_UnsignedInt = 0x1405,	// gl::UNSIGNED_INT
	TDT_Int = 0x1404,	// gl::INT
	TDT_Float = 0x1406,	// gl::FLOAT
	TDT_UnsignedByte332 = 0x8032,	// gl::UNSIGNED_BYTE_3_3_2
	TDT_UnsingedByte233Rev = 0x8362,	// gl::UNSIGNED_BYTE_2_3_3_REV
	TDT_UnsignedShort565 = 0x8363,	// gl::UNSIGNED_SHORT_5_6_5
	TDT_UnsignedShort565Rev = 0x8364,	// gl::UNSIGNED_SHORT_5_6_5_REV
	TDT_UnsignedShort4444 = 0x8033,	// gl::UNSIGNED_SHORT_4_4_4_4
	TDT_UnsignedShort4444Rev = 0x8365,	// gl::UNSIGNED_SHORT_4_4_4_4_REV
	TDT_UnsignedShort5551 = 0x8034,	// gl::UNSIGNED_SHORT_5_5_5_1
	TDT_UnsignedShort1555Rev = 0x8366,	// gl::UNSIGNED_SHORT_1_5_5_5_REV
	TDT_UnsignedInt8888 = 0x8035,	// gl::UNSIGNED_INT_8_8_8_8
	TDT_UnsignedInt8888Rev = 0x8367,	// gl::UNSIGNED_INT_8_8_8_8_REV
	TDT_UnsignedInt1010102 = 0x8036,	// gl::UNSIGNED_INT_10_10_10_2
	TDT_UnsignedInt2101010Rev = 0x8368	// gl::UNSIGNED_INT_2_10_10_10_REV
};

//////////////////////////////////////////////////////////////////////////

// Texture filtering mode.
enum TTextureFilter {
	TF_Nearest = 0x2600,	// gl::NEAREST
	TF_Linear = 0x2601,	// gl::LINEAR
	TF_NearestMipmapNearest = 0x2700,	// gl::NEAREST_MIPMAP_NEAREST
	TF_LinearMipmapNearest = 0x2701,	// gl::LINEAR_MIPMAP_NEAREST
	TF_NearestMipmapLinear = 0x2702,	// gl::NEAREST_MIPMAP_LINEAR
	TF_LinearMipmapLinear = 0x2703	// gl::LINEAR_MIPMAP_LINEAR
};

//////////////////////////////////////////////////////////////////////////

// Texture wrapping dimension.
enum TTextureWrapDim {
	TWD_WrapS = 0x2802,	// gl::TEXTURE_WRAP_S
	TWD_WrapT = 0x2803,	// gl::TEXTURE_WRAP_T
	TWD_WrapR = 0x8072	// gl::TEXTURE_WRAP_R
};

// Texture wrapping mode.
enum TTextureWrapMode {
	TWM_ClampToEdge = 0x812F,	// gl::CLAMP_TO_EDGE
	TWM_ClampToBorder = 0x812D,	// gl::CLAMP_TO_BORDER
	TWM_Repeat = 0x2901,	// gl::REPEAT
	TWM_MirrorRepeat = 0x8370,	// gl::MIRRORED_REPEAT
	TWM_MirrorClampToEdge = 0x8743	// gl::MIRROR_CLAMP_TO_EDGE
};

// Target for texture binding.
enum TTextureBindingTarget {
	TBT_Texture1 = 0x0DE0,	// gl::TEXTURE_1D
	TBT_Texture2 = 0x0DE1,	// gl::TEXTURE_2D
	TBT_TextureArray1 = 0x8C18,	// gl::TEXTURE_1D_ARRAY
	TBT_TextureArray2 = 0x8C1A,	// gl::TEXTURE_2D_ARRAY
	TBT_CubeMap = 0x8513	// gl::TEXTURE_CUBE_MAP
};

//////////////////////////////////////////////////////////////////////////

// Supported compression types for the texture.
enum TTextureCompressionType {
	TCT_Uncompressed,
	TCT_Dxt1_RGB = 0x83F0,	// gl::COMPRESSED_RGB_S3TC_DXT1_EXT
	TCT_Dxt3 = 0x83F2,	// gl::COMPRESSED_RGBA_S3TC_DXT3_EXT
	TCT_Dxt5 = 0x83F3,	// gl::COMPRESSED_RGBA_S3TC_DXT5_EXT
	TCT_Dxt1_sRGB = 0x8C4C,	// gl::COMPRESSED_SRGB_S3TC_DXT1_EXT
	TCT_Dxt3_sRGBA = 0x8C4E,	// gl::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
	TCT_Dxt5_sRGBA = 0x8C4F	// gl::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
};

// Supported texture types.
enum TTextureType {
	TT_Texture1D,
	TT_Texture2D,
	TT_Texture3D,
	TT_TextureCubeMap
};

// Type of a system alignment
enum TAlignmentType {
	AT_Pack = 0x0D05,	// gl::PACK_ALIGNMENT
	AT_Unpack = 0x0CF5	// gl::UNPACK_ALIGNMENT
};

// OpenGL boolean type.
enum TGlBool {
	GB_True = 1,	// gl::TRUE_
	GB_False = 0	// gl::FALSE_
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
