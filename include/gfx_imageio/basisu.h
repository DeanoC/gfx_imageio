#pragma once
#ifdef USE_BASISU_TRANSCODER

#include "al2o3_platform/platform.h"
#include "gfx_image/image.h"
#include "al2o3_vfile/vfile.h"

typedef struct Image_BasisU *Image_BasisUHandle;

AL2O3_EXTERN_C Image_BasisUHandle Image_CreateBasisU(VFile_Handle file);
AL2O3_EXTERN_C void Image_DestroyBasisU(Image_BasisUHandle basisu);
AL2O3_EXTERN_C bool Image_BasisURead(Image_BasisUHandle basisu);
AL2O3_EXTERN_C uint32_t Image_BasisUImageCount(Image_BasisUHandle basisu);
AL2O3_EXTERN_C Image_ImageHeader const *Image_BasisUReadImage(Image_BasisUHandle basisu, uint32_t imageIndex);

#endif