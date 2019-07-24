// MIT license see full LICENSE text at end of file
#pragma once
#ifndef TINY_DDS_TINYDDS_H
#define TINY_DDS_TINYDDS_H

#ifndef TINYDDS_HAVE_UINTXX_T
#include <stdint.h>  // for uint32_t and int64_t
#endif
#ifndef TINYDDS_HAVE_BOOL
#include <stdbool.h>  // for bool
#endif
#ifndef TINYDDS_HAVE_SIZE_T
#include <stddef.h>    // for size_t
#endif
#ifndef TINYDDS_HAVE_MEMCPY
#include <string.h>  // for memcpy
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TINYDDS_MAX_MIPMAPLEVELS 16

typedef struct TinyDDS_Context *TinyDDS_ContextHandle;

typedef void *(*TinyDDS_AllocFunc)(void *user, size_t size);
typedef void (*TinyDDS_FreeFunc)(void *user, void *memory);
typedef size_t (*TinyDDS_ReadFunc)(void *user, void *buffer, size_t byteCount);
typedef bool (*TinyDDS_SeekFunc)(void *user, int64_t offset);
typedef int64_t (*TinyDDS_TellFunc)(void *user);
typedef void (*TinyDDS_ErrorFunc)(void *user, char const *msg);

typedef struct TinyDDS_Callbacks {
	TinyDDS_ErrorFunc error;
	TinyDDS_AllocFunc alloc;
	TinyDDS_FreeFunc free;
	TinyDDS_ReadFunc read;
	TinyDDS_SeekFunc seek;
	TinyDDS_TellFunc tell;
} TinyDDS_Callbacks;

TinyDDS_ContextHandle TinyDDS_CreateContext(TinyDDS_Callbacks const *callbacks, void *user);
void TinyDDS_DestroyContext(TinyDDS_ContextHandle handle);

// reset lets you reuse the context for another file (saves an alloc/free cycle)
void TinyDDS_Reset(TinyDDS_ContextHandle handle);

// call this to read the header file should already be at the start of the KTX data
bool TinyDDS_ReadHeader(TinyDDS_ContextHandle handle);

bool TinyDDS_Is1D(TinyDDS_ContextHandle handle);
bool TinyDDS_Is2D(TinyDDS_ContextHandle handle);
bool TinyDDS_Is3D(TinyDDS_ContextHandle handle);
bool TinyDDS_IsCubemap(TinyDDS_ContextHandle handle);
bool TinyDDS_IsArray(TinyDDS_ContextHandle handle);

bool TinyDDS_Dimensions(TinyDDS_ContextHandle handle,
												uint32_t *width,
												uint32_t *height,
												uint32_t *depth,
												uint32_t *slices);
uint32_t TinyDDS_Width(TinyDDS_ContextHandle handle);
uint32_t TinyDDS_Height(TinyDDS_ContextHandle handle);
uint32_t TinyDDS_Depth(TinyDDS_ContextHandle handle);
uint32_t TinyDDS_ArraySlices(TinyDDS_ContextHandle handle);

bool TinyDDS_NeedsGenerationOfMipmaps(TinyDDS_ContextHandle handle);
bool TinyDDS_NeedsEndianCorrecting(TinyDDS_ContextHandle handle);

uint32_t TinyDDS_NumberOfMipmaps(TinyDDS_ContextHandle handle);
uint32_t TinyDDS_ImageSize(TinyDDS_ContextHandle handle, uint32_t mipmaplevel);

// data return by ImageRawData is owned by the context. Don't free it!
void const *TinyDDS_ImageRawData(TinyDDS_ContextHandle handle, uint32_t mipmaplevel);

typedef void (*TinyDDS_WriteFunc)(void *user, void const *buffer, size_t byteCount);

typedef struct TinyDDS_WriteCallbacks {
	TinyDDS_ErrorFunc error;
	TinyDDS_AllocFunc alloc;
	TinyDDS_FreeFunc free;
	TinyDDS_WriteFunc write;
} TinyDDS_WriteCallbacks;

#ifndef TINYDDS_DEFINED
// early DDS was a direct copy of the Draw Draw surface bits, later on (Dx10) it moved to
// DXGI_FORMAT we use a similar thing to DXGI_FORMAT second form but will synthesis
// the old style when required when saving and vice versa when loading.
typedef enum TinyImageFormat_DXGI_FORMAT {
	TIF_DXGI_FORMAT_UNKNOWN = 0,
	TIF_DXGI_FORMAT_R32G32B32A32_TYPELESS,
	TIF_DXGI_FORMAT_R32G32B32A32_FLOAT,
	TIF_DXGI_FORMAT_R32G32B32A32_UINT,
	TIF_DXGI_FORMAT_R32G32B32A32_SINT,
	TIF_DXGI_FORMAT_R32G32B32_TYPELESS,
	TIF_DXGI_FORMAT_R32G32B32_FLOAT,
	TIF_DXGI_FORMAT_R32G32B32_UINT,
	TIF_DXGI_FORMAT_R32G32B32_SINT,
	TIF_DXGI_FORMAT_R16G16B16A16_TYPELESS,
	TIF_DXGI_FORMAT_R16G16B16A16_FLOAT,
	TIF_DXGI_FORMAT_R16G16B16A16_UNORM,
	TIF_DXGI_FORMAT_R16G16B16A16_UINT,
	TIF_DXGI_FORMAT_R16G16B16A16_SNORM,
	TIF_DXGI_FORMAT_R16G16B16A16_SINT,
	TIF_DXGI_FORMAT_R32G32_TYPELESS,
	TIF_DXGI_FORMAT_R32G32_FLOAT,
	TIF_DXGI_FORMAT_R32G32_UINT,
	TIF_DXGI_FORMAT_R32G32_SINT,
	TIF_DXGI_FORMAT_R32G8X24_TYPELESS,
	TIF_DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	TIF_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
	TIF_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
	TIF_DXGI_FORMAT_R10G10B10A2_TYPELESS,
	TIF_DXGI_FORMAT_R10G10B10A2_UNORM,
	TIF_DXGI_FORMAT_R10G10B10A2_UINT,
	TIF_DXGI_FORMAT_R11G11B10_FLOAT,
	TIF_DXGI_FORMAT_R8G8B8A8_TYPELESS,
	TIF_DXGI_FORMAT_R8G8B8A8_UNORM,
	TIF_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	TIF_DXGI_FORMAT_R8G8B8A8_UINT,
	TIF_DXGI_FORMAT_R8G8B8A8_SNORM,
	TIF_DXGI_FORMAT_R8G8B8A8_SINT,
	TIF_DXGI_FORMAT_R16G16_TYPELESS,
	TIF_DXGI_FORMAT_R16G16_FLOAT,
	TIF_DXGI_FORMAT_R16G16_UNORM,
	TIF_DXGI_FORMAT_R16G16_UINT,
	TIF_DXGI_FORMAT_R16G16_SNORM,
	TIF_DXGI_FORMAT_R16G16_SINT,
	TIF_DXGI_FORMAT_R32_TYPELESS,
	TIF_DXGI_FORMAT_D32_FLOAT,
	TIF_DXGI_FORMAT_R32_FLOAT,
	TIF_DXGI_FORMAT_R32_UINT,
	TIF_DXGI_FORMAT_R32_SINT,
	TIF_DXGI_FORMAT_R24G8_TYPELESS,
	TIF_DXGI_FORMAT_D24_UNORM_S8_UINT,
	TIF_DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	TIF_DXGI_FORMAT_X24_TYPELESS_G8_UINT,
	TIF_DXGI_FORMAT_R8G8_TYPELESS,
	TIF_DXGI_FORMAT_R8G8_UNORM,
	TIF_DXGI_FORMAT_R8G8_UINT,
	TIF_DXGI_FORMAT_R8G8_SNORM,
	TIF_DXGI_FORMAT_R8G8_SINT,
	TIF_DXGI_FORMAT_R16_TYPELESS,
	TIF_DXGI_FORMAT_R16_FLOAT,
	TIF_DXGI_FORMAT_D16_UNORM,
	TIF_DXGI_FORMAT_R16_UNORM,
	TIF_DXGI_FORMAT_R16_UINT,
	TIF_DXGI_FORMAT_R16_SNORM,
	TIF_DXGI_FORMAT_R16_SINT,
	TIF_DXGI_FORMAT_R8_TYPELESS,
	TIF_DXGI_FORMAT_R8_UNORM,
	TIF_DXGI_FORMAT_R8_UINT,
	TIF_DXGI_FORMAT_R8_SNORM,
	TIF_DXGI_FORMAT_R8_SINT,
	TIF_DXGI_FORMAT_A8_UNORM,
	TIF_DXGI_FORMAT_R1_UNORM,
	TIF_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	TIF_DXGI_FORMAT_R8G8_B8G8_UNORM,
	TIF_DXGI_FORMAT_G8R8_G8B8_UNORM,
	TIF_DXGI_FORMAT_BC1_TYPELESS,
	TIF_DXGI_FORMAT_BC1_UNORM,
	TIF_DXGI_FORMAT_BC1_UNORM_SRGB,
	TIF_DXGI_FORMAT_BC2_TYPELESS,
	TIF_DXGI_FORMAT_BC2_UNORM,
	TIF_DXGI_FORMAT_BC2_UNORM_SRGB,
	TIF_DXGI_FORMAT_BC3_TYPELESS,
	TIF_DXGI_FORMAT_BC3_UNORM,
	TIF_DXGI_FORMAT_BC3_UNORM_SRGB,
	TIF_DXGI_FORMAT_BC4_TYPELESS,
	TIF_DXGI_FORMAT_BC4_UNORM,
	TIF_DXGI_FORMAT_BC4_SNORM,
	TIF_DXGI_FORMAT_BC5_TYPELESS,
	TIF_DXGI_FORMAT_BC5_UNORM,
	TIF_DXGI_FORMAT_BC5_SNORM,
	TIF_DXGI_FORMAT_B5G6R5_UNORM,
	TIF_DXGI_FORMAT_B5G5R5A1_UNORM,
	TIF_DXGI_FORMAT_B8G8R8A8_UNORM,
	TIF_DXGI_FORMAT_B8G8R8X8_UNORM,
	TIF_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	TIF_DXGI_FORMAT_B8G8R8A8_TYPELESS,
	TIF_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
	TIF_DXGI_FORMAT_B8G8R8X8_TYPELESS,
	TIF_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
	TIF_DXGI_FORMAT_BC6H_TYPELESS,
	TIF_DXGI_FORMAT_BC6H_UF16,
	TIF_DXGI_FORMAT_BC6H_SF16,
	TIF_DXGI_FORMAT_BC7_TYPELESS,
	TIF_DXGI_FORMAT_BC7_UNORM,
	TIF_DXGI_FORMAT_BC7_UNORM_SRGB,
	TIF_DXGI_FORMAT_AYUV,
	TIF_DXGI_FORMAT_Y410,
	TIF_DXGI_FORMAT_Y416,
	TIF_DXGI_FORMAT_NV12,
	TIF_DXGI_FORMAT_P010,
	TIF_DXGI_FORMAT_P016,
	TIF_DXGI_FORMAT_420_OPAQUE,
	TIF_DXGI_FORMAT_YUY2,
	TIF_DXGI_FORMAT_Y210,
	TIF_DXGI_FORMAT_Y216,
	TIF_DXGI_FORMAT_NV11,
	TIF_DXGI_FORMAT_AI44,
	TIF_DXGI_FORMAT_IA44,
	TIF_DXGI_FORMAT_P8,
	TIF_DXGI_FORMAT_A8P8,
	TIF_DXGI_FORMAT_B4G4R4A4_UNORM,
	TIF_DXGI_FORMAT_P208,
	TIF_DXGI_FORMAT_V208,
	TIF_DXGI_FORMAT_V408,
	TIF_DXGI_FORMAT_FORCE_UINT
} TinyImageFormat_DXGI_FORMAT;

typedef enum TinyDDS_Format {
	TDDS_UNDEFINED = TIF_DXGI_FORMAT_UNKNOWN,
	TDDS_B4G4R4A4_UNORM = TIF_DXGI_FORMAT_B4G4R4A4_UNORM,
	TDDS_B5G6R5_UNORM = TIF_DXGI_FORMAT_B5G6R5_UNORM,
	TDDS_R5G5B5A1_UNORM = TIF_DXGI_FORMAT_B5G5R5A1_UNORM,
	TDDS_R8_UNORM = TIF_DXGI_FORMAT_R8_UNORM,
	TDDS_R8_SNORM = TIF_DXGI_FORMAT_R8_SNORM,
	TDDS_R8_UINT = TIF_DXGI_FORMAT_R8_UINT,
	TDDS_R8_SINT = TIF_DXGI_FORMAT_R8_SINT,
	TDDS_R8G8_UNORM = TIF_DXGI_FORMAT_R8G8_UNORM,
	TDDS_R8G8_SNORM = TIF_DXGI_FORMAT_R8G8_SNORM,
	TDDS_R8G8_UINT = TIF_DXGI_FORMAT_R8G8_UINT,
	TDDS_R8G8_SINT = TIF_DXGI_FORMAT_R8G8_SINT,
	TDDS_R8G8B8A8_UNORM = TIF_DXGI_FORMAT_R8G8B8A8_UNORM,
	TDDS_R8G8B8A8_SNORM = TIF_DXGI_FORMAT_R8G8B8A8_SNORM,
	TDDS_R8G8B8A8_UINT = TIF_DXGI_FORMAT_R8G8B8A8_UINT,
	TDDS_R8G8B8A8_SINT = TIF_DXGI_FORMAT_R8G8B8A8_SINT,
	TDDS_R8G8B8A8_SRGB = TIF_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	TDDS_B8G8R8A8_UNORM = TIF_DXGI_FORMAT_B8G8R8A8_UNORM,
	TDDS_B8G8R8A8_SRGB = TIF_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,

	TDDS_R9G9B9E5_UFLOAT = TIF_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	TDDS_R10G10B10A2_UNORM = TIF_DXGI_FORMAT_R10G10B10A2_UNORM,
	TDDS_R10G10B10A2_UINT = TIF_DXGI_FORMAT_R10G10B10A2_UINT,
	TDDS_R11G11B10_UFLOAT = TIF_DXGI_FORMAT_R11G11B10_FLOAT,

	TDDS_R16_UNORM = TIF_DXGI_FORMAT_R16_UNORM,
	TDDS_R16_SNORM = TIF_DXGI_FORMAT_R16_SNORM,
	TDDS_R16_UINT = TIF_DXGI_FORMAT_R16_UINT,
	TDDS_R16_SINT = TIF_DXGI_FORMAT_R16_SINT,
	TDDS_R16_SFLOAT = TIF_DXGI_FORMAT_R16_FLOAT,

	TDDS_R16G16_UNORM = TIF_DXGI_FORMAT_R16G16_UNORM,
	TDDS_R16G16_SNORM = TIF_DXGI_FORMAT_R16G16_SNORM,
	TDDS_R16G16_UINT = TIF_DXGI_FORMAT_R16G16_UINT,
	TDDS_R16G16_SINT = TIF_DXGI_FORMAT_R16G16_SINT,
	TDDS_R16G16_SFLOAT = TIF_DXGI_FORMAT_R16G16_FLOAT,

	TDDS_R16G16B16A16_UNORM = TIF_DXGI_FORMAT_R16G16B16A16_UNORM,
	TDDS_R16G16B16A16_SNORM = TIF_DXGI_FORMAT_R16G16B16A16_SNORM,
	TDDS_R16G16B16A16_UINT = TIF_DXGI_FORMAT_R16G16B16A16_UINT,
	TDDS_R16G16B16A16_SINT = TIF_DXGI_FORMAT_R16G16B16A16_SINT,
	TDDS_R16G16B16A16_SFLOAT = TIF_DXGI_FORMAT_R16G16B16A16_FLOAT,

	TDDS_R32_UINT = TIF_DXGI_FORMAT_R32_UINT,
	TDDS_R32_SINT = TIF_DXGI_FORMAT_R32_SINT,
	TDDS_R32_SFLOAT = TIF_DXGI_FORMAT_R32_FLOAT,

	TDDS_R32G32_UINT = TIF_DXGI_FORMAT_R32G32_UINT,
	TDDS_R32G32_SINT = TIF_DXGI_FORMAT_R32G32_SINT,
	TDDS_R32G32_SFLOAT = TIF_DXGI_FORMAT_R32G32_FLOAT,

	TDDS_R32G32B32_UINT = TIF_DXGI_FORMAT_R32G32B32_UINT,
	TDDS_R32G32B32_SINT = TIF_DXGI_FORMAT_R32G32B32_SINT,
	TDDS_R32G32B32_SFLOAT = TIF_DXGI_FORMAT_R32G32B32_FLOAT,

	TDDS_R32G32B32A32_UINT = TIF_DXGI_FORMAT_R32G32B32A32_UINT,
	TDDS_R32G32B32A32_SINT = TIF_DXGI_FORMAT_R32G32B32A32_SINT,
	TDDS_R32G32B32A32_SFLOAT = TIF_DXGI_FORMAT_R32G32B32A32_FLOAT,

	TDDS_BC1_RGBA_UNORM_BLOCK = TIF_DXGI_FORMAT_BC1_UNORM,
	TDDS_BC1_RGBA_SRGB_BLOCK = TIF_DXGI_FORMAT_BC1_UNORM_SRGB,
	TDDS_BC2_UNORM_BLOCK = TIF_DXGI_FORMAT_BC2_UNORM,
	TDDS_BC2_SRGB_BLOCK = TIF_DXGI_FORMAT_BC2_UNORM_SRGB,
	TDDS_BC3_UNORM_BLOCK = TIF_DXGI_FORMAT_BC3_UNORM,
	TDDS_BC3_SRGB_BLOCK = TIF_DXGI_FORMAT_BC3_UNORM_SRGB,
	TDDS_BC4_UNORM_BLOCK = TIF_DXGI_FORMAT_BC4_UNORM,
	TDDS_BC4_SNORM_BLOCK = TIF_DXGI_FORMAT_BC4_SNORM,
	TDDS_BC5_UNORM_BLOCK = TIF_DXGI_FORMAT_BC5_UNORM,
	TDDS_BC5_SNORM_BLOCK = TIF_DXGI_FORMAT_BC5_SNORM,

	TDDS_BC6H_UFLOAT_BLOCK = TIF_DXGI_FORMAT_BC6H_UF16,
	TDDS_BC6H_SFLOAT_BLOCK = TIF_DXGI_FORMAT_BC6H_SF16,
	TDDS_BC7_UNORM_BLOCK = TIF_DXGI_FORMAT_BC7_UNORM,
	TDDS_BC7_SRGB_BLOCK = TIF_DXGI_FORMAT_BC7_UNORM_SRGB,

	TDDS_SYNTHESISED_DXGIFORMATS = 0xFFFF,
	TDDS_R4G4_UNORM = TDDS_SYNTHESISED_DXGIFORMATS,
	TDDS_R4G4B4A4_UNORM,
	TDDS_A4R4G4B4_UNORM,
	TDDS_R5G6B5_UNORM,
	TDDS_B5G5R5A1_UNORM,
	TDDS_A1R5G5B5_UNORM,
	TDDS_R8_SRGB,
	TDDS_R8G8_SRGB,
	TDDS_R8G8B8_UNORM,
	TDDS_R8G8B8_SNORM,
	TDDS_R8G8B8_UINT,
	TDDS_R8G8B8_SINT,
	TDDS_R8G8B8_SRGB,
	TDDS_B8G8R8_UNORM,
	TDDS_B8G8R8_SNORM,
	TDDS_B8G8R8_UINT,
	TDDS_B8G8R8_SINT,
	TDDS_B8G8R8_SRGB,

	TDDS_A8B8G8R8_UNORM,
	TDDS_A8B8G8R8_SNORM,
	TDDS_A8B8G8R8_UINT,
	TDDS_A8B8G8R8_SINT,
	TDDS_A8B8G8R8_SRGB,

	TDDS_A8R8G8B8_UNORM,

	TDDS_B8G8R8A8_SNORM,
	TDDS_B8G8R8A8_UINT,
	TDDS_B8G8R8A8_SINT,

	TDDS_R16G16B16_UNORM,
	TDDS_R16G16B16_SNORM,
	TDDS_R16G16B16_UINT,
	TDDS_R16G16B16_SINT,
	TDDS_R16G16B16_SFLOAT,

	TDDS_A2B10G10R10_UNORM,
	TDDS_A2B10G10R10_UINT,
	TDDS_BC1_RGB_UNORM_BLOCK,
	TDDS_BC1_RGB_SRGB_BLOCK,

} TinyDDS_Format;
#define TINYDDS_DEFINED
#endif

TinyDDS_Format TinyDDS_GetFormat(TinyDDS_ContextHandle handle);
bool TinyDDS_WriteImage(TinyDDS_WriteCallbacks const *callbacks,
												void *user,
												uint32_t width,
												uint32_t height,
												uint32_t depth,
												uint32_t slices,
												uint32_t mipmaplevels,
												TinyDDS_Format format,
												bool cubemap,
												bool preferDx10Format,
												uint32_t const *mipmapsizes,
												void const **mipmaps);

#ifdef TINYDDS_IMPLEMENTATION

#define TINYDDS_DDSD_CAPS 0x00000001
#define TINYDDS_DDSD_HEIGHT 0x00000002
#define TINYDDS_DDSD_WIDTH 0x00000004
#define TINYDDS_DDSD_PITCH 0x00000008
#define TINYDDS_DDSD_PIXELFORMAT 0x00001000
#define TINYDDS_DDSD_MIPMAPCOUNT 0x00020000
#define TINYDDS_DDSD_LINEARSIZE 0x00080000
#define TINYDDS_DDSD_DEPTH 0x00800000
#define TINYDDS_DDSCAPS_COMPLEX 0x00000008
#define TINYDDS_DDSCAPS_TEXTURE 0x00001000
#define TINYDDS_DDSCAPS_MIPMAP 0x00400000
#define TINYDDS_DDSCAPS2_CUBEMAP 0x00000200
#define TINYDDS_DDSCAPS2_VOLUME 0x00200000
#define TINYDDS_DDSCAPS2_CUBEMAP_ALL 0x0000FC000
#define TINYDDS_D3D10_RESOURCE_MISC_TEXTURECUBE 0x4
#define TINYDDS_D3D10_RESOURCE_DIMENSION_BUFFER 1
#define TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE3D 4
#define TINYDDS_DDPF_ALPHAPIXELS                        0x00000001l
#define TINYDDS_DDPF_ALPHA                              0x00000002l
#define TINYDDS_DDPF_FOURCC                             0x00000004l
#define TINYDDS_DDPF_PALETTEINDEXED4                    0x00000008l
#define TINYDDS_DDPF_PALETTEINDEXEDTO8                  0x00000010l
#define TINYDDS_DDPF_PALETTEINDEXED8                    0x00000020l
#define TINYDDS_DDPF_RGB                                0x00000040l
#define TINYDDS_DDPF_LUMINANCE                          0x00020000l
#define TINYDDS_DDPF_BUMPLUMINANCE                      0x00040000l
#define TINYDDS_DDPF_BUMPDUDV                           0x00080000l

// some of these get stuck in unofficial DDS v9 FourCC code
typedef enum TINYDDS_D3DFORMAT {
	TINYDDS_D3DFMT_UNKNOWN              =  0,
	TINYDDS_D3DFMT_R8G8B8               = 20,
	TINYDDS_D3DFMT_A8R8G8B8             = 21,
	TINYDDS_D3DFMT_X8R8G8B8             = 22,
	TINYDDS_D3DFMT_R5G6B5               = 23,
	TINYDDS_D3DFMT_X1R5G5B5             = 24,
	TINYDDS_D3DFMT_A1R5G5B5             = 25,
	TINYDDS_D3DFMT_A4R4G4B4             = 26,
	TINYDDS_D3DFMT_R3G3B2               = 27,
	TINYDDS_D3DFMT_A8                   = 28,
	TINYDDS_D3DFMT_A8R3G3B2             = 29,
	TINYDDS_D3DFMT_X4R4G4B4             = 30,
	TINYDDS_D3DFMT_A2B10G10R10          = 31,
	TINYDDS_D3DFMT_A8B8G8R8             = 32,
	TINYDDS_D3DFMT_X8B8G8R8             = 33,
	TINYDDS_D3DFMT_G16R16               = 34,
	TINYDDS_D3DFMT_A2R10G10B10          = 35,
	TINYDDS_D3DFMT_A16B16G16R16         = 36,
	TINYDDS_D3DFMT_A8P8                 = 40,
	TINYDDS_D3DFMT_P8                   = 41,
	TINYDDS_D3DFMT_L8                   = 50,
	TINYDDS_D3DFMT_A8L8                 = 51,
	TINYDDS_D3DFMT_A4L4                 = 52,
	TINYDDS_D3DFMT_V8U8                 = 60,
	TINYDDS_D3DFMT_L6V5U5               = 61,
	TINYDDS_D3DFMT_X8L8V8U8             = 62,
	TINYDDS_D3DFMT_Q8W8V8U8             = 63,
	TINYDDS_D3DFMT_V16U16               = 64,
	TINYDDS_D3DFMT_A2W10V10U10          = 67,
	TINYDDS_D3DFMT_L16                  = 81,
	TINYDDS_D3DFMT_Q16W16V16U16         = 110,
	TINYDDS_D3DFMT_R16F                 = 111,
	TINYDDS_D3DFMT_G16R16F              = 112,
	TINYDDS_D3DFMT_A16B16G16R16F        = 113,
	TINYDDS_D3DFMT_R32F                 = 114,
	TINYDDS_D3DFMT_G32R32F              = 115,
	TINYDDS_D3DFMT_A32B32G32R32F        = 116,
	TINYDDS_D3DFMT_CxV8U8               = 117,
	TINYDDS_D3DFMT_A1                   = 118,
	TINYDDS_D3DFMT_A2B10G10R10_XR_BIAS  = 119,
} TINYDDS_D3DFORMAT;

typedef struct TinyDDS_Header {
	uint32_t magic;
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t pitchOrLinearSize;
	uint32_t depth;
	uint32_t mipMapCount;
	uint32_t reserved0[11];

	uint32_t formatSize;
	uint32_t formatFlags;
	uint32_t formatFourCC;
	uint32_t formatRGBBitCount;
	uint32_t formatRBitMask;
	uint32_t formatGBitMask;
	uint32_t formatBBitMask;
	uint32_t formatABitMask;

	uint32_t caps1;
	uint32_t caps2;
	uint32_t caps3; // not used?
	uint32_t caps4; // not used?

	uint32_t reserved1;
} TinyDDS_Header;

typedef struct TinyDDS_HeaderDX10 {
	uint32_t DXGIFormat;
	uint32_t resourceDimension;
	uint32_t miscFlag;
	uint32_t arraySize;
	uint32_t reserved;
} TinyDDS_HeaderDX10;

typedef struct TinyDDS_Context {
	TinyDDS_Callbacks callbacks;
	void *user;
	uint64_t headerPos;
	uint64_t firstImagePos;

	TinyDDS_Header header;
	TinyDDS_HeaderDX10 headerDx10;
	TinyDDS_Format format;

	bool headerValid;
	uint8_t const *mipmaps[TINYDDS_MAX_MIPMAPLEVELS];
} TinyDDS_Context;

#define TINYDDS_MAKE_RIFFCODE(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

static uint32_t TinyDDS_fileIdentifier = TINYDDS_MAKE_RIFFCODE('D', 'D', 'S', ' ');

static void TinyDDS_NullErrorFunc(void *user, char const *msg) {}

TinyDDS_ContextHandle TinyDDS_CreateContext(TinyDDS_Callbacks const *callbacks, void *user) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) callbacks->alloc(user, sizeof(TinyDDS_Context));
	if (ctx == NULL)
		return NULL;

	memset(ctx, 0, sizeof(TinyDDS_Context));
	memcpy(&ctx->callbacks, callbacks, sizeof(TinyDDS_Callbacks));
	ctx->user = user;
	if (ctx->callbacks.error == NULL) {
		ctx->callbacks.error = &TinyDDS_NullErrorFunc;
	}

	if (ctx->callbacks.read == NULL) {
		ctx->callbacks.error(user, "TinyDDS must have read callback");
		return NULL;
	}
	if (ctx->callbacks.alloc == NULL) {
		ctx->callbacks.error(user, "TinyDDS must have alloc callback");
		return NULL;
	}
	if (ctx->callbacks.free == NULL) {
		ctx->callbacks.error(user, "TinyDDS must have free callback");
		return NULL;
	}
	if (ctx->callbacks.seek == NULL) {
		ctx->callbacks.error(user, "TinyDDS must have seek callback");
		return NULL;
	}
	if (ctx->callbacks.tell == NULL) {
		ctx->callbacks.error(user, "TinyDDS must have tell callback");
		return NULL;
	}

	TinyDDS_Reset(ctx);

	return ctx;
}

void TinyDDS_DestroyContext(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return;
	TinyDDS_Reset(handle);

	ctx->callbacks.free(ctx->user, ctx);
}

void TinyDDS_Reset(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return;

	// backup user provided callbacks and data
	TinyDDS_Callbacks callbacks;
	memcpy(&callbacks, &ctx->callbacks, sizeof(TinyDDS_Callbacks));
	void *user = ctx->user;

	for (int i = 0; i < TINYDDS_MAX_MIPMAPLEVELS; ++i) {
		if (ctx->mipmaps[i] != NULL) {
			callbacks.free(user, (void *) ctx->mipmaps[i]);
		}
	}

	// reset to default state
	memset(ctx, 0, sizeof(TinyDDS_Context));
	memcpy(&ctx->callbacks, &callbacks, sizeof(TinyDDS_Callbacks));
	ctx->user = user;

}

static bool TinyDDS_IsCompressed(TinyDDS_Format fmt) {
	switch (fmt) {
	case TDDS_BC1_RGBA_UNORM_BLOCK:
	case TDDS_BC1_RGBA_SRGB_BLOCK:
	case TDDS_BC2_UNORM_BLOCK:
	case TDDS_BC2_SRGB_BLOCK:
	case TDDS_BC3_UNORM_BLOCK:
	case TDDS_BC3_SRGB_BLOCK:
	case TDDS_BC4_UNORM_BLOCK:
	case TDDS_BC4_SNORM_BLOCK:
	case TDDS_BC5_UNORM_BLOCK:
	case TDDS_BC5_SNORM_BLOCK:
	case TDDS_BC6H_UFLOAT_BLOCK:
	case TDDS_BC6H_SFLOAT_BLOCK:
	case TDDS_BC7_UNORM_BLOCK:
	case TDDS_BC7_SRGB_BLOCK:
	case TDDS_BC1_RGB_UNORM_BLOCK:
	case TDDS_BC1_RGB_SRGB_BLOCK: return true;
	default: return false;
	}
}

// the size is per pixel for uncompressed and per block of 16 pixels for compressed
static uint32_t TinyDDS_FormatSize(TinyDDS_Format fmt) {
	switch(fmt) {
	case TDDS_R8_UNORM:
	case TDDS_R8_SNORM:
	case TDDS_R8_UINT:
	case TDDS_R8_SINT:
	case TDDS_R4G4_UNORM:
	case TDDS_R8_SRGB:
		return 1;
	case TDDS_B4G4R4A4_UNORM:
	case TDDS_B5G6R5_UNORM:
	case TDDS_R5G5B5A1_UNORM:
	case TDDS_R8G8_UNORM:
	case TDDS_R8G8_SNORM:
	case TDDS_R8G8_UINT:
	case TDDS_R8G8_SINT:
	case TDDS_R16_UNORM:
	case TDDS_R16_SNORM:
	case TDDS_R16_UINT:
	case TDDS_R16_SINT:
	case TDDS_R16_SFLOAT:
	case TDDS_R4G4B4A4_UNORM:
	case TDDS_A4R4G4B4_UNORM:
	case TDDS_R5G6B5_UNORM:
	case TDDS_B5G5R5A1_UNORM:
	case TDDS_A1R5G5B5_UNORM:
	case TDDS_R8G8_SRGB:
		return 2;
	case TDDS_R8G8B8_UNORM:
	case TDDS_R8G8B8_SNORM:
	case TDDS_R8G8B8_UINT:
	case TDDS_R8G8B8_SINT:
	case TDDS_R8G8B8_SRGB:
	case TDDS_B8G8R8_UNORM:
	case TDDS_B8G8R8_SNORM:
	case TDDS_B8G8R8_UINT:
	case TDDS_B8G8R8_SINT:
	case TDDS_B8G8R8_SRGB:
		return 3;
	case TDDS_R8G8B8A8_UNORM:
	case TDDS_R8G8B8A8_SNORM:
	case TDDS_R8G8B8A8_UINT:
	case TDDS_R8G8B8A8_SINT:
	case TDDS_R8G8B8A8_SRGB:
	case TDDS_B8G8R8A8_UNORM:
	case TDDS_B8G8R8A8_SRGB:
	case TDDS_R9G9B9E5_UFLOAT:
	case TDDS_R10G10B10A2_UNORM:
	case TDDS_R10G10B10A2_UINT:
	case TDDS_R11G11B10_UFLOAT:
	case TDDS_R16G16_UNORM:
	case TDDS_R16G16_SNORM:
	case TDDS_R16G16_UINT:
	case TDDS_R16G16_SINT:
	case TDDS_R16G16_SFLOAT:
	case TDDS_R32_UINT:
	case TDDS_R32_SINT:
	case TDDS_R32_SFLOAT:
	case TDDS_A8B8G8R8_UNORM:
	case TDDS_A8B8G8R8_SNORM:
	case TDDS_A8B8G8R8_UINT:
	case TDDS_A8B8G8R8_SINT:
	case TDDS_A8B8G8R8_SRGB:
	case TDDS_B8G8R8A8_SNORM:
	case TDDS_B8G8R8A8_UINT:
	case TDDS_B8G8R8A8_SINT:
	case TDDS_A2B10G10R10_UNORM:
	case TDDS_A2B10G10R10_UINT:
	case TDDS_A8R8G8B8_UNORM:
		return 4;
	case TDDS_R16G16B16_UNORM:
	case TDDS_R16G16B16_SNORM:
	case TDDS_R16G16B16_UINT:
	case TDDS_R16G16B16_SINT:
	case TDDS_R16G16B16_SFLOAT:
		return 6;
	case TDDS_R16G16B16A16_UNORM:
	case TDDS_R16G16B16A16_SNORM:
	case TDDS_R16G16B16A16_UINT:
	case TDDS_R16G16B16A16_SINT:
	case TDDS_R16G16B16A16_SFLOAT:
	case TDDS_R32G32_UINT:
	case TDDS_R32G32_SINT:
	case TDDS_R32G32_SFLOAT:
		return 8;
	case TDDS_R32G32B32_UINT:
	case TDDS_R32G32B32_SINT:
	case TDDS_R32G32B32_SFLOAT:
		return 12;
	case TDDS_R32G32B32A32_UINT:
	case TDDS_R32G32B32A32_SINT:
	case TDDS_R32G32B32A32_SFLOAT:
		return 16;
	case TDDS_BC1_RGBA_UNORM_BLOCK:
	case TDDS_BC1_RGBA_SRGB_BLOCK:
	case TDDS_BC1_RGB_UNORM_BLOCK:
	case TDDS_BC1_RGB_SRGB_BLOCK:
		return 8;
	case TDDS_BC2_UNORM_BLOCK:
	case TDDS_BC2_SRGB_BLOCK:
	case TDDS_BC3_UNORM_BLOCK:
	case TDDS_BC3_SRGB_BLOCK:
	case TDDS_BC4_UNORM_BLOCK:
	case TDDS_BC4_SNORM_BLOCK:
	case TDDS_BC5_UNORM_BLOCK:
	case TDDS_BC5_SNORM_BLOCK:
	case TDDS_BC6H_UFLOAT_BLOCK:
	case TDDS_BC6H_SFLOAT_BLOCK:
	case TDDS_BC7_UNORM_BLOCK:
	case TDDS_BC7_SRGB_BLOCK:
		return 16;

	case TDDS_UNDEFINED:
	default: return 0;
	}
}

#define TINYDDS_CHK_DDSFORMAT(bits, rm, gm, bm, am, fmt) \
			if ((ctx->header.formatRGBBitCount == bits) && \
					(ctx->header.formatRBitMask == rm) && \
					(ctx->header.formatGBitMask == gm) && \
					(ctx->header.formatBBitMask == bm) && \
					(ctx->header.formatABitMask == am)) { return fmt; }

static TinyDDS_Format TinyDDS_DecodeFormat(TinyDDS_Context *ctx) {
	if (ctx->header.formatFlags & TINYDDS_DDPF_FOURCC) {
		if (ctx->headerDx10.DXGIFormat != TIF_DXGI_FORMAT_UNKNOWN) {
			return (TinyDDS_Format) ctx->headerDx10.DXGIFormat;
		}

		// check fourCC and some special numbers..
		// unofficially during the dx9 timeline, D3D_FORMAT were stuck directly into
		// formatFourCC field we handle FourCC and these < 119 codes here
		// its unclear if this was only for formats that couldn't be exposed via
		// Direct Draw Surfaces (like floats etc.) so I decode most of them anyway
		switch (ctx->header.formatFourCC) {
		case TINYDDS_D3DFMT_R8G8B8: return TDDS_R8G8B8_UNORM;
		case TINYDDS_D3DFMT_A8R8G8B8: return TDDS_A8R8G8B8_UNORM;
		case TINYDDS_D3DFMT_X8R8G8B8: return TDDS_A8R8G8B8_UNORM;
		case TINYDDS_D3DFMT_R5G6B5: return TDDS_R5G6B5_UNORM;
		case TINYDDS_D3DFMT_X1R5G5B5: return TDDS_A1R5G5B5_UNORM;
		case TINYDDS_D3DFMT_A1R5G5B5: return TDDS_A1R5G5B5_UNORM;
		case TINYDDS_D3DFMT_A4R4G4B4: return TDDS_A4R4G4B4_UNORM;
		case TINYDDS_D3DFMT_R3G3B2: return TDDS_UNDEFINED; // not supported
		case TINYDDS_D3DFMT_A8: return TDDS_R8_UNORM;
		case TINYDDS_D3DFMT_A8R3G3B2: return TDDS_UNDEFINED; // not supported
		case TINYDDS_D3DFMT_X4R4G4B4: return TDDS_A4R4G4B4_UNORM;
		case TINYDDS_D3DFMT_A2B10G10R10: return TDDS_A2B10G10R10_UNORM;
		case TINYDDS_D3DFMT_A8B8G8R8: return TDDS_A8B8G8R8_UNORM;
		case TINYDDS_D3DFMT_X8B8G8R8: return TDDS_A8B8G8R8_UNORM;
		case TINYDDS_D3DFMT_A2R10G10B10: return TDDS_UNDEFINED; // TODO TDDS_A2B10G10R10_UNORM;
		case TINYDDS_D3DFMT_G16R16: return TDDS_R16G16_UNORM;
		case TINYDDS_D3DFMT_A16B16G16R16: return TDDS_R16G16B16A16_UNORM;
		case TINYDDS_D3DFMT_R16F: return TDDS_R16_SFLOAT;
		case TINYDDS_D3DFMT_G16R16F: return TDDS_R16G16_SFLOAT;
		case TINYDDS_D3DFMT_A16B16G16R16F: return TDDS_R16G16B16A16_SFLOAT;
		case TINYDDS_D3DFMT_A8P8: return TDDS_UNDEFINED; // TODO
		case TINYDDS_D3DFMT_P8: return TDDS_UNDEFINED; // TODO
		case TINYDDS_D3DFMT_L8: return TDDS_R8_UNORM;
		case TINYDDS_D3DFMT_A8L8: return TDDS_R8G8_UNORM;
		case TINYDDS_D3DFMT_A4L4: return TDDS_R4G4_UNORM;
		case TINYDDS_D3DFMT_V8U8: return TDDS_R8G8_SNORM;
		case TINYDDS_D3DFMT_L6V5U5: return TDDS_UNDEFINED; // TODO TDDS_R5G6B5_SNORM_PACK16;
		case TINYDDS_D3DFMT_X8L8V8U8: return TDDS_R8G8B8A8_SNORM;
		case TINYDDS_D3DFMT_Q8W8V8U8: return TDDS_R8G8B8A8_SNORM;
		case TINYDDS_D3DFMT_V16U16: return TDDS_R16G16_SNORM;
		case TINYDDS_D3DFMT_A2W10V10U10: return TDDS_UNDEFINED; // TODO TDDS_A2R10G10B10_SNORM_PACK32
		case TINYDDS_D3DFMT_L16: return TDDS_R16_UNORM;
		case TINYDDS_D3DFMT_Q16W16V16U16: return TDDS_R16G16B16A16_SNORM;
		case TINYDDS_D3DFMT_R32F: return TDDS_R32_SFLOAT;
		case TINYDDS_D3DFMT_G32R32F: return TDDS_R32G32_SFLOAT;
		case TINYDDS_D3DFMT_A32B32G32R32F: return TDDS_R32G32B32A32_SFLOAT;
		case TINYDDS_D3DFMT_CxV8U8: return TDDS_UNDEFINED;
		case TINYDDS_D3DFMT_A1: return TDDS_UNDEFINED;          // TODO TDDS_R1_UNORM_PACK32?
		case TINYDDS_D3DFMT_A2B10G10R10_XR_BIAS: return TDDS_UNDEFINED;

		// real 4CC no exotics yet just the block compression ones
		case TINYDDS_MAKE_RIFFCODE('D', 'X', 'T', '1'): return TDDS_BC1_RGBA_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('D', 'X', 'T', '2'): return TDDS_BC2_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('D', 'X', 'T', '3'): return TDDS_BC2_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('D', 'X', 'T', '4'): return TDDS_BC3_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('D', 'X', 'T', '5'): return TDDS_BC3_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('A', 'T', 'I', '1'): return TDDS_BC4_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('A', 'T', 'I', '2'): return TDDS_BC5_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('B', 'C', '4', 'U'): return TDDS_BC4_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('B', 'C', '4', 'S'): return TDDS_BC4_SNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('B', 'C', '5', 'U'): return TDDS_BC5_UNORM_BLOCK;
		case TINYDDS_MAKE_RIFFCODE('B', 'C', '5', 'S'): return TDDS_BC5_SNORM_BLOCK;
		}
	}

	// okay back to direct draw surface bit fields to try and work format out.
	// TODO this could be better i'm sure

	// most have RGB data and/or alpha
	if ((ctx->header.formatFlags & TINYDDS_DDPF_RGB) ||
			(ctx->header.formatFlags & TINYDDS_DDPF_ALPHA)) {

		TINYDDS_CHK_DDSFORMAT(8, 0xF0, 0x0F, 0, 0, TDDS_R4G4_UNORM);
		TINYDDS_CHK_DDSFORMAT(8, 0xFF, 0, 0, 0, TDDS_R8_UNORM);
		TINYDDS_CHK_DDSFORMAT(8, 0, 0, 0, 0xFF, TDDS_R8_UNORM); // A8 as R8

		TINYDDS_CHK_DDSFORMAT(16, 0xF000, 0x0F00, 0x00F0, 0x000F, TDDS_R4G4B4A4_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x0F00, 0x00F0, 0x000F, 0xF000, TDDS_A4R4G4B4_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x0F00, 0x00F0, 0x000F, 0x0000, TDDS_R4G4B4A4_UNORM); //X8
		TINYDDS_CHK_DDSFORMAT(16, 0x00F0, 0x0F00, 0xF000, 0x000F, TDDS_B4G4R4A4_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x00F0, 0x0F00, 0xF000, 0x0000, TDDS_R4G4B4A4_UNORM); // X8
		TINYDDS_CHK_DDSFORMAT(16, 0x7C00, 0x03E0, 0x001F, 0x8000, TDDS_A1R5G5B5_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x7C00, 0x03E0, 0x001F, 0x0000, TDDS_A1R5G5B5_UNORM); // X1
		TINYDDS_CHK_DDSFORMAT(16, 0x003E, 0x07C0, 0xF800, 0x0001, TDDS_B5G5R5A1_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0xF800, 0x07C0, 0x003E, 0x0001, TDDS_R5G5B5A1_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0xF800, 0x07E0, 0x001F, 0x0000, TDDS_R5G6B5_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x001F, 0x07E0, 0xF800, 0x0000, TDDS_B5G6R5_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0xFF00, 0x00FF, 0x0000, 0x0000, TDDS_R8G8_UNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x00FF, 0xFF00, 0x0000, 0x0000, TDDS_R8G8_UNORM); //G8R8
		TINYDDS_CHK_DDSFORMAT(16, 0xFFFF, 0x0000, 0x0000, 0x0000, TDDS_R16_UNORM);

		TINYDDS_CHK_DDSFORMAT(24, 0xFF0000, 0x00FF00, 0x0000FF, 0x0, TDDS_R8G8B8_UNORM);
		TINYDDS_CHK_DDSFORMAT(24, 0x0000FF, 0x00FF00, 0xFF0000, 0x0, TDDS_B8G8R8_UNORM);

		TINYDDS_CHK_DDSFORMAT(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, TDDS_A8B8G8R8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, TDDS_A8B8G8R8_UNORM); // X
		TINYDDS_CHK_DDSFORMAT(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, TDDS_R8G8B8A8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000, TDDS_A8B8G8R8_UNORM); // X
		TINYDDS_CHK_DDSFORMAT(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, TDDS_A8R8G8B8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, TDDS_B8G8R8A8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000, TDDS_B8G8R8A8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, TDDS_A8R8G8B8_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0xFFC00000, 0x003FF000, 0x00000FFC, 0x00000003, TDDS_R10G10B10A2_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000, TDDS_A2B10G10R10_UNORM);
//		TINYDDS_CHK_DDSFORMAT(32, 0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000, TDDS_A2R10G10B10_UNORM_PACK32);
		TINYDDS_CHK_DDSFORMAT(32, 0xFFFF0000, 0x0000FFFF, 0x00000000, 0x00000000, TDDS_R16G16_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000, TDDS_R16G16_UNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, TDDS_R32_UINT);
	}

	if ((ctx->header.formatFlags & TINYDDS_DDPF_BUMPDUDV) ||
			(ctx->header.formatFlags & TINYDDS_DDPF_BUMPLUMINANCE)) {
		TINYDDS_CHK_DDSFORMAT(16, 0xFF00, 0x00FF, 0x0000, 0x0000, TDDS_R8G8_SNORM);
		TINYDDS_CHK_DDSFORMAT(16, 0x00FF, 0xFF00, 0x0000, 0x0000, TDDS_R8G8_SNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0xFFFF0000, 0x0000FFFF, 0x0000, 0x0, TDDS_R16G16_SNORM);
		TINYDDS_CHK_DDSFORMAT(32, 0xFFFF0000, 0x0000FFFF, 0x0000, 0x0, TDDS_R16G16_SNORM); // G8R8
		TINYDDS_CHK_DDSFORMAT(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x0, TDDS_A8B8G8R8_SNORM); // G8R8
	}

	if (ctx->header.formatFlags & TINYDDS_DDPF_LUMINANCE) {
		TINYDDS_CHK_DDSFORMAT(8, 0x0F, 0xF0, 0x00, 0x00, TDDS_R4G4_UNORM); // this is A4L4 aka A4R4 we decode this as R4G4
		TINYDDS_CHK_DDSFORMAT(16, 0x00FF, 0xFF00, 0x0000, 0x0000, TDDS_R8G8_UNORM); // this is A8L8 aka A4R8 we decode this as R8G8

		if (ctx->header.formatRGBBitCount == 8) return TDDS_R8_UNORM;
		if (ctx->header.formatRGBBitCount == 16) return TDDS_R16_UNORM;
		if (ctx->header.formatRGBBitCount == 32) return TDDS_R32_UINT;

	}

	return TDDS_UNDEFINED;
}
#undef TINYDDS_CHK_DDSFORMAT

static uint32_t TinyDDS_MipMapReduce(uint32_t value, uint32_t mipmaplevel) {

	// handle 0 being passed in
	if (value <= 1)
		return 1;

	// there are better ways of doing this (log2 etc.) but this doesn't require any
	// dependecies and isn't used enough to matter imho
	for (uint32_t i = 0u; i < mipmaplevel; ++i) {
		if (value <= 1)
			return 1;
		value = value / 2;
	}
	return value;
}

bool TinyDDS_ReadHeader(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;

	ctx->headerPos = ctx->callbacks.tell(ctx->user);
	if( ctx->callbacks.read(ctx->user, &ctx->header, sizeof(TinyDDS_Header)) != sizeof(TinyDDS_Header)) {
		ctx->callbacks.error(ctx->user, "Count not read DDS header");
		return false;
	}

	// try the easy case of a modern dx10 DDS file
	if ((ctx->header.formatFlags & TINYDDS_DDPF_FOURCC) &&
			(ctx->header.formatFourCC == TINYDDS_MAKE_RIFFCODE('D', 'X', '1', '0'))) {
		ctx->callbacks.read(ctx->user, &ctx->headerDx10, sizeof(TinyDDS_HeaderDX10));

		if (ctx->headerDx10.DXGIFormat >= TDDS_SYNTHESISED_DXGIFORMATS) {
			ctx->callbacks.error(ctx->user, "DX10 Header has an invalid DXGI_FORMAT code");
			return false;
		}
	}

	ctx->format = TinyDDS_DecodeFormat(ctx);
	if (ctx->format == TDDS_UNDEFINED) {
		ctx->callbacks.error(ctx->user, "Could not decode DDS format");
		return false;
	}

	if(	(ctx->header.formatRGBBitCount != 0) &&
			((ctx->header.formatRGBBitCount/8) != TinyDDS_FormatSize(ctx->format))) {
		ctx->callbacks.error(ctx->user, "Format size mismatch");
		return false;
	}

	// we 'correct' compressed mipmap counts
	if (TinyDDS_IsCompressed(ctx->format) && ctx->header.mipMapCount > 1) {
		uint32_t w = ctx->header.width;
		uint32_t h = ctx->header.height;

		for(uint32_t i = 0; i < ctx->header.mipMapCount;++i) {
			if( w <= 4 || h <= 4) {
				ctx->header.mipMapCount = i + 1;
				break;
			}
			w = w / 2;
			h = h / 2;
		}

		// compressed images never get asked to make mip maps which is good as
		// requires decompress/compress cycle
		if(ctx->header.mipMapCount == 0) ctx->header.mipMapCount = 1;
	}

	ctx->firstImagePos = ctx->callbacks.tell(ctx->user);
	ctx->headerValid = true;
	return true;
}

bool TinyDDS_IsCubemap(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}

	return (ctx->header.caps2 & TINYDDS_DDSCAPS2_CUBEMAP);
}

bool TinyDDS_Dimensions(TinyDDS_ContextHandle handle,
												uint32_t *width,
												uint32_t *height,
												uint32_t *depth,
												uint32_t *slices) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}

	if (width)
		*width = ctx->header.width;
	if (height)
		*height = ctx->header.height;
	if (depth)
		*depth = ctx->header.depth;
	if (slices)
		*slices = ctx->headerDx10.arraySize;
	return true;
}

uint32_t TinyDDS_Width(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}
	return ctx->header.width;
}

uint32_t TinyDDS_Height(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}
	return ctx->header.height;
}

uint32_t TinyDDS_Depth(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}

	return ctx->header.depth;
}

uint32_t TinyDDS_ArraySlices(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}

	return ctx->headerDx10.arraySize;
}

bool TinyDDS_Is1D(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}
	return (ctx->header.height <= 1 && ctx->header.depth <= 1);
}
bool TinyDDS_Is2D(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}
	return (ctx->header.height > 1 && ctx->header.depth <= 1);
}
bool TinyDDS_Is3D(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}

	return (ctx->header.height > 1 && ctx->header.depth > 1);
}

bool TinyDDS_IsArray(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}

	return (ctx->headerDx10.arraySize >= 1);
}

uint32_t TinyDDS_NumberOfMipmaps(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}

	return ctx->header.mipMapCount ? ctx->header.mipMapCount : 1;
}

bool TinyDDS_NeedsGenerationOfMipmaps(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return false;
	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return false;
	}

	return ctx->header.mipMapCount == 0;
}

bool TinyDDS_NeedsEndianCorrecting(TinyDDS_ContextHandle handle) {
	// TODO should return true if this file is compiled on big endian machines
	return false;
}

uint32_t TinyDDS_ImageSize(TinyDDS_ContextHandle handle, uint32_t mipmaplevel) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return 0;

	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return 0;
	}
	uint32_t w = TinyDDS_MipMapReduce(ctx->header.width, mipmaplevel);
	uint32_t h = TinyDDS_MipMapReduce(ctx->header.height, mipmaplevel);
	uint32_t d = TinyDDS_MipMapReduce(ctx->header.depth, mipmaplevel);
	uint32_t s = ctx->headerDx10.arraySize ? ctx->headerDx10.arraySize : 1;

	if (TinyDDS_IsCompressed(ctx->format)) {
		if(w < 4 || h < 4) {
			ctx->callbacks.error(ctx->user, "Compressed formats can't be less the 4x4 pixels");
			return 0;
		}

		w = w / 4;
		h = h / 4;
	}
	uint32_t pitch = ctx->header.pitchOrLinearSize;
	uint32_t const formatSize = TinyDDS_FormatSize(ctx->format);
	if(pitch == 0) {
   		pitch = w * formatSize;
	}
	return pitch * h * d * s;
}

void const *TinyDDS_ImageRawData(TinyDDS_ContextHandle handle, uint32_t mipmaplevel) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return NULL;

	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return NULL;
	}

	if (mipmaplevel >= (ctx->header.mipMapCount ? ctx->header.mipMapCount : 1) ) {
		ctx->callbacks.error(ctx->user, "Invalid mipmap level");
		return NULL;
	}

	if (mipmaplevel >= TINYDDS_MAX_MIPMAPLEVELS) {
		ctx->callbacks.error(ctx->user, "Invalid mipmap level");
		return NULL;
	}

	if (ctx->mipmaps[mipmaplevel] != NULL)
		return ctx->mipmaps[mipmaplevel];

	uint64_t offset = 0;
	for(uint32_t i=0;i < mipmaplevel;++i) {
		offset += TinyDDS_ImageSize(handle, i);
	}

	uint32_t size = TinyDDS_ImageSize(handle, mipmaplevel);
	if (size == 0)
		return NULL;

	ctx->callbacks.seek(ctx->user, offset + ctx->firstImagePos);

	ctx->mipmaps[mipmaplevel] = (uint8_t const *) ctx->callbacks.alloc(ctx->user, size);
	if (ctx->mipmaps[mipmaplevel]) {
		ctx->callbacks.read(ctx->user, (void *) ctx->mipmaps[mipmaplevel], size);
	}

	return ctx->mipmaps[mipmaplevel];
}
TinyDDS_Format TinyDDS_GetFormat(TinyDDS_ContextHandle handle) {
	TinyDDS_Context *ctx = (TinyDDS_Context *) handle;
	if (ctx == NULL)
		return TDDS_UNDEFINED;

	if (!ctx->headerValid) {
		ctx->callbacks.error(ctx->user, "Header data hasn't been read yet or its invalid");
		return TDDS_UNDEFINED;
	}
	return ctx->format;
}

#define TDDS_EF(bits, rm, gm, bm, am, fl) \
		header->formatRGBBitCount = bits; \
		header->formatRBitMask = rm; \
		header->formatGBitMask = gm; \
		header->formatBBitMask = bm; \
		header->formatABitMask = am; \
		header->formatFlags = fl; \
		header->formatFourCC = 0; \
		return true;

#define TDDS_EF_RGB(bits, rm, gm, bm) TDDS_EF(bits, rm, gm, bm, 0, TINYDDS_DDPF_RGB )
#define TDDS_EF_RGBA(bits, rm, gm, bm, am) TDDS_EF(bits, rm, gm, bm, am, TINYDDS_DDPF_RGB | TINYDDS_DDPF_ALPHAPIXELS)

static bool TinyDDS_EncodeFormat(TinyDDS_Format fmt, TinyDDS_Header* header, TinyDDS_HeaderDX10* headerDx10) {
	// lets start with the easy part. if its real DXGI_FORMAT we can just fill in the Dx10 part
	if(fmt < TDDS_SYNTHESISED_DXGIFORMATS) {
		headerDx10->DXGIFormat = (TinyImageFormat_DXGI_FORMAT)fmt;
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','1','0');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
	} else {
		headerDx10->DXGIFormat = TIF_DXGI_FORMAT_UNKNOWN;
	}
	// now lets try synthesising if possible
	// if we can reset the DX10 fourCC but leave the format in place
	// that way if we have slices which can only be DXGI_FORMAT we can use it
	switch(fmt) {
	case TDDS_R4G4_UNORM: TDDS_EF_RGB(8, 0xF0, 0x0F, 0)
	case TDDS_R8_UNORM:
		//		TDDS_EF_RGB(8, 0xFF, 0, 0 );
		// save as A8 as they seems more compatible
		header->formatRGBBitCount = 8;
		header->formatRBitMask = 0;
		header->formatGBitMask = 0;
		header->formatBBitMask = 0;
		header->formatABitMask = 0xFF;
		header->formatFlags = TINYDDS_DDPF_ALPHA;
		header->formatFourCC = 0;
		return true;
	case TDDS_R4G4B4A4_UNORM:
		TDDS_EF_RGBA(16, 0xF000, 0x0F00, 0x00F0, 0x000F);
	case TDDS_A4R4G4B4_UNORM:
		TDDS_EF_RGBA(16, 0x0F00, 0x00F0, 0x000F, 0xF000);
	case TDDS_B4G4R4A4_UNORM:
		TDDS_EF_RGBA(16, 0x00F0, 0x0F00, 0xF000, 0x000F);
	case TDDS_A1R5G5B5_UNORM:
		TDDS_EF_RGBA(16, 0x7C00, 0x03E0, 0x001F, 0x8000);
	case TDDS_B5G5R5A1_UNORM:
		TDDS_EF_RGBA(16, 0x003E, 0x07C0, 0xF800, 0x0001);
	case TDDS_R5G5B5A1_UNORM:
		TDDS_EF_RGBA(16, 0xF800, 0x07C0, 0x003E, 0x0001);
	case TDDS_R5G6B5_UNORM:
		TDDS_EF_RGB(16, 0xF800, 0x07E0, 0x001F);
	case TDDS_B5G6R5_UNORM:
		TDDS_EF_RGB(16, 0x001F, 0x07E0, 0xF800);
	case TDDS_R8G8_UNORM:
		TDDS_EF_RGB(16, 0x00FF, 0xFF00, 0);

	case TDDS_R8G8B8_UNORM:
		TDDS_EF_RGB( 24,0x00FF0000, 0x0000FF00, 0x000000FF)
	case TDDS_B8G8R8_UNORM:
		TDDS_EF_RGB( 24,0x000000FF, 0x0000FF00, 0x00FF0000)

	case TDDS_R8G8B8A8_UNORM:
		TDDS_EF_RGBA( 32,0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF)
	case TDDS_B8G8R8A8_UNORM:
		TDDS_EF_RGBA( 32,0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF)
	case TDDS_A8B8G8R8_UNORM:
		TDDS_EF_RGBA( 32,0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000)
	case TDDS_A8R8G8B8_UNORM:
		TDDS_EF_RGBA( 32,0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)
	case TDDS_R10G10B10A2_UNORM:
		TDDS_EF_RGBA( 32,0xFFC00000, 0x003FF000, 0x00000FFC, 0x00000003)
	case TDDS_A2B10G10R10_UNORM:
		TDDS_EF_RGBA( 32,0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000)
	case TDDS_BC1_RGBA_UNORM_BLOCK:
	case TDDS_BC1_RGB_UNORM_BLOCK:
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','T','1');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
		return true;
	case TDDS_BC2_UNORM_BLOCK:
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','T','3');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
		return true;
	case TDDS_BC3_UNORM_BLOCK:
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','T','5');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
		return true;
	case TDDS_BC4_UNORM_BLOCK:
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('A','T','I','1');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
		return true;
	case TDDS_BC5_UNORM_BLOCK:
		header->formatFourCC = TINYDDS_MAKE_RIFFCODE('A','T','I','2');
		header->formatFlags = TINYDDS_DDPF_FOURCC;
		return true;

	default:
		if(headerDx10->DXGIFormat == TIF_DXGI_FORMAT_UNKNOWN) return false;
		else return true;
	}
}

#undef TDDS_EF
#undef TDDS_EF_RGB
#undef TDDS_EF_RGBA

bool TinyDDS_WriteImage(TinyDDS_WriteCallbacks const *callbacks,
													void *user,
													uint32_t width,
													uint32_t height,
													uint32_t depth,
													uint32_t slices,
													uint32_t mipmaplevels,
													TinyDDS_Format format,
													bool cubemap,
													bool preferDx10Format,
													uint32_t const *mipmapsizes,
													void const **mipmaps) {
	TinyDDS_Header header;
	TinyDDS_HeaderDX10 headerDX10;
	memset(&header, 0, sizeof(header));
	memset(&headerDX10, 0, sizeof(headerDX10));

	header.magic = TINYDDS_MAKE_RIFFCODE('D', 'D', 'S', ' ');
	header.size = 124;
	header.formatSize = 32;

	header.width = width;
	header.height = height;
	header.depth = (depth > 1) ? depth : 0;
	header.mipMapCount = mipmaplevels;

	if(!TinyDDS_EncodeFormat(format, &header, &headerDX10)) return false;

	// do we have to force dx10 (for slices)
	if (slices > 1) {
		if(headerDX10.DXGIFormat == TIF_DXGI_FORMAT_UNKNOWN) {
			// DDS doesn't support slices for formats that are DXGI compatible
			return false;
		}
		header.formatFlags = TINYDDS_DDPF_FOURCC;
		header.formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','1','0');
		headerDX10.arraySize = slices;
	}
	header.flags = TINYDDS_DDSD_CAPS | TINYDDS_DDSD_PIXELFORMAT | TINYDDS_DDSD_MIPMAPCOUNT;
	header.caps1 = TINYDDS_DDSCAPS_TEXTURE | TINYDDS_DDSCAPS_COMPLEX | TINYDDS_DDSCAPS_MIPMAP;

	if(depth > 1) {
		headerDX10.resourceDimension = TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE3D;
		header.flags |= TINYDDS_DDSD_DEPTH;
		header.caps2 |= TINYDDS_DDSCAPS2_VOLUME;
	}
	else if(height > 1) {
		headerDX10.resourceDimension = TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE2D;
		header.flags |= TINYDDS_DDSD_HEIGHT;
	}
	else if(width > 1) {
		headerDX10.resourceDimension = TINYDDS_D3D10_RESOURCE_DIMENSION_TEXTURE1D;
		header.flags |= TINYDDS_DDSD_WIDTH;
	}
	if(cubemap) {
		headerDX10.miscFlag |= TINYDDS_D3D10_RESOURCE_MISC_TEXTURECUBE;
		header.caps2 |= TINYDDS_DDSCAPS2_CUBEMAP | TINYDDS_DDSCAPS2_CUBEMAP_ALL;
	}

	// unclear whether we need to save this or exactly what it should be...
	header.pitchOrLinearSize = 0;
	if(preferDx10Format && headerDX10.DXGIFormat != TIF_DXGI_FORMAT_UNKNOWN) {
		header.formatFlags = TINYDDS_DDPF_FOURCC;
		header.formatFourCC = TINYDDS_MAKE_RIFFCODE('D','X','1','0');
	}

	// now write
	callbacks->write(user, &header, sizeof(TinyDDS_Header));
	if(header.formatFlags & TINYDDS_DDPF_FOURCC &&
		header.formatFourCC == TINYDDS_MAKE_RIFFCODE('D','X','1','0')) {
		callbacks->write(user, &headerDX10, sizeof(TinyDDS_HeaderDX10));
	}

	for (uint32_t mipMapLevel = 0; mipMapLevel < header.mipMapCount; mipMapLevel++) {
		callbacks->write(user, mipmaps[mipMapLevel], mipmapsizes[mipMapLevel]);
	}
	return true;
}

#endif

#ifdef __cplusplus
};
#endif

#endif // end header
/*
MIT License

Copyright (c) 2019 DeanoC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/