#include "al2o3_platform/platform.h"
#include "al2o3_cmath/scalar.h"
#include "al2o3_stb/stb_image_write.h"
#include "al2o3_vfile/vfile.hpp"
#include "gfx_imageformat/format.h"
#include "gfx_imageformat/formatcracker.h"
#include "gfx_image/image.h"
#include "gfx_image/utils.h"
#include "tiny_ktx/tinyktx.h"
#include "dds.hpp"
#include <float.h>

// TODO EXR
//#include "syoyo/tiny_exr.hpp"
namespace Image {
static DDS_DXGI_FORMAT ImageFormatToDDSDXGIFormat(ImageFormat fmt) {
	switch (fmt) {
	case ImageFormat_R5G6B5_UNORM_PACK16: 	return DDS_DXGI_FORMAT_B5G6R5_UNORM;
	case ImageFormat_B5G6R5_UNORM_PACK16: 	return DDS_DXGI_FORMAT_B5G6R5_UNORM;
	case ImageFormat_B5G5R5A1_UNORM_PACK16: return DDS_DXGI_FORMAT_B5G5R5A1_UNORM;

	case ImageFormat_R8_UNORM: 				return DDS_DXGI_FORMAT_R8_UNORM;
	case ImageFormat_R8_SNORM: 				return DDS_DXGI_FORMAT_R8_SNORM;
	case ImageFormat_R8_UINT: 				return DDS_DXGI_FORMAT_R8_UINT;
	case ImageFormat_R8_SINT: 				return DDS_DXGI_FORMAT_R8_SINT;

	case ImageFormat_R8G8_UNORM: 			return DDS_DXGI_FORMAT_R8G8_UNORM;
	case ImageFormat_R8G8_SNORM: 			return DDS_DXGI_FORMAT_R8G8_SNORM;
	case ImageFormat_R8G8_UINT: 			return DDS_DXGI_FORMAT_R8G8_UINT;
	case ImageFormat_R8G8_SINT: 			return DDS_DXGI_FORMAT_R8G8_SINT;

	case ImageFormat_R8G8B8A8_UNORM: 	return DDS_DXGI_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat_R8G8B8A8_SNORM: 	return DDS_DXGI_FORMAT_R8G8B8A8_SNORM;
	case ImageFormat_R8G8B8A8_UINT: 	return DDS_DXGI_FORMAT_R8G8B8A8_UINT;
	case ImageFormat_R8G8B8A8_SINT: 	return DDS_DXGI_FORMAT_R8G8B8A8_SINT;
	case ImageFormat_R8G8B8A8_SRGB: 	return DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case ImageFormat_B8G8R8A8_UNORM: 	return DDS_DXGI_FORMAT_B8G8R8A8_UNORM;
	case ImageFormat_B8G8R8A8_SRGB:		return DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

	case ImageFormat_R16_UNORM:				return DDS_DXGI_FORMAT_R16_UNORM;
	case ImageFormat_R16_SNORM:				return DDS_DXGI_FORMAT_R16_SNORM;
	case ImageFormat_R16_UINT:				return DDS_DXGI_FORMAT_R16_UINT;
	case ImageFormat_R16_SINT:				return DDS_DXGI_FORMAT_R16_SINT;
	case ImageFormat_R16_SFLOAT:			return DDS_DXGI_FORMAT_R16_FLOAT;

	case ImageFormat_R16G16_UNORM:		return DDS_DXGI_FORMAT_R16G16_UNORM;
	case ImageFormat_R16G16_SNORM:		return DDS_DXGI_FORMAT_R16G16_SNORM;
	case ImageFormat_R16G16_UINT:			return DDS_DXGI_FORMAT_R16G16_UINT;
	case ImageFormat_R16G16_SINT:			return DDS_DXGI_FORMAT_R16G16_SINT;
	case ImageFormat_R16G16_SFLOAT:		return DDS_DXGI_FORMAT_R16G16_FLOAT;

	case ImageFormat_R16G16B16A16_UNORM:	return DDS_DXGI_FORMAT_R16G16B16A16_UNORM;
	case ImageFormat_R16G16B16A16_SNORM:	return DDS_DXGI_FORMAT_R16G16B16A16_SNORM;
	case ImageFormat_R16G16B16A16_UINT:		return DDS_DXGI_FORMAT_R16G16B16A16_UINT;
	case ImageFormat_R16G16B16A16_SINT:		return DDS_DXGI_FORMAT_R16G16B16A16_SINT;
	case ImageFormat_R16G16B16A16_SFLOAT:	return DDS_DXGI_FORMAT_R16G16B16A16_FLOAT;

	case ImageFormat_R32_UINT:				return DDS_DXGI_FORMAT_R32_UINT;
	case ImageFormat_R32_SINT:				return DDS_DXGI_FORMAT_R32_SINT;
	case ImageFormat_R32_SFLOAT:			return DDS_DXGI_FORMAT_R32_FLOAT;

	case ImageFormat_R32G32_UINT:			return DDS_DXGI_FORMAT_R32G32_UINT;
	case ImageFormat_R32G32_SINT:			return DDS_DXGI_FORMAT_R32G32_SINT;
	case ImageFormat_R32G32_SFLOAT:		return DDS_DXGI_FORMAT_R32G32_FLOAT;

	case ImageFormat_R32G32B32_UINT:	return DDS_DXGI_FORMAT_R32G32B32_UINT;
	case ImageFormat_R32G32B32_SINT:	return DDS_DXGI_FORMAT_R32G32B32_SINT;
	case ImageFormat_R32G32B32_SFLOAT:return DDS_DXGI_FORMAT_R32G32B32_FLOAT;

	case ImageFormat_R32G32B32A32_UINT:		return DDS_DXGI_FORMAT_R32G32B32A32_UINT;
	case ImageFormat_R32G32B32A32_SINT:		return DDS_DXGI_FORMAT_R32G32B32A32_SINT;
	case ImageFormat_R32G32B32A32_SFLOAT:	return DDS_DXGI_FORMAT_R32G32B32A32_FLOAT;

	case ImageFormat_E5B9G9R9_UFLOAT_PACK32: return DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP; // ??

	case ImageFormat_D16_UNORM:				return DDS_DXGI_FORMAT_R16_UNORM;
	case ImageFormat_X8_D24_UNORM_PACK32:	return DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // ??
	case ImageFormat_D32_SFLOAT:			return DDS_DXGI_FORMAT_R32_FLOAT;
	case ImageFormat_S8_UINT:					return DDS_DXGI_FORMAT_R8_UINT;
	case ImageFormat_D32_SFLOAT_S8_UINT: return DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT; // ??
	case ImageFormat_BC1_RGB_UNORM_BLOCK: 	return DDS_DXGI_FORMAT_BC1_UNORM;
	case ImageFormat_BC1_RGB_SRGB_BLOCK:		return DDS_DXGI_FORMAT_BC1_UNORM_SRGB;
	case ImageFormat_BC1_RGBA_UNORM_BLOCK:	return DDS_DXGI_FORMAT_BC1_UNORM;
	case ImageFormat_BC1_RGBA_SRGB_BLOCK:		return DDS_DXGI_FORMAT_BC1_UNORM_SRGB;
	case ImageFormat_BC2_UNORM_BLOCK:				return DDS_DXGI_FORMAT_BC2_UNORM;
	case ImageFormat_BC2_SRGB_BLOCK:				return DDS_DXGI_FORMAT_BC2_UNORM_SRGB;
	case ImageFormat_BC3_UNORM_BLOCK:				return DDS_DXGI_FORMAT_BC3_UNORM;
	case ImageFormat_BC3_SRGB_BLOCK:				return DDS_DXGI_FORMAT_BC3_UNORM_SRGB;
	case ImageFormat_BC4_UNORM_BLOCK:				return DDS_DXGI_FORMAT_BC4_UNORM;
	case ImageFormat_BC4_SNORM_BLOCK:				return DDS_DXGI_FORMAT_BC4_SNORM;
	case ImageFormat_BC5_UNORM_BLOCK:				return DDS_DXGI_FORMAT_BC5_UNORM;
	case ImageFormat_BC5_SNORM_BLOCK:				return DDS_DXGI_FORMAT_BC5_SNORM;
	case ImageFormat_BC6H_UFLOAT_BLOCK:			return DDS_DXGI_FORMAT_BC6H_UF16;
	case ImageFormat_BC6H_SFLOAT_BLOCK:			return DDS_DXGI_FORMAT_BC6H_SF16;
	case ImageFormat_BC7_UNORM_BLOCK:				return DDS_DXGI_FORMAT_BC7_UNORM;
	case ImageFormat_BC7_SRGB_BLOCK:				return DDS_DXGI_FORMAT_BC7_UNORM_SRGB;

	// unsupported
	// TODO Some of these could be supported by the older non DX10 format..
	case ImageFormat_PVR_2BPP_BLOCK:
	case ImageFormat_PVR_2BPPA_BLOCK:
	case ImageFormat_PVR_4BPP_BLOCK:
	case ImageFormat_PVR_4BPPA_BLOCK:
	case ImageFormat_PVR_2BPP_SRGB_BLOCK:
	case ImageFormat_PVR_2BPPA_SRGB_BLOCK:
	case ImageFormat_PVR_4BPP_SRGB_BLOCK:
	case ImageFormat_PVR_4BPPA_SRGB_BLOCK:

	case ImageFormat_R4G4_UNORM_PACK8:
	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8_SRGB:
	case ImageFormat_R4G4B4A4_UNORM_PACK16:
	case ImageFormat_B4G4R4A4_UNORM_PACK16:
	case ImageFormat_R5G5B5A1_UNORM_PACK16:
	case ImageFormat_A1R5G5B5_UNORM_PACK16:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8_SRGB:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_R8G8B8_SRGB:
	case ImageFormat_B8G8R8_USCALED:
	case ImageFormat_B8G8R8_SSCALED:
	case ImageFormat_B8G8R8_SRGB:
	case ImageFormat_R8G8B8_UNORM: 		//return DDS_DXGI_FORMAT_R8G8B8_UNORM;
	case ImageFormat_R8G8B8_SNORM: 		//return DDS_DXGI_FORMAT_R8G8B8_SNORM;
	case ImageFormat_R8G8B8_UINT: 		//return DDS_DXGI_FORMAT_R8G8B8_UINT;
	case ImageFormat_R8G8B8_SINT: 		//return DDS_DXGI_FORMAT_R8G8B8_SINT;
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_B8G8R8A8_USCALED:
	case ImageFormat_B8G8R8A8_SSCALED:
	case ImageFormat_A8B8G8R8_USCALED_PACK32:
	case ImageFormat_A8B8G8R8_SSCALED_PACK32:
	case ImageFormat_R16_USCALED:
	case ImageFormat_R16_SSCALED:
	case ImageFormat_R16G16_USCALED:
	case ImageFormat_R16G16_SSCALED:
	case ImageFormat_R16G16B16_USCALED:
	case ImageFormat_R16G16B16_SSCALED:
	case ImageFormat_R16G16B16A16_USCALED:
	case ImageFormat_R16G16B16A16_SSCALED:
	case ImageFormat_B8G8R8_UNORM:
	case ImageFormat_B8G8R8_SNORM:
	case ImageFormat_B8G8R8_UINT:
	case ImageFormat_B8G8R8_SINT:
	case ImageFormat_B8G8R8A8_SNORM:
	case ImageFormat_B8G8R8A8_UINT:
	case ImageFormat_B8G8R8A8_SINT:
	case ImageFormat_A8B8G8R8_UNORM_PACK32:
	case ImageFormat_A8B8G8R8_SNORM_PACK32:
	case ImageFormat_A8B8G8R8_UINT_PACK32:
	case ImageFormat_A8B8G8R8_SINT_PACK32:
	case ImageFormat_A8B8G8R8_SRGB_PACK32:
	case ImageFormat_A2R10G10B10_UNORM_PACK32:
	case ImageFormat_A2R10G10B10_USCALED_PACK32:
	case ImageFormat_A2R10G10B10_UINT_PACK32:
	case ImageFormat_A2B10G10R10_UNORM_PACK32:
	case ImageFormat_A2B10G10R10_USCALED_PACK32:
	case ImageFormat_A2B10G10R10_UINT_PACK32:
	case ImageFormat_R16G16B16_UNORM:	//return DDS_DXGI_FORMAT_R16G16B16_UNORM;
	case ImageFormat_R16G16B16_SNORM:	//return DDS_DXGI_FORMAT_R16G16B16_SNORM;
	case ImageFormat_R16G16B16_UINT:	//return DDS_DXGI_FORMAT_R16G16B16_UINT;
	case ImageFormat_R16G16B16_SINT:	//return DDS_DXGI_FORMAT_R16G16B16_SINT;
	case ImageFormat_R16G16B16_SFLOAT://return DDS_DXGI_FORMAT_R16G16B16_FLOAT;
	case ImageFormat_R64_UINT:
	case ImageFormat_R64_SINT:
	case ImageFormat_R64_SFLOAT:
	case ImageFormat_R64G64_UINT:
	case ImageFormat_R64G64_SINT:
	case ImageFormat_R64G64_SFLOAT:
	case ImageFormat_R64G64B64_UINT:
	case ImageFormat_R64G64B64_SINT:
	case ImageFormat_R64G64B64_SFLOAT:
	case ImageFormat_R64G64B64A64_UINT:
	case ImageFormat_R64G64B64A64_SINT:
	case ImageFormat_R64G64B64A64_SFLOAT:
	case ImageFormat_B10G11R11_UFLOAT_PACK32:
	case ImageFormat_D16_UNORM_S8_UINT:
	case ImageFormat_D24_UNORM_S8_UINT:
	case ImageFormat_ETC2_R8G8B8_UNORM_BLOCK:
	case ImageFormat_ETC2_R8G8B8A1_UNORM_BLOCK:
	case ImageFormat_ETC2_R8G8B8A8_UNORM_BLOCK:
	case ImageFormat_ETC2_R8G8B8_SRGB_BLOCK:
	case ImageFormat_ETC2_R8G8B8A1_SRGB_BLOCK:
	case ImageFormat_ETC2_R8G8B8A8_SRGB_BLOCK:
	case ImageFormat_EAC_R11_UNORM_BLOCK:
	case ImageFormat_EAC_R11G11_UNORM_BLOCK:
	case ImageFormat_EAC_R11_SNORM_BLOCK:
	case ImageFormat_EAC_R11G11_SNORM_BLOCK:
	case ImageFormat_ASTC_4x4_UNORM_BLOCK:
	case ImageFormat_ASTC_4x4_SRGB_BLOCK:
	case ImageFormat_ASTC_5x4_UNORM_BLOCK:
	case ImageFormat_ASTC_5x4_SRGB_BLOCK:
	case ImageFormat_ASTC_5x5_UNORM_BLOCK:
	case ImageFormat_ASTC_5x5_SRGB_BLOCK:
	case ImageFormat_ASTC_6x5_UNORM_BLOCK:
	case ImageFormat_ASTC_6x5_SRGB_BLOCK:
	case ImageFormat_ASTC_6x6_UNORM_BLOCK:
	case ImageFormat_ASTC_6x6_SRGB_BLOCK:
	case ImageFormat_ASTC_8x5_UNORM_BLOCK:
	case ImageFormat_ASTC_8x5_SRGB_BLOCK:
	case ImageFormat_ASTC_8x6_UNORM_BLOCK:
	case ImageFormat_ASTC_8x6_SRGB_BLOCK:
	case ImageFormat_ASTC_8x8_UNORM_BLOCK:
	case ImageFormat_ASTC_8x8_SRGB_BLOCK:
	case ImageFormat_ASTC_10x5_UNORM_BLOCK:
	case ImageFormat_ASTC_10x5_SRGB_BLOCK:
	case ImageFormat_ASTC_10x6_UNORM_BLOCK:
	case ImageFormat_ASTC_10x6_SRGB_BLOCK:
	case ImageFormat_ASTC_10x8_UNORM_BLOCK:
	case ImageFormat_ASTC_10x8_SRGB_BLOCK:
	case ImageFormat_ASTC_10x10_UNORM_BLOCK:
	case ImageFormat_ASTC_10x10_SRGB_BLOCK:
	case ImageFormat_ASTC_12x10_UNORM_BLOCK:
	case ImageFormat_ASTC_12x10_SRGB_BLOCK:
	case ImageFormat_ASTC_12x12_UNORM_BLOCK:
	case ImageFormat_ASTC_12x12_SRGB_BLOCK:

	case ImageFormat_UNDEFINED:
	default: return DDS_DXGI_FORMAT_UNKNOWN;

	}
}
} // end namespace Image

AL2O3_EXTERN_C bool Image_SaveDDS(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}

	DDSHeader header;
	DDSHeaderDX10 headerDX10;
	memset(&header, 0, sizeof(header));
	memset(&headerDX10, 0, sizeof(headerDX10));

	header.mDWMagic = MAKE_CHAR4('D', 'D', 'S', ' ');
	header.mDWSize = 124;

	header.mDWWidth = image->width;
	header.mDWHeight = image->height;
	header.mDWDepth = (image->depth > 1) ? image->depth : 0;
	header.mDWPitchOrLinearSize = 0;
	header.mDWMipMapCount = (uint32_t) Image_LinkedImageCountOf(image);
	header.mPixelFormat.mDWSize = 32;

	header.mDWFlags =
			DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT  |
					(Image_LinkedImageCountOf(image) > 1 ? DDSD_MIPMAPCOUNT : 0)
					| (image->depth > 1 ? DDSD_DEPTH : 0);

	int nChannels = ImageFormat_ChannelCount(image->format);

	headerDX10.mDXGIFormat = DDS_DXGI_FORMAT_UNKNOWN;;
	header.mPixelFormat.mDWFlags = DDPF_FOURCC;

	headerDX10.mDXGIFormat = ImageFormatToDDSDXGIFormat(image->format);
	if(headerDX10.mDXGIFormat == DDS_DXGI_FORMAT_UNKNOWN) {
		// TODO more fallbacks
		if (image->format == ImageFormat_R8G8B8_UNORM) {
			header.mPixelFormat.mDWRGBBitCount = 24;
			header.mPixelFormat.mDWRBitMask = 0x000000FF;
			header.mPixelFormat.mDWGBitMask = 0x0000FF00;
			header.mPixelFormat.mDWBBitMask = 0x00FF0000;
			header.mPixelFormat.mDWFlags = DDPF_RGB;
		} else {
			return false;
		}
	} else {
		header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', '1', '0');
		headerDX10.mArraySize = 1;
		headerDX10.mMiscFlag = Image_IsCubemap(image) ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
		if (Image_Is1D(image)) {
			headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
		} else if (Image_Is2D(image)) {
			headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
		} else if (Image_Is3D(image)) {
			headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
		} else {
			return false;
		}
	}

	header.mCaps.mDWCaps1 =
			DDSCAPS_TEXTURE | (Image_LinkedImageCountOf(image) > 1 ? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) |
					(image->depth != 1 ? DDSCAPS_COMPLEX : 0);
	header.mCaps.mDWCaps2 = (image->depth > 1) ? DDSCAPS2_VOLUME :
													Image_IsCubemap(image) ? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
	header.mCaps.mReserved[0] = 0;
	header.mCaps.mReserved[1] = 0;
	header.mDWReserved2 = 0;

	VFile::File *file = VFile::File::FromHandle(handle);
	file->Write(&header, sizeof(header));

	if (headerDX10.mDXGIFormat != DDS_DXGI_FORMAT_UNKNOWN) {
		file->Write(&headerDX10, sizeof(headerDX10) * 1);
	}

	for (uint32_t mipMapLevel = 0; mipMapLevel < header.mDWMipMapCount; mipMapLevel++) {
		Image_ImageHeader const *mipmap = Image_LinkedImageOf(image, mipMapLevel);

		size_t faceSize = Image_ByteCountOf(mipmap);
		void *src = Image_RawDataPtr(mipmap);
		file->Write(src, faceSize);
	}

	return true;
}
/*
bool convertAndSaveImage(const Image& image, bool (Image::*saverFunction)(const char *), const char *fileName) {
  bool bSaveImageSuccess = false;
  Image imgCopy(image);
  imgCopy.Uncompress();
  if (imgCopy.Convert(RGBA8)) {
    bSaveImageSuccess = (imgCopy.*saverFunction)(fileName);
  }

  imgCopy.Destroy();
  return bSaveImageSuccess;
}
*/

static void stbIoCallbackWrite(void *user, void *data, int size) {
	VFile_Handle handle = (VFile_Handle) user;
	VFile_Write(handle, data, size);
}

AL2O3_EXTERN_C bool Image_SaveTGA(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case ImageFormat_R8_UINT:
	case ImageFormat_R8_SINT:
	case ImageFormat_R8_UNORM:
	case ImageFormat_R8_SNORM:
	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case ImageFormat_R8G8_UINT:
	case ImageFormat_R8G8_SINT:
	case ImageFormat_R8G8_UNORM:
	case ImageFormat_R8G8_SNORM:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case ImageFormat_R8G8B8_UINT:
	case ImageFormat_R8G8B8_SINT:
	case ImageFormat_R8G8B8_UNORM:
	case ImageFormat_R8G8B8_SNORM:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case ImageFormat_R8G8B8A8_UINT:
	case ImageFormat_R8G8B8A8_SINT:
	case ImageFormat_R8G8B8A8_UNORM:
	case ImageFormat_R8G8B8A8_SNORM:
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SaveBMP(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case ImageFormat_R8_UINT:
	case ImageFormat_R8_SINT:
	case ImageFormat_R8_UNORM:
	case ImageFormat_R8_SNORM:
	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case ImageFormat_R8G8_UINT:
	case ImageFormat_R8G8_SINT:
	case ImageFormat_R8G8_UNORM:
	case ImageFormat_R8G8_SNORM:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case ImageFormat_R8G8B8_UINT:
	case ImageFormat_R8G8B8_SINT:
	case ImageFormat_R8G8B8_UNORM:
	case ImageFormat_R8G8B8_SNORM:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case ImageFormat_R8G8B8A8_UINT:
	case ImageFormat_R8G8B8A8_SINT:
	case ImageFormat_R8G8B8A8_UNORM:
	case ImageFormat_R8G8B8A8_SNORM:
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SavePNG(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case ImageFormat_R8_UINT:
	case ImageFormat_R8_SINT:
	case ImageFormat_R8_UNORM:
	case ImageFormat_R8_SNORM:
	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src, 0);
	case ImageFormat_R8G8_UINT:
	case ImageFormat_R8G8_SINT:
	case ImageFormat_R8G8_UNORM:
	case ImageFormat_R8G8_SNORM:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src, 0);
	case ImageFormat_R8G8B8_UINT:
	case ImageFormat_R8G8B8_SINT:
	case ImageFormat_R8G8B8_UNORM:
	case ImageFormat_R8G8B8_SNORM:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src, 0);
	case ImageFormat_R8G8B8A8_UINT:
	case ImageFormat_R8G8B8A8_SINT:
	case ImageFormat_R8G8B8A8_UNORM:
	case ImageFormat_R8G8B8A8_SNORM:
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src, 0);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SaveJPG(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case ImageFormat_R8_UINT:
	case ImageFormat_R8_SINT:
	case ImageFormat_R8_UNORM:
	case ImageFormat_R8_SNORM:
	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src, 0);
	case ImageFormat_R8G8_UINT:
	case ImageFormat_R8G8_SINT:
	case ImageFormat_R8G8_UNORM:
	case ImageFormat_R8G8_SNORM:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src, 0);
	case ImageFormat_R8G8B8_UINT:
	case ImageFormat_R8G8B8_SINT:
	case ImageFormat_R8G8B8_UNORM:
	case ImageFormat_R8G8B8_SNORM:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src, 0);
	case ImageFormat_R8G8B8A8_UINT:
	case ImageFormat_R8G8B8A8_SINT:
	case ImageFormat_R8G8B8A8_UNORM:
	case ImageFormat_R8G8B8A8_SNORM:
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src, 0);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SaveHDR(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	float const *src = (float const *) Image_RawDataPtr(image);

	switch (image->format) {
	case ImageFormat_R32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case ImageFormat_R32G32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case ImageFormat_R32G32B32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case ImageFormat_R32G32B32A32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

static TinyKtx_Format ImageFormatToTinyKtxFormat(ImageFormat format) {

	switch (format) {
	case ImageFormat_UNDEFINED: return TKTX_UNDEFINED;
	case ImageFormat_R4G4_UNORM_PACK8: return TKTX_R4G4_UNORM_PACK8;
	case ImageFormat_R4G4B4A4_UNORM_PACK16: return TKTX_R4G4B4A4_UNORM_PACK16;
	case ImageFormat_B4G4R4A4_UNORM_PACK16: return TKTX_B4G4R4A4_UNORM_PACK16;
	case ImageFormat_R5G6B5_UNORM_PACK16: return TKTX_R5G6B5_UNORM_PACK16;
	case ImageFormat_B5G6R5_UNORM_PACK16: return TKTX_B5G6R5_UNORM_PACK16;
	case ImageFormat_R5G5B5A1_UNORM_PACK16: return TKTX_R5G5B5A1_UNORM_PACK16;
	case ImageFormat_B5G5R5A1_UNORM_PACK16: return TKTX_B5G5R5A1_UNORM_PACK16;
	case ImageFormat_A1R5G5B5_UNORM_PACK16: return TKTX_A1R5G5B5_UNORM_PACK16;
	case ImageFormat_R8_UNORM: return TKTX_R8_UNORM;
	case ImageFormat_R8_SNORM: return TKTX_R8_SNORM;
	case ImageFormat_R8_UINT: return TKTX_R8_UINT;
	case ImageFormat_R8_SINT: return TKTX_R8_SINT;
	case ImageFormat_R8_SRGB: return TKTX_R8_SRGB;
	case ImageFormat_R8G8_UNORM: return TKTX_R8G8_UNORM;
	case ImageFormat_R8G8_SNORM: return TKTX_R8G8_SNORM;
	case ImageFormat_R8G8_UINT: return TKTX_R8G8_UINT;
	case ImageFormat_R8G8_SINT: return TKTX_R8G8_SINT;
	case ImageFormat_R8G8_SRGB: return TKTX_R8G8_SRGB;
	case ImageFormat_R8G8B8_UNORM: return TKTX_R8G8B8_UNORM;
	case ImageFormat_R8G8B8_SNORM: return TKTX_R8G8B8_SNORM;
	case ImageFormat_R8G8B8_UINT: return TKTX_R8G8B8_UINT;
	case ImageFormat_R8G8B8_SINT: return TKTX_R8G8B8_SINT;
	case ImageFormat_R8G8B8_SRGB: return TKTX_R8G8B8_SRGB;
	case ImageFormat_B8G8R8_UNORM: return TKTX_B8G8R8_UNORM;
	case ImageFormat_B8G8R8_SNORM: return TKTX_B8G8R8_SNORM;
	case ImageFormat_B8G8R8_UINT: return TKTX_B8G8R8_UINT;
	case ImageFormat_B8G8R8_SINT: return TKTX_B8G8R8_SINT;
	case ImageFormat_B8G8R8_SRGB: return TKTX_B8G8R8_SRGB;
	case ImageFormat_R8G8B8A8_UNORM: return TKTX_R8G8B8A8_UNORM;
	case ImageFormat_R8G8B8A8_SNORM: return TKTX_R8G8B8A8_SNORM;
	case ImageFormat_R8G8B8A8_UINT: return TKTX_R8G8B8A8_UINT;
	case ImageFormat_R8G8B8A8_SINT: return TKTX_R8G8B8A8_SINT;
	case ImageFormat_R8G8B8A8_SRGB: return TKTX_R8G8B8A8_SRGB;
	case ImageFormat_B8G8R8A8_UNORM: return TKTX_B8G8R8A8_UNORM;
	case ImageFormat_B8G8R8A8_SNORM: return TKTX_B8G8R8A8_SNORM;
	case ImageFormat_B8G8R8A8_UINT: return TKTX_B8G8R8A8_UINT;
	case ImageFormat_B8G8R8A8_SINT: return TKTX_B8G8R8A8_SINT;
	case ImageFormat_B8G8R8A8_SRGB: return TKTX_B8G8R8A8_SRGB;
	case ImageFormat_A8B8G8R8_UNORM_PACK32: return TKTX_A8B8G8R8_UNORM_PACK32;
	case ImageFormat_A8B8G8R8_SNORM_PACK32: return TKTX_A8B8G8R8_SNORM_PACK32;
	case ImageFormat_A8B8G8R8_UINT_PACK32: return TKTX_A8B8G8R8_UINT_PACK32;
	case ImageFormat_A8B8G8R8_SINT_PACK32: return TKTX_A8B8G8R8_SINT_PACK32;
	case ImageFormat_A8B8G8R8_SRGB_PACK32: return TKTX_A8B8G8R8_SRGB_PACK32;
	case ImageFormat_A2R10G10B10_UNORM_PACK32: return TKTX_A2R10G10B10_UNORM_PACK32;
	case ImageFormat_A2R10G10B10_UINT_PACK32: return TKTX_A2R10G10B10_UINT_PACK32;
	case ImageFormat_A2B10G10R10_UNORM_PACK32: return TKTX_A2B10G10R10_UNORM_PACK32;
	case ImageFormat_A2B10G10R10_UINT_PACK32: return TKTX_A2B10G10R10_UINT_PACK32;
	case ImageFormat_R16_UNORM: return TKTX_R16_UNORM;
	case ImageFormat_R16_SNORM: return TKTX_R16_SNORM;
	case ImageFormat_R16_UINT: return TKTX_R16_UINT;
	case ImageFormat_R16_SINT: return TKTX_R16_SINT;
	case ImageFormat_R16_SFLOAT: return TKTX_R16_SFLOAT;
	case ImageFormat_R16G16_UNORM: return TKTX_R16G16_UNORM;
	case ImageFormat_R16G16_SNORM: return TKTX_R16G16_SNORM;
	case ImageFormat_R16G16_UINT: return TKTX_R16G16_UINT;
	case ImageFormat_R16G16_SINT: return TKTX_R16G16_SINT;
	case ImageFormat_R16G16_SFLOAT: return TKTX_R16G16_SFLOAT;
	case ImageFormat_R16G16B16_UNORM: return TKTX_R16G16B16_UNORM;
	case ImageFormat_R16G16B16_SNORM: return TKTX_R16G16B16_SNORM;
	case ImageFormat_R16G16B16_UINT: return TKTX_R16G16B16_UINT;
	case ImageFormat_R16G16B16_SINT: return TKTX_R16G16B16_SINT;
	case ImageFormat_R16G16B16_SFLOAT: return TKTX_R16G16B16_SFLOAT;
	case ImageFormat_R16G16B16A16_UNORM: return TKTX_R16G16B16A16_UNORM;
	case ImageFormat_R16G16B16A16_SNORM: return TKTX_R16G16B16A16_SNORM;
	case ImageFormat_R16G16B16A16_UINT: return TKTX_R16G16B16A16_UINT;
	case ImageFormat_R16G16B16A16_SINT: return TKTX_R16G16B16A16_SINT;
	case ImageFormat_R16G16B16A16_SFLOAT: return TKTX_R16G16B16A16_SFLOAT;
	case ImageFormat_R32_UINT: return TKTX_R32_UINT;
	case ImageFormat_R32_SINT: return TKTX_R32_SINT;
	case ImageFormat_R32_SFLOAT: return TKTX_R32_SFLOAT;
	case ImageFormat_R32G32_UINT: return TKTX_R32G32_UINT;
	case ImageFormat_R32G32_SINT: return TKTX_R32G32_SINT;
	case ImageFormat_R32G32_SFLOAT: return TKTX_R32G32_SFLOAT;
	case ImageFormat_R32G32B32_UINT: return TKTX_R32G32B32_UINT;
	case ImageFormat_R32G32B32_SINT: return TKTX_R32G32B32_SINT;
	case ImageFormat_R32G32B32_SFLOAT: return TKTX_R32G32B32_SFLOAT;
	case ImageFormat_R32G32B32A32_UINT: return TKTX_R32G32B32A32_UINT;
	case ImageFormat_R32G32B32A32_SINT: return TKTX_R32G32B32A32_SINT;
	case ImageFormat_R32G32B32A32_SFLOAT: return TKTX_R32G32B32A32_SFLOAT;
	case ImageFormat_B10G11R11_UFLOAT_PACK32: return TKTX_B10G11R11_UFLOAT_PACK32;
	case ImageFormat_E5B9G9R9_UFLOAT_PACK32: return TKTX_E5B9G9R9_UFLOAT_PACK32;
	case ImageFormat_BC1_RGB_UNORM_BLOCK: return TKTX_BC1_RGB_UNORM_BLOCK;
	case ImageFormat_BC1_RGB_SRGB_BLOCK: return TKTX_BC1_RGB_SRGB_BLOCK;
	case ImageFormat_BC1_RGBA_UNORM_BLOCK: return TKTX_BC1_RGBA_UNORM_BLOCK;
	case ImageFormat_BC1_RGBA_SRGB_BLOCK: return TKTX_BC1_RGBA_SRGB_BLOCK;
	case ImageFormat_BC2_UNORM_BLOCK: return TKTX_BC2_UNORM_BLOCK;
	case ImageFormat_BC2_SRGB_BLOCK: return TKTX_BC2_SRGB_BLOCK;
	case ImageFormat_BC3_UNORM_BLOCK: return TKTX_BC3_UNORM_BLOCK;
	case ImageFormat_BC3_SRGB_BLOCK: return TKTX_BC3_SRGB_BLOCK;
	case ImageFormat_BC4_UNORM_BLOCK: return TKTX_BC4_UNORM_BLOCK;
	case ImageFormat_BC4_SNORM_BLOCK: return TKTX_BC4_SNORM_BLOCK;
	case ImageFormat_BC5_UNORM_BLOCK: return TKTX_BC5_UNORM_BLOCK;
	case ImageFormat_BC5_SNORM_BLOCK: return TKTX_BC5_SNORM_BLOCK;
	case ImageFormat_BC6H_UFLOAT_BLOCK: return TKTX_BC6H_UFLOAT_BLOCK;
	case ImageFormat_BC6H_SFLOAT_BLOCK: return TKTX_BC6H_SFLOAT_BLOCK;
	case ImageFormat_BC7_UNORM_BLOCK: return TKTX_BC7_UNORM_BLOCK;
	case ImageFormat_BC7_SRGB_BLOCK: return TKTX_BC7_SRGB_BLOCK;
	case ImageFormat_PVR_2BPP_BLOCK: return TKTX_PVR_2BPP_BLOCK;
	case ImageFormat_PVR_2BPPA_BLOCK: return TKTX_PVR_2BPPA_BLOCK;
	case ImageFormat_PVR_4BPP_BLOCK: return TKTX_PVR_4BPP_BLOCK;
	case ImageFormat_PVR_4BPPA_BLOCK: return TKTX_PVR_4BPPA_BLOCK;
	case ImageFormat_PVR_2BPP_SRGB_BLOCK: return TKTX_PVR_2BPP_SRGB_BLOCK;
	case ImageFormat_PVR_2BPPA_SRGB_BLOCK: return TKTX_PVR_2BPPA_SRGB_BLOCK;
	case ImageFormat_PVR_4BPP_SRGB_BLOCK: return TKTX_PVR_4BPP_SRGB_BLOCK;
	case ImageFormat_PVR_4BPPA_SRGB_BLOCK: return TKTX_PVR_4BPPA_SRGB_BLOCK;

	case ImageFormat_R8_USCALED:
	case ImageFormat_R8_SSCALED:
	case ImageFormat_R8G8_USCALED:
	case ImageFormat_R8G8_SSCALED:
	case ImageFormat_R8G8B8_USCALED:
	case ImageFormat_R8G8B8_SSCALED:
	case ImageFormat_B8G8R8_USCALED:
	case ImageFormat_B8G8R8_SSCALED:
	case ImageFormat_R8G8B8A8_USCALED:
	case ImageFormat_R8G8B8A8_SSCALED:
	case ImageFormat_B8G8R8A8_USCALED:
	case ImageFormat_B8G8R8A8_SSCALED:
	case ImageFormat_A8B8G8R8_USCALED_PACK32:
	case ImageFormat_A8B8G8R8_SSCALED_PACK32:
	case ImageFormat_A2R10G10B10_USCALED_PACK32:
	case ImageFormat_A2B10G10R10_USCALED_PACK32:
	case ImageFormat_R16_USCALED:
	case ImageFormat_R16_SSCALED:
	case ImageFormat_R16G16_USCALED:
	case ImageFormat_R16G16_SSCALED:
	case ImageFormat_R16G16B16_USCALED:
	case ImageFormat_R16G16B16_SSCALED:
	case ImageFormat_R16G16B16A16_USCALED:
	case ImageFormat_R16G16B16A16_SSCALED:
	case ImageFormat_R64_UINT:
	case ImageFormat_R64_SINT:
	case ImageFormat_R64_SFLOAT:
	case ImageFormat_R64G64_UINT:
	case ImageFormat_R64G64_SINT:
	case ImageFormat_R64G64_SFLOAT:
	case ImageFormat_R64G64B64_UINT:
	case ImageFormat_R64G64B64_SINT:
	case ImageFormat_R64G64B64_SFLOAT:
	case ImageFormat_R64G64B64A64_UINT:
	case ImageFormat_R64G64B64A64_SINT:
	case ImageFormat_R64G64B64A64_SFLOAT:
	case ImageFormat_D16_UNORM:
	case ImageFormat_X8_D24_UNORM_PACK32:
	case ImageFormat_D32_SFLOAT:
	case ImageFormat_S8_UINT:
	case ImageFormat_D16_UNORM_S8_UINT:
	case ImageFormat_D24_UNORM_S8_UINT:
	case ImageFormat_D32_SFLOAT_S8_UINT:
	default: return TKTX_UNDEFINED;
	};

}
static void tinyktxCallbackError(void *user, char const *msg) {
	LOGERRORF("Tiny_Ktx ERROR: %s", msg);
}

static void *tinyktxCallbackAlloc(void *user, size_t size) {
	return MEMORY_MALLOC(size);
}

static void tinyktxCallbackFree(void *user, void *data) {
	MEMORY_FREE(data);
}

static void tinyktxCallbackWrite(void *user, void const *data, size_t size) {
	auto handle = (VFile_Handle) user;
	VFile_Write(handle, data, size);
}

AL2O3_EXTERN_C bool Image_SaveKTX(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;
	TinyKtx_WriteCallbacks callback{
		&tinyktxCallbackError,
		&tinyktxCallbackAlloc,
		&tinyktxCallbackFree,
		&tinyktxCallbackWrite,
	};

	TinyKtx_Format fmt = ImageFormatToTinyKtxFormat(image->format);
	if(fmt == TKTX_UNDEFINED) return false;

	// mipmaps or no linked only
	if(image->nextType != Image_NextType::Image_NT_MipMaps &&
			image->nextType != Image_NextType::Image_NT_None	) {
		return false;
	}
	uint32_t numMipmaps = (image->nextType == Image_NextType::Image_NT_None) ? 1 : (uint32_t)Image_LinkedImageCountOf(image);

	uint32_t mipmapsizes[TINYKTX_MAX_MIPMAPLEVELS];
	void const* mipmaps[TINYKTX_MAX_MIPMAPLEVELS];
	memset(mipmapsizes, 0, sizeof(uint32_t)*TINYKTX_MAX_MIPMAPLEVELS);
	memset(mipmaps, 0, sizeof(void const*)*TINYKTX_MAX_MIPMAPLEVELS);

	for(size_t i = 0; i < numMipmaps; ++i) {
		mipmapsizes[i] = (uint32_t) Image_LinkedImageOf(image, i)->dataSize;
		mipmaps[i] = Image_RawDataPtr(Image_LinkedImageOf(image, i));
	}

	return TinyKtx_WriteImage(&callback,
										 handle,
										 image->width,
										 image->height,
										 image->depth,
										 image->slices,
										 numMipmaps,
										 fmt,
										 Image_IsCubemap(image),
										 mipmapsizes,
										 mipmaps );
}