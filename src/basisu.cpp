#ifdef USE_BASISU_TRANSCODER
#include "al2o3_platform/platform.h"
#include "al2o3_memory/memory.h"
#include "tiny_imageformat/tinyimageformat_decode.h"
#include "gfx_image/image.h"
#include "gfx_image/create.h"
#include "gfx_imageio/basisu.h"
#include "al2o3_vfile/vfile.h"
#include <algorithm>

namespace {
#include "basisu_transcoder/basisu_file_headers.h"
#include "basisu_transcoder/basisu_transcoder_internal.h"
#include "basisu_transcoder/basisu_global_selector_palette.h"
#include "basisu_transcoder/basisu_transcoder.h"
#include "basisu_transcoder/basisu_transcoder.cpp"
}
struct Image_BasisU {
	VFile_Handle fileHandle;
	int64_t fileStart;

	basist::etc1_global_selector_codebook codebook;
	basist::basisu_transcoder *transcoder;

	uint32_t fileSize;
	void *fileMem;
	bool dataLoaded;
};

AL2O3_EXTERN_C Image_BasisUHandle Image_CreateBasisU(VFile_Handle file) {
	using namespace basist;

	auto deBasisU = (Image_BasisU *) MEMORY_CALLOC(1, sizeof(Image_BasisU));
	if (!deBasisU) {
		return nullptr;
	}

	// ensure static tables are created
	basisu_transcoder_init();
	deBasisU->codebook.init(g_global_selector_cb_size, g_global_selector_cb);

	deBasisU->transcoder = (basisu_transcoder *) MEMORY_CALLOC(1, sizeof(basisu_transcoder));
	if (!deBasisU->transcoder) {
		MEMORY_FREE(deBasisU);
		return nullptr;
	}
	new(deBasisU->transcoder) basisu_transcoder(&deBasisU->codebook);

	deBasisU->fileHandle = file;
	deBasisU->fileStart = VFile_Tell(deBasisU->fileHandle);

	return deBasisU;
}

AL2O3_EXTERN_C void Image_DestroyBasisU(Image_BasisUHandle basisu) {
	using namespace basist;
	if (!basisu) {
		return;
	}

	basisu->codebook.~etc1_global_selector_codebook();
	basisu->transcoder->~basisu_transcoder();

	MEMORY_FREE(basisu->fileMem);
	MEMORY_FREE(basisu->transcoder);
	MEMORY_FREE(basisu);
}

AL2O3_EXTERN_C bool Image_BasisURead(Image_BasisUHandle basisu) {
	using namespace basist;

	if (!basisu) {
		return false;
	}

	VFile_Seek(basisu->fileHandle, basisu->fileStart, VFile_SD_Begin);
	basisu->fileMem = MEMORY_MALLOC(sizeof(basis_file_header));
	size_t headerBytes = VFile_Read(basisu->fileHandle, basisu->fileMem, sizeof(basis_file_header));
	if (headerBytes != sizeof(basis_file_header)) {
		LOGWARNING("basisu header read header error");
		return false;
	}

	// basic header checks
	auto header = (basis_file_header const *) basisu->fileMem;
	if ((header->m_sig != basis_file_header::cBASISSigValue) ||
			(header->m_header_size != sizeof(basis_file_header))) {
		LOGWARNING("basisu header has an invalid signature or header size");
		return false;
	}

	basisu->fileSize = sizeof(basis_file_header) + header->m_data_size;
	if (!basisu->transcoder->validate_header(basisu->fileMem, basisu->fileSize)) {
		return false;
	}
	basisu->dataLoaded = false; // tell first image load to actually read the data

	return true;
}
AL2O3_EXTERN_C uint32_t Image_BasisUImageCount(Image_BasisUHandle basisu) {
	if (!basisu) {
		return 0;
	}
	return basisu->transcoder->get_total_images(basisu->fileMem, basisu->fileSize);
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_BasisUReadImage(Image_BasisUHandle basisu, uint32_t imageIndex) {
	using namespace basist;
	if (!basisu) {
		return nullptr;
	}

	if (basisu->dataLoaded == false) {
		basisu->fileMem = MEMORY_REALLOC(basisu->fileMem, basisu->fileSize);
		VFile_Seek(basisu->fileHandle, basisu->fileStart, VFile_SD_Begin);

		// tehcnically we don't need to re-read the header but meh easier to just load the lot
		size_t bytes = VFile_Read(basisu->fileHandle, basisu->fileMem, basisu->fileSize);
		if (bytes != basisu->fileSize) {
			return nullptr;
		}
		basisu->dataLoaded = true;
	}

	basist::basisu_image_info info;
	bool okay = basisu->transcoder->get_image_info(basisu->fileMem, basisu->fileSize, info, imageIndex);
	if (!okay) {
		return nullptr;
	}

	Image_ImageHeader const *topImage = nullptr;
	Image_ImageHeader const *lastImage = nullptr;

	for (uint32_t n = 0; n < info.m_total_levels; n++) {
		uint32_t w = 0;
		uint32_t h = 0;
		uint32_t blockCount;

		okay = basisu->transcoder->get_image_level_desc(basisu->fileMem, basisu->fileSize, imageIndex, n, w, h, blockCount);
		if (!okay) {
			return nullptr;
		}

		okay = basisu->transcoder->start_transcoding(basisu->fileMem, basisu->fileSize);
		if (!okay) {
			return nullptr;
		}

		TinyImageFormat format = TinyImageFormat_DXBC1_RGB_UNORM;
		transcoder_texture_format transcodeFormat = cTFBC1;

		if (info.m_alpha_flag) {
			format = TinyImageFormat_DXBC7_UNORM;
			transcodeFormat = cTFBC7_M5;
		}
		// always BC7 for now
		auto image = Image_Create2DNoClear(w, h, format);
		if (!image) {
			return topImage;
		}
		if (n == 0) {
			topImage = image;
			lastImage = topImage;
		} else {
			((Image_ImageHeader *) lastImage)->nextImage = image;
			((Image_ImageHeader *) lastImage)->nextType = Image_NT_MipMap;
		}

		okay = basisu->transcoder->transcode_image_level(basisu->fileMem,
																										 basisu->fileSize,
																										 imageIndex,
																										 n,
																										 Image_RawDataPtr(image),
																										 blockCount,
																										 transcodeFormat);
	}

	return topImage;
}

#endif