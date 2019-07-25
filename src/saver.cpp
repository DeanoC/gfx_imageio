#include "al2o3_platform/platform.h"
#include "al2o3_cmath/scalar.h"
#include "al2o3_stb/stb_image_write.h"
#include "al2o3_vfile/vfile.hpp"
#include "tiny_imageformat/format.h"
#include "gfx_image/image.h"
#include "gfx_image/utils.h"

#include "tiny_imageformat/formatcracker.h"
#include "tiny_ktx/tinyktx.h"
#include "tiny_dds/tinydds.h"
#include <float.h>

// TODO EXR
//#include "syoyo/tiny_exr.hpp"
namespace Image {

} // end namespace Image


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

AL2O3_EXTERN_C bool Image_SaveDDS(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;
	TinyDDS_WriteCallbacks callback{
			&tinyktxCallbackError,
			&tinyktxCallbackAlloc,
			&tinyktxCallbackFree,
			&tinyktxCallbackWrite,
	};



	TinyDDS_Format fmt = ImageFormat_ToTinyDDSFormat(image->format);
	if(fmt == TDDS_UNDEFINED) return false;

	// mipmaps or no linked only
	if(image->nextType != Image_NextType::Image_NT_MipMaps &&
			image->nextType != Image_NextType::Image_NT_None	) {
		return false;
	}
	uint32_t numMipmaps = (image->nextType == Image_NextType::Image_NT_None) ? 1 : (uint32_t)Image_LinkedImageCountOf(image);

	uint32_t mipmapsizes[TINYDDS_MAX_MIPMAPLEVELS];
	void const* mipmaps[TINYDDS_MAX_MIPMAPLEVELS];
	memset(mipmapsizes, 0, sizeof(uint32_t)*TINYDDS_MAX_MIPMAPLEVELS);
	memset(mipmaps, 0, sizeof(void const*)*TINYDDS_MAX_MIPMAPLEVELS);

	for(size_t i = 0; i < numMipmaps; ++i) {
		mipmapsizes[i] = (uint32_t) Image_LinkedImageOf(image, i)->dataSize;
		mipmaps[i] = Image_RawDataPtr(Image_LinkedImageOf(image, i));
	}

	return TinyDDS_WriteImage(&callback,
														handle,
														image->width,
														image->height,
														image->depth,
														image->slices,
														numMipmaps,
														fmt,
														Image_IsCubemap(image),
														true,
														mipmapsizes,
														mipmaps );
}

AL2O3_EXTERN_C bool Image_SaveKTX(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;
	TinyKtx_WriteCallbacks callback{
		&tinyktxCallbackError,
		&tinyktxCallbackAlloc,
		&tinyktxCallbackFree,
		&tinyktxCallbackWrite,
	};

	TinyKtx_Format fmt = ImageFormat_ToTinyKtxFormat(image->format);
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