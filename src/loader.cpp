#include "al2o3_platform/platform.h"
#include "al2o3_cmath/scalar.h"
#include "al2o3_stb/stb_image.h"
#include "al2o3_vfile/vfile.hpp"
#include "utils_misccpp/compiletimehash.hpp"
#include "gfx_image/create.h"

#include "tiny_imageformat/tinyimageformat_base.h"
#include "gfx_image/image.h"
#include "tiny_ktx/tinyktx.h"
#include "tiny_dds/tinydds.h"
#include "al2o3_syoyo/tiny_exr.hpp"
#include "gfx_imageio/basisu.h"

#include <float.h>

namespace {
// PVR loader borrowed from https://github.com/ConfettiFX/The-Forge
// Describes the header of a PVR header-texture
typedef struct PVR_Header_Texture_TAG {
	uint32_t mVersion;
	uint32_t mFlags; //!< Various format flags.
	uint64_t mPixelFormat; //!< The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
	uint32_t mColorSpace; //!< The Color Space of the texture, currently either linear RGB or sRGB.
	uint32_t
			mChannelType; //!< Variable type that the channel is stored in. Supports signed/unsigned int/short/char/float.
	uint32_t mHeight; //!< Height of the texture.
	uint32_t mWidth; //!< Width of the texture.
	uint32_t mDepth; //!< Depth of the texture. (Z-slices)
	uint32_t mNumSurfaces; //!< Number of members in a Texture Array.
	uint32_t mNumFaces; //!< Number of faces in a Cube Map. Maybe be a value other than 6.
	uint32_t mNumMipMaps; //!< Number of MIP Maps in the texture - NB: Includes top level.
	uint32_t mMetaDataSize; //!< Size of the accompanying meta data.
} PVR_Texture_Header;

const unsigned int gPvrtexV3HeaderVersion = 0x03525650;

} // end anon namespace


AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadPVR(VFile_Handle handle) {
	// TODO: Image
	// - no support for PVRTC2 at the moment since it isn't supported on iOS devices.
	// - only new PVR header V3 is supported at the moment.  Should we add legacy for V2 and V1?
	// - metadata is ignored for now.  Might be useful to implement it if the need for metadata arises (eg. padding, atlas coordinates, orientations, border data, etc...).
	// - flags are also ignored for now.  Currently a flag of 0x02 means that the color have been pre-multiplied byt the alpha values.

	// Assumptions:
	// - it's assumed that the texture is already twiddled (ie. Morton).  This should always be the case for PVRTC V3.

	PVR_Texture_Header header;
	VFile::File *file = VFile::FromHandle(handle);
	file->Read(&header, sizeof(header));

	if (header.mVersion != gPvrtexV3HeaderVersion) {
		LOGERROR("Load PVR failed: Not a valid PVR V3 header.");
		return nullptr;
	}

	if (header.mPixelFormat > 3) {
		LOGERROR("Load PVR failed: Not a supported PVR pixel format.  Only PVRTC is supported at the moment.");
		return nullptr;
	}

	if (header.mNumSurfaces > 1 && header.mNumFaces > 1) {
		LOGERROR("Load PVR failed: Loading arrays of cubemaps isn't supported.");
		return nullptr;
	}

	uint32_t width = header.mWidth;
	uint32_t height = header.mHeight;
	uint32_t depth = header.mDepth;
	uint32_t slices = header.mNumSurfaces * header.mNumFaces;
	uint32_t mipMapCount = header.mNumMipMaps;
	TinyImageFormat format = TinyImageFormat_UNDEFINED;

	bool isSrgb = (header.mColorSpace == 1);

	switch (header.mPixelFormat) {
	case 0:
	case 1: format = isSrgb ? TinyImageFormat_PVRTC1_2BPP_SRGB : TinyImageFormat_PVRTC1_2BPP_UNORM;
		break;
	case 2:
	case 3: format = isSrgb ? TinyImageFormat_PVRTC1_4BPP_SRGB : TinyImageFormat_PVRTC1_4BPP_UNORM;
		break;
	default:    // NOT SUPPORTED
		LOGERROR("Load PVR failed: pixel type not supported. ");
		return nullptr;
	}

	// TODO Dean load mipmaps
	// TODO read pvr data so no mipmaps at all for now :(

	// skip the meta data
	file->Seek(header.mMetaDataSize, VFile_SD_Current);

	// Create and extract the pixel data
	auto image = Image_Create(width, height, depth, slices, format);

	file->Read(Image_RawDataPtr(image), Image_ByteCountOf(image));
	// TODO we should skip to the end here, but we
	// don't have pack or streams files so no harm yet

	return image;
}

static int stbIoCallbackRead(void *user, char *data, int size) {
	VFile_Handle handle = (VFile_Handle) user;
	return (int) VFile_Read(handle, data, size);
}
static void stbIoCallbackSkip(void *user, int n) {
	VFile_Handle handle = (VFile_Handle) user;
	VFile_Seek(handle, n, VFile_SD_Current);
}
static int stbIoCallbackEof(void *user) {
	VFile_Handle handle = (VFile_Handle) user;
	return VFile_IsEOF(handle);
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadLDR(VFile_Handle handle) {

	stbi_io_callbacks callbacks{
			&stbIoCallbackRead,
			&stbIoCallbackSkip,
			&stbIoCallbackEof
	};

	size_t origin = VFile_Tell(handle);

	int w = 0, h = 0, cmp = 0;
	stbi_info_from_callbacks(&callbacks, handle, &w, &h, &cmp);

	if (w <= 0 || h <= 0 || cmp <= 0 || cmp > 4) {
		return nullptr;
	}

	TinyImageFormat format = TinyImageFormat_UNDEFINED;
	uint64_t memoryRequirement = sizeof(stbi_uc) * w * h * cmp;

	switch (cmp) {
	case 1: format = TinyImageFormat_R8_UNORM;
		break;
	case 2: format = TinyImageFormat_R8G8_UNORM;
		break;
	case 3: format = TinyImageFormat_R8G8B8_UNORM;
		break;
	case 4: format = TinyImageFormat_R8G8B8A8_UNORM;
		break;
	}

	VFile_Seek(handle, origin, VFile_SD_Begin);
	stbi_uc *uncompressed = stbi_load_from_callbacks(&callbacks, handle, &w, &h, &cmp, cmp);
	if (uncompressed == nullptr) {
		return nullptr;
	}

	// Create and extract the pixel data
	auto image = Image_Create(w, h, 1, 1, format);
	ASSERT(memoryRequirement == Image_ByteCountOf(image));

	memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
	stbi_image_free(uncompressed);
	return image;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadHDR(VFile_Handle handle) {

	stbi_io_callbacks callbacks{
			&stbIoCallbackRead,
			&stbIoCallbackSkip,
			&stbIoCallbackEof
	};

	int w = 0, h = 0, cmp = 0, requiredCmp = 0;
	stbi_info_from_callbacks(&callbacks, handle, &w, &h, &cmp);

	if (w == 0 || h == 0 || cmp == 0) {
		return nullptr;
	}

	requiredCmp = cmp;

	TinyImageFormat format = TinyImageFormat_UNDEFINED;
	switch (requiredCmp) {
	case 1: format = TinyImageFormat_R32_SFLOAT;
		break;
	case 2: format = TinyImageFormat_R32G32_SFLOAT;
		break;
	case 3: format = TinyImageFormat_R32G32B32_SFLOAT;
		break;
	case 4: format = TinyImageFormat_R32G32B32A32_SFLOAT;
		break;
	}

	uint64_t memoryRequirement = sizeof(float) * w * h * requiredCmp;

	float *uncompressed = stbi_loadf_from_callbacks(&callbacks, handle, &w, &h, &cmp, requiredCmp);
	if (uncompressed == nullptr) {
		return nullptr;
	}

	// Create and extract the pixel data
	auto image = Image_Create(w, h, 1, 1, format);
	ASSERT(memoryRequirement == Image_ByteCountOf(image));

	memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
	stbi_image_free(uncompressed);
	return image;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadEXR(VFile_Handle handle) {
	VFile::File *file = VFile::FromHandle(handle);

	using namespace tinyexr;
	EXRVersion version;
	EXRHeader header;
	InitEXRHeader(&header);
	int ret = 0;
	ret = ParseEXRVersion(&version, handle);
	if (ret != 0) {
		LOGERROR("Parse EXR error");
		return nullptr;
	}

	file->Seek(0, VFile_SD_Begin);
	ret = ParseEXRHeader(&header, &version, handle);
	if (ret != 0) {
		LOGERROR("Parse EXR error");
		return nullptr;
	}

	// only support homogenous image (all channels the same format)
	int firstPixelType = 0;
	for (int i = 0; i < Math_MinI32(header.num_channels, 4); i++) {
		if (i == 0) {
			firstPixelType = header.pixel_types[0];
		} else {
			if (header.pixel_types[i] != firstPixelType) {
				LOGERROR("EXR image not homogenous");
				return nullptr;
			}
		}
	}

	EXRImage exrImage;
	InitEXRImage(&exrImage);

	file->Seek(0, VFile_SD_Begin);
	ret = LoadEXRImage(&exrImage, &header, handle);
	if (ret != 0) {
		LOGERROR("Load EXR error\n");
		return nullptr;
	}

	// RGBA
	int idxR = -1;
	int idxG = -1;
	int idxB = -1;
	int idxA = -1;
	int numChannels = 0;
	for (int c = 0; c < header.num_channels; c++) {
		if (strcmp(header.channels[c].name, "R") == 0) {
			idxR = c;
			numChannels++;
		} else if (strcmp(header.channels[c].name, "G") == 0) {
			idxG = c;
			numChannels++;
		} else if (strcmp(header.channels[c].name, "B") == 0) {
			idxB = c;
			numChannels++;
		} else if (strcmp(header.channels[c].name, "A") == 0) {
			idxA = c;
			numChannels++;
		}
	}

	int idxChannels[] = {-1, -1, -1, -1};
	int idxCur = 0;
	if (idxR != -1) {
		idxChannels[idxCur++] = idxR;
	}
	if (idxG != -1) {
		idxChannels[idxCur++] = idxG;
	}
	if (idxB != -1) {
		idxChannels[idxCur++] = idxB;
	}
	if (idxA != -1) {
		idxChannels[idxCur++] = idxA;
	}

	TinyImageFormat format = TinyImageFormat_UNDEFINED;

	switch (firstPixelType) {
	case TINYEXR_PIXELTYPE_UINT:
		switch (numChannels) {
		case 1: format = TinyImageFormat_R32_UINT;
			break;
		case 2: format = TinyImageFormat_R32G32_UINT;
			break;
		case 3:format = TinyImageFormat_R32G32B32_UINT;
			break;
		case 4: format = TinyImageFormat_R32G32B32A32_UINT;
			break;
		default: LOGERROR("EXR image has more than 4 channels.");
			return nullptr;
		}
	case TINYEXR_PIXELTYPE_FLOAT:
		switch (numChannels) {
		case 1: format = TinyImageFormat_R32_SFLOAT;
			break;
		case 2: format = TinyImageFormat_R32G32_SFLOAT;
			break;
		case 3:format = TinyImageFormat_R32G32B32_SFLOAT;
			break;
		case 4: format = TinyImageFormat_R32G32B32A32_SFLOAT;
			break;
		default: LOGERROR("EXR image has more than 4 channels.");
			return nullptr;
		}
	case TINYEXR_PIXELTYPE_HALF:
		switch (numChannels) {
		case 1: format = TinyImageFormat_R16_SFLOAT;
			break;
		case 2: format = TinyImageFormat_R16G16_SFLOAT;
			break;
		case 3:format = TinyImageFormat_R16G16B16_SFLOAT;
			break;
		case 4: format = TinyImageFormat_R16G16B16A16_SFLOAT;
			break;
		default: LOGERROR("EXR image has more than 4 channels.");
			return nullptr;
		}
	}

	// Create and extract the pixel data
	auto image = Image_Create(exrImage.width, exrImage.height, 1, 1, format);

	if (firstPixelType == TINYEXR_PIXELTYPE_FLOAT) {
		float *out = (float *) Image_RawDataPtr(image);

		for (uint32_t i = 0; i < image->width * image->height; i++) {
			for (int chn = 0; chn < numChannels; chn++) {
				out[i * numChannels + chn] = ((float **) exrImage.images)[idxChannels[chn]][i];
			}
		}
	} else if (firstPixelType == TINYEXR_PIXELTYPE_HALF) {
		uint16_t *out = (uint16_t *) Image_RawDataPtr(image);

		for (uint32_t i = 0; i < image->width * image->height; i++) {
			for (int chn = 0; chn < numChannels; chn++) {
				out[i * numChannels + chn] = ((uint16_t **) exrImage.images)[idxChannels[chn]][i];
			}
		}
	} else if (firstPixelType == TINYEXR_PIXELTYPE_UINT) {
		uint32_t *out = (uint32_t *) Image_RawDataPtr(image);

		for (uint32_t i = 0; i < image->width * image->height; i++) {
			for (int chn = 0; chn < numChannels; chn++) {
				out[i * numChannels + chn] = ((uint32_t **) exrImage.images)[idxChannels[chn]][i];
			}
		}
	}

	return image;
}
static void tinyktxddsCallbackError(void *user, char const *msg) {
	LOGERROR("Tiny_ ERROR: %s", msg);
}
static void *tinyktxddsCallbackAlloc(void *user, size_t size) {
	return MEMORY_MALLOC(size);
}
static void tinyktxddsCallbackFree(void *user, void *data) {
	MEMORY_FREE(data);
}
static size_t tinyktxddsCallbackRead(void *user, void *data, size_t size) {
	auto handle = (VFile_Handle) user;
	return VFile_Read(handle, data, size);
}
static bool tinyktxddsCallbackSeek(void *user, int64_t offset) {
	auto handle = (VFile_Handle) user;
	return VFile_Seek(handle, offset, VFile_SD_Begin);

}
static int64_t tinyktxddsCallbackTell(void *user) {
	auto handle = (VFile_Handle) user;
	return VFile_Tell(handle);
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadKTX(VFile_Handle handle) {
	TinyKtx_Callbacks callbacks{
			&tinyktxddsCallbackError,
			&tinyktxddsCallbackAlloc,
			&tinyktxddsCallbackFree,
			&tinyktxddsCallbackRead,
			&tinyktxddsCallbackSeek,
			&tinyktxddsCallbackTell
	};

	auto ctx = TinyKtx_CreateContext(&callbacks, handle);
	bool headerOkay = TinyKtx_ReadHeader(ctx);
	if (!headerOkay) {
		TinyKtx_DestroyContext(ctx);
		return nullptr;
	}

	uint32_t w = TinyKtx_Width(ctx);
	uint32_t h = TinyKtx_Height(ctx);
	uint32_t d = TinyKtx_Depth(ctx);
	uint32_t s = TinyKtx_ArraySlices(ctx);
	TinyImageFormat fmt = TinyImageFormat_FromTinyKtxFormat(TinyKtx_GetFormat(ctx));
	if (fmt == TinyImageFormat_UNDEFINED) {
		TinyKtx_DestroyContext(ctx);
		return nullptr;
	}

	Image_ImageHeader const *topImage = nullptr;
	Image_ImageHeader const *prevImage = nullptr;
	for (auto i = 0u; i < TinyKtx_NumberOfMipmaps(ctx); ++i) {
		Image_ImageHeader const *image = nullptr;
		if (TinyKtx_IsCubemap(ctx)) {
			image = Image_CreateCubemapArrayNoClear(w, h, s, fmt);
		} else if (TinyKtx_Is3D(ctx)) {
			image = Image_Create3DNoClear(w, h, d, fmt);
		} else {
			image = Image_Create2DArrayNoClear(w, h, s, fmt);
		}

		if (!image)
			break;
		if (i == 0)
			topImage = image;

		if (TinyKtx_IsMipMapLevelUnpacked(ctx, i)) {
			uint32_t const srcStride = TinyKtx_UnpackedRowStride(ctx, i);
			uint32_t const dstStride = (uint32_t) Image_ByteCountPerRowOf(image);

			auto src = (uint8_t const *) TinyKtx_ImageRawData(ctx, i);
			auto dst = (uint8_t *) Image_RawDataPtr(image);

			for (uint32_t ww = 0u; ww < image->slices; ++ww) {
				for (uint32_t zz = 0; zz < image->depth; ++zz) {
					for (uint32_t yy = 0; yy < image->height; ++yy) {
						memcpy(dst, src, dstStride);
						src += srcStride;
						dst += dstStride;
					}
				}
			}

		} else {
			// fast path data is packed we can just copy
			size_t const expectedSize = Image_ByteCountOf(image);
			size_t const fileSize = TinyKtx_ImageSize(ctx, i);
			if (expectedSize > fileSize) {
				LOGERROR("KTX file %s mipmap %i size error %liu > %liu", VFile_GetName(handle), i, expectedSize, fileSize);
				Image_Destroy(topImage);
				TinyKtx_DestroyContext(ctx);
				return nullptr;
			}
			memcpy(Image_RawDataPtr(image), TinyKtx_ImageRawData(ctx, i), Image_ByteCountOf(image));
		}
		if (prevImage) {
			auto p = (Image_ImageHeader *) prevImage;
			p->nextType = Image_NextType::Image_NT_MipMap;
			p->nextImage = image;
		}
		if (w > 1)
			w = w / 2;
		if (h > 1)
			h = h / 2;
		if (d > 1)
			d = d / 2;
		prevImage = image;
	}

	TinyKtx_DestroyContext(ctx);
	return topImage;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadDDS(VFile_Handle handle) {
	TinyDDS_Callbacks callbacks{
			&tinyktxddsCallbackError,
			&tinyktxddsCallbackAlloc,
			&tinyktxddsCallbackFree,
			tinyktxddsCallbackRead,
			&tinyktxddsCallbackSeek,
			&tinyktxddsCallbackTell
	};

	auto ctx = TinyDDS_CreateContext(&callbacks, handle);
	bool headerOkay = TinyDDS_ReadHeader(ctx);
	if (!headerOkay) {
		TinyDDS_DestroyContext(ctx);
		return nullptr;
	}

	uint32_t w = TinyDDS_Width(ctx);
	uint32_t h = TinyDDS_Height(ctx);
	uint32_t d = TinyDDS_Depth(ctx);
	uint32_t s = TinyDDS_ArraySlices(ctx);
	TinyImageFormat fmt = TinyImageFormat_FromTinyDDSFormat(TinyDDS_GetFormat(ctx));
	if (fmt == TinyImageFormat_UNDEFINED) {
		TinyDDS_DestroyContext(ctx);
		return nullptr;
	}

	// DDS store mipmaps of each cubemap faces together
	// so face[0] -> all mipmaps for face 0
	// so face[1] -> all mipmaps for face 1
	// etc.
	// we want it the other way
	// mip[0] -> face[0], face[1], etc.
	// mip[1] -> face[0], face[1], etc.

	Image_ImageHeader const *images[TINYDDS_MAX_MIPMAPLEVELS];
	for (auto i = 0u; i < TinyDDS_NumberOfMipmaps(ctx); ++i) {
		Image_ImageHeader const *image = nullptr;
		if (TinyDDS_IsCubemap(ctx)) {
			image = Image_CreateCubemapArrayNoClear(w, h, s, fmt);
		} else if (TinyDDS_Is3D(ctx)) {
			image = Image_Create3DNoClear(w, h, d, fmt);
		} else {
			image = Image_Create2DArrayNoClear(w, h, s, fmt);
		}
		images[i] = image;

		size_t const expectedSize = Image_ByteCountOf(image);
		size_t const fileSize = TinyDDS_ImageSize(ctx, i);
		if (expectedSize != fileSize) {
			LOGERROR("DDS file %s mipmap %i size error %liu < %liu", VFile_GetName(handle), i, expectedSize, fileSize);
			for (auto j = 0u; j < TinyDDS_NumberOfMipmaps(ctx); ++j) {
				Image_Destroy(images[j]);
			}
			TinyDDS_DestroyContext(ctx);
			return nullptr;
		}
		memcpy(Image_RawDataPtr(image), TinyDDS_ImageRawData(ctx, i), fileSize);

		if (i > 0) {
			auto p = (Image_ImageHeader *) images[i - 1];
			p->nextType = Image_NextType::Image_NT_MipMap;
			p->nextImage = image;
		}

		if (w > 1) {
			w = w / 2;
		}
		if (h > 1) {
			h = h / 2;
		}
		if (d > 1) {
			d = d / 2;
		}
	}

	TinyDDS_DestroyContext(ctx);
	return images[0];
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadBasisU(VFile_Handle handle) {
	Image_BasisUHandle basisU = Image_CreateBasisU(handle);
	if (!basisU) {
		return nullptr;
	}
	bool okay = Image_BasisURead(basisU);
	if (!okay) {
		Image_DestroyBasisU(basisU);
		return nullptr;
	}
	if (Image_BasisUImageCount(basisU) == 0) {
		Image_DestroyBasisU(basisU);
		return nullptr;
	}
	// we only support the first image for now
	Image_ImageHeader const *image = Image_BasisUReadImage(basisU, 0);
	Image_DestroyBasisU(basisU);

	return image;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_Load(VFile_Handle handle) {
	VFile::File *file = VFile::FromHandle(handle);
	tinystl::string name = file->GetName();
	name = name.to_lower();
	if (auto pos = name.rfind('.')) {
		if (pos == tinystl::string_view::npos) {
			return nullptr;
		}

		tinystl::string_view ext(name.data() + pos + 1, name.size() - pos - 1);

		switch (Utils::CompileTimeHash(ext)) {
			case "basis"_hash:
			case "basisu"_hash: return Image_LoadBasisU(handle);
			case "dds"_hash:return Image_LoadDDS(handle);
			case "pvr"_hash:return Image_LoadPVR(handle);
			case "exr"_hash:return Image_LoadEXR(handle);
			case "hdr"_hash:return Image_LoadHDR(handle);
			case "jpg"_hash:
			case "jpeg"_hash:
			case "png"_hash:
			case "tga"_hash:
			case "bmp"_hash:
			case "psd"_hash:
			case "gif"_hash:
			case "pic"_hash:
			case "pnm"_hash:
			case "ppm"_hash:return Image_LoadLDR(handle);
				//case "ktx2"_hash:
			case "ktx"_hash: return Image_LoadKTX(handle);
			default:return nullptr;
		}
	} else {
		return nullptr;
	}
}
