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

	if (ImageFormat_BitWidth(image->format) <= 32) {
		if (ImageFormat_IsHomogenous(image->format)) {
			switch (ImageFormat_ChannelBitWidth(image->format, 0)) {
			case 4:
				// only 4 channel 32 bit formats are 2x4 and 4x4 channels
				if (nChannels == 2) {
					header.mPixelFormat.mDWRGBBitCount = 8;
					header.mPixelFormat.mDWRGBAlphaBitMask = 0xF0;
					header.mPixelFormat.mDWRBitMask = 0x0F;
				} else {
					header.mPixelFormat.mDWRGBBitCount = 16;
					header.mPixelFormat.mDWRGBAlphaBitMask = 0xF000;
					header.mPixelFormat.mDWRBitMask = 0x0F00;
					header.mPixelFormat.mDWGBitMask = 0x00F0;
					header.mPixelFormat.mDWBBitMask = 0x000F;
				}
				break;
			case 8:header.mPixelFormat.mDWRGBBitCount = 8 * nChannels;
				header.mPixelFormat.mDWRGBAlphaBitMask = (nChannels == 4) ? 0xFF000000 : (nChannels == 2) ? 0xFF00 : 0;
				header.mPixelFormat.mDWRBitMask = (nChannels > 2) ? 0x00FF0000 : 0xFF;
				header.mPixelFormat.mDWGBitMask = (nChannels > 1) ? 0x0000FF00 : 0;
				header.mPixelFormat.mDWBBitMask = (nChannels > 1) ? 0x000000FF : 0;
				break;
			case 16:
				if (nChannels == 1) {
					header.mPixelFormat.mDWRGBBitCount = 16;
					header.mPixelFormat.mDWRBitMask = 0xFFFF;
				} else {
					header.mPixelFormat.mDWRGBBitCount = 32;
					header.mPixelFormat.mDWRBitMask = 0xFFFF0000;
					header.mPixelFormat.mDWRGBAlphaBitMask = 0x0000FFFF;
				}
				break;
			case 32:header.mPixelFormat.mDWRGBBitCount = 32;
				header.mPixelFormat.mDWRBitMask = 0xFFFFFFFF;
				break;
			}
		} else {
			switch (image->format) {
			case ImageFormat_R5G6B5_UNORM_PACK16:
			case ImageFormat_B5G6R5_UNORM_PACK16:header.mPixelFormat.mDWRGBBitCount = 16;
				header.mPixelFormat.mDWRGBAlphaBitMask = 0;
				header.mPixelFormat.mDWBBitMask = 0xF800;
				header.mPixelFormat.mDWGBitMask = 0x07E0;
				header.mPixelFormat.mDWRBitMask = 0x001F;
				break;
			case ImageFormat_R5G5B5A1_UNORM_PACK16:
			case ImageFormat_B5G5R5A1_UNORM_PACK16:
			case ImageFormat_A1R5G5B5_UNORM_PACK16:header.mPixelFormat.mDWRGBBitCount = 16;
				header.mPixelFormat.mDWRGBAlphaBitMask = 0x0001;
				header.mPixelFormat.mDWRBitMask = 0xF800;
				header.mPixelFormat.mDWGBitMask = 0x07C0;
				header.mPixelFormat.mDWBBitMask = 0x003E;
				break;

			case ImageFormat_A2R10G10B10_UNORM_PACK32:
			case ImageFormat_A2R10G10B10_USCALED_PACK32:
			case ImageFormat_A2R10G10B10_UINT_PACK32:
			case ImageFormat_A2B10G10R10_UNORM_PACK32:
			case ImageFormat_A2B10G10R10_USCALED_PACK32:
			case ImageFormat_A2B10G10R10_UINT_PACK32:header.mPixelFormat.mDWRGBBitCount = 32;
				header.mPixelFormat.mDWRGBAlphaBitMask = 0xC0000000;
				header.mPixelFormat.mDWRBitMask = 0x3FF00000;
				header.mPixelFormat.mDWGBitMask = 0x000FFC00;
				header.mPixelFormat.mDWBBitMask = 0x000003FF;
				break;
			default:return false;
			}
		}
		header.mPixelFormat.mDWFlags = ((nChannels < 3) ? 0x00020000 : DDPF_RGB) | ((nChannels & 1) ? 0 : DDPF_ALPHAPIXELS);
	} else {
		header.mPixelFormat.mDWFlags = DDPF_FOURCC;

		switch (image->format) {
		case ImageFormat_R16G16_UNORM: header.mPixelFormat.mDWFourCC = 34;
			break;
		case ImageFormat_R16G16B16A16_UNORM: header.mPixelFormat.mDWFourCC = 36;
			break;
		case ImageFormat_R16_SFLOAT: header.mPixelFormat.mDWFourCC = 111;
			break;
		case ImageFormat_R16G16_SFLOAT: header.mPixelFormat.mDWFourCC = 112;
			break;
		case ImageFormat_R16G16B16A16_SFLOAT: header.mPixelFormat.mDWFourCC = 113;
			break;
		case ImageFormat_R32_SFLOAT: header.mPixelFormat.mDWFourCC = 114;
			break;
		case ImageFormat_R32G32_SFLOAT: header.mPixelFormat.mDWFourCC = 115;
			break;
		case ImageFormat_R32G32B32A32_SFLOAT: header.mPixelFormat.mDWFourCC = 116;
			break;
		case ImageFormat_BC1_RGBA_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '1');
			break;
		case ImageFormat_BC2_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '3');
			break;
		case ImageFormat_BC3_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '5');
			break;
		case ImageFormat_BC4_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '1');
			break;
		case ImageFormat_BC5_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '2');
			break;
		default:header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', '1', '0');
			headerDX10.mArraySize = 1;
			headerDX10.mDXGIFormat = Image_IsCubemap(image) ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
			if (Image_Is1D(image)) {
				headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
			} else if (Image_Is2D(image)) {
				headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
			} else if (Image_Is3D(image)) {
				headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
			}

			switch (image->format) {
			case ImageFormat_R32G32B32A32_SFLOAT: headerDX10.mDXGIFormat = 6;
				break;
			case ImageFormat_E5B9G9R9_UFLOAT_PACK32: headerDX10.mDXGIFormat = 67;
				break;
			case ImageFormat_B10G11R11_UFLOAT_PACK32: headerDX10.mDXGIFormat = 26;
				break;
			default: return false;
			}
		}
	}
	// header.

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

	if (headerDX10.mDXGIFormat) {
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
	if(image->nextType != Image_NextType::Image_IT_MipMaps &&
			image->nextType != Image_NextType::Image_IT_None	) {
		return false;
	}
	uint32_t numMipmaps = (image->nextType == Image_NextType::Image_IT_None) ? 1 : (uint32_t)Image_LinkedImageCountOf(image);

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