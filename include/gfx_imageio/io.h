#pragma once
#ifndef GFX_IMAGEIO_IO_H
#define GFX_IMAGEIO_IO_H

#include "al2o3_platform/platform.h"
#include "gfx_image/image.h"
#include "al2o3_vfile/vfile.h"

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadDDS(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadPVR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadLDR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadHDR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadEXR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadKTX(VFile_Handle handle);

AL2O3_EXTERN_C bool Image_SaveAsDDS(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsTGA(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsBMP(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsPNG(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsJPG(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsHDR(Image_ImageHeader const *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveAsKTX(Image_ImageHeader const *image, VFile_Handle handle);

AL2O3_EXTERN_C bool Image_CanSaveAsDDS(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsTGA(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsBMP(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsPNG(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsJPG(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsHDR(Image_ImageHeader const *image);
AL2O3_EXTERN_C bool Image_CanSaveAsKTX(Image_ImageHeader const *image);

// try to figure out which format the file is in and load it
AL2O3_EXTERN_C Image_ImageHeader const *Image_Load(VFile_Handle handle);

#endif //WYRD_IMAGE_IO_HPP
