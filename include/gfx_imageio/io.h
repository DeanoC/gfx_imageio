#pragma once
#ifndef GFX_IMAGEIO_IO_H
#define GFX_IMAGEIO_IO_H

#include "al2o3_platform/platform.h"
#include "gfx_image/image.h"
#include "al2o3_vfile/vfile.h"

AL2O3_EXTERN_C Image_ImageHeader *Image_LoadDDS(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader *Image_LoadPVR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader *Image_LoadLDR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader *Image_LoadHDR(VFile_Handle handle);
AL2O3_EXTERN_C Image_ImageHeader *Image_LoadEXR(VFile_Handle handle);

AL2O3_EXTERN_C bool Image_SaveDDS(Image_ImageHeader *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveTGA(Image_ImageHeader *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveBMP(Image_ImageHeader *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SavePNG(Image_ImageHeader *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveJPG(Image_ImageHeader *image, VFile_Handle handle);
AL2O3_EXTERN_C bool Image_SaveHDR(Image_ImageHeader *image, VFile_Handle handle);

// try to figure out which format the file is in and load it
AL2O3_EXTERN_C Image_ImageHeader *Image_Load(VFile_Handle handle);

#endif //WYRD_IMAGE_IO_HPP
