#include "al2o3_platform/platform.h"
#include "al2o3_stb/stb_image_write.h"
#include "al2o3_vfile/vfile.hpp"
#include "tiny_imageformat/tinyimageformat_base.h"
#include "gfx_image/image.h"

#include "tiny_ktx/tinyktx.h"
#include "tiny_dds/tinydds.h"

// TODO EXR
//#include "syoyo/tiny_exr.hpp"
namespace Image {

} // end namespace Image


static void stbIoCallbackWrite(void *user, void *data, int size) {
	VFile_Handle handle = (VFile_Handle) user;
	VFile_Write(handle, data, size);
}

AL2O3_EXTERN_C bool Image_SaveAsTGA(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default: {
		// uncompress/convert and try again
		//return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SaveAsBMP(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default: {
		return false;
	}
	}
}

AL2O3_EXTERN_C bool Image_SaveAsPNG(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src, 0);
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src, 0);
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src, 0);
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src, 0);
	default: return false;
	}
}

AL2O3_EXTERN_C bool Image_SaveAsJPG(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	void *src = Image_RawDataPtr(image);

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src, 0);
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src, 0);
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src, 0);
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src, 0);
	default:
		return false;
	}
}

AL2O3_EXTERN_C bool Image_SaveAsHDR(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;

	if (!handle) {
		return false;
	}
	float const *src = (float const *) Image_RawDataPtr(image);

	switch (image->format) {
	case TinyImageFormat_R32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 1, src);
	case TinyImageFormat_R32G32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 2, src);
	case TinyImageFormat_R32G32B32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 3, src);
	case TinyImageFormat_R32G32B32A32_SFLOAT:
		return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
																			 image->width, image->height, 4, src);
	default:
		return false;
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

AL2O3_EXTERN_C bool Image_SaveAsDDS(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;
	TinyDDS_WriteCallbacks callback{
			&tinyktxCallbackError,
			&tinyktxCallbackAlloc,
			&tinyktxCallbackFree,
			&tinyktxCallbackWrite,
	};

	TinyDDS_Format fmt = TinyImageFormat_ToTinyDDSFormat(image->format);
	if(fmt == TDDS_UNDEFINED) return false;

	// mipmaps or no linked only
	if(image->nextType != Image_NextType::Image_NT_MipMap &&
			image->nextType != Image_NextType::Image_NT_None	) {
		return false;
	}
	uint32_t numMipmaps = (uint32_t)Image_MipMapCountOf(image);

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

AL2O3_EXTERN_C bool Image_SaveAsKTX(Image_ImageHeader *image, VFile_Handle handle) {
	using namespace Image;
	TinyKtx_WriteCallbacks callback{
		&tinyktxCallbackError,
		&tinyktxCallbackAlloc,
		&tinyktxCallbackFree,
		&tinyktxCallbackWrite,
	};

	TinyKtx_Format fmt = TinyImageFormat_ToTinyKtxFormat(image->format);
	if(fmt == TKTX_UNDEFINED) return false;

	// mipmaps or no linked only
	if(image->nextType != Image_NextType::Image_NT_MipMap &&
			image->nextType != Image_NextType::Image_NT_None	) {
		return false;
	}
	uint32_t numMipmaps = (uint32_t)Image_MipMapCountOf(image);

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


AL2O3_EXTERN_C bool Image_CanSaveAsTGA(Image_ImageHeader const *image) {
	if(!Image_Is1D(image) || !Image_Is2D(image)) return false;

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return true;
	default: return false;
	}
}

AL2O3_EXTERN_C bool Image_CanSaveAsBMP(Image_ImageHeader const *image) {
	if(!Image_Is1D(image) || !Image_Is2D(image)) return false;

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return true;
	default: return false;
	}
}
AL2O3_EXTERN_C bool Image_CanSaveAsPNG(Image_ImageHeader const *image) {
	if(!Image_Is1D(image) || !Image_Is2D(image)) return false;

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return true;
	default: return false;
	}

}
AL2O3_EXTERN_C bool Image_CanSaveAsJPG(Image_ImageHeader const *image) {
	if(!Image_Is1D(image) || !Image_Is2D(image)) return false;

	switch (image->format) {
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_SRGB:
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_SRGB:
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_R8G8B8_SRGB:
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return true;
	default:
		return false;
	}

}

AL2O3_EXTERN_C bool Image_CanSaveAsHDR(Image_ImageHeader const *image) {
	if(!Image_Is1D(image) || !Image_Is2D(image)) return false;

	switch (image->format) {
	case TinyImageFormat_R32_SFLOAT:
	case TinyImageFormat_R32G32_SFLOAT:
	case TinyImageFormat_R32G32B32_SFLOAT:
	case TinyImageFormat_R32G32B32A32_SFLOAT:
		return true;
	default:
		return false;
	}

}

AL2O3_EXTERN_C bool Image_CanSaveAsDDS(Image_ImageHeader const *image) {
	if(Image_Is3D(image) && Image_IsArray(image)) return false;

	TinyDDS_Format fmt = TinyImageFormat_ToTinyDDSFormat(image->format);
	return !(fmt == TDDS_UNDEFINED);
}

AL2O3_EXTERN_C bool Image_CanSaveAsKTX(Image_ImageHeader const *image) {
	if(Image_Is3D(image) && Image_IsArray(image)) return false;

	TinyKtx_Format fmt = TinyImageFormat_ToTinyKtxFormat(image->format);
	return !(fmt == TKTX_UNDEFINED);

}
