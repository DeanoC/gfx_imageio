#include "al2o3_platform/platform.h"
#include "al2o3_cmath/scalar.h"
#include "al2o3_stb/stb_image.h"
#include "al2o3_vfile/vfile.hpp"
#include "gfx_imageformat/format.h"
#include "gfx_imageformat/formatcracker.h"
#include "gfx_image/image.h"
#include "gfx_image/utils.h"
#include "al2o3_syoyo/tiny_exr.hpp"
#include "utils_misccpp/compiletimehash.hpp"
#include "dds.hpp"
#include <float.h>
#include "tiny_ktx/tinyktx.h"

// parts of the DDS and PVR loader borrowed from https://github.com/ConfettiFX/The-Forge

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

// Load Image Data form mData functions

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadDDS(VFile_Handle handle) {
  using namespace Image;
  DDSHeader header;

  VFile::File *file = VFile::File::FromHandle(handle);
  file->Read(&header, sizeof(header));

  if (header.mDWMagic != MAKE_CHAR4('D', 'D', 'S', ' ')) {
    return nullptr;
  }

  Image_ImageHeader const *image = nullptr;
  ImageFormat format = ImageFormat_UNDEFINED;

  if (header.mPixelFormat.mDWFourCC == MAKE_CHAR4('D', 'X', '1', '0')) {
    DDSHeaderDX10 dx10Header;
    file->Read(&dx10Header, sizeof(dx10Header));

    switch (dx10Header.mDXGIFormat) {
      case DDS_DXGI_FORMAT_R32G32B32A32_FLOAT: format = ImageFormat_R32G32B32A32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32A32_UINT: format = ImageFormat_R32G32B32A32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32A32_SINT: format = ImageFormat_R32G32B32A32_SINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_FLOAT: format = ImageFormat_R32G32B32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_UINT: format = ImageFormat_R32G32B32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_SINT: format = ImageFormat_R32G32B32_SINT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_FLOAT: format = ImageFormat_R16G16B16A16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_UNORM: format = ImageFormat_R16G16B16A16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_UINT: format = ImageFormat_R16G16B16A16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_SNORM: format = ImageFormat_R16G16B16A16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_SINT: format = ImageFormat_R16G16B16A16_SINT;
        break;
      case DDS_DXGI_FORMAT_R32G32_FLOAT: format = ImageFormat_R32G32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32_UINT: format = ImageFormat_R32G32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32_SINT: format = ImageFormat_R32G32_SINT;
        break;
      case DDS_DXGI_FORMAT_R10G10B10A2_UNORM: format = ImageFormat_A2B10G10R10_UNORM_PACK32;
        break;
      case DDS_DXGI_FORMAT_R10G10B10A2_UINT: format = ImageFormat_A2B10G10R10_UINT_PACK32;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UNORM: format = ImageFormat_R8G8B8A8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: format = ImageFormat_R8G8B8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UINT: format = ImageFormat_R8G8B8A8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_SNORM: format = ImageFormat_R8G8B8A8_SNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_SINT: format = ImageFormat_R8G8B8A8_SINT;
        break;
      case DDS_DXGI_FORMAT_R16G16_FLOAT: format = ImageFormat_R16G16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R16G16_UNORM: format = ImageFormat_R16G16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16_UINT: format = ImageFormat_R16G16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16G16_SNORM: format = ImageFormat_R16G16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16_SINT: format = ImageFormat_R16G16_SINT;
        break;
      case DDS_DXGI_FORMAT_D32_FLOAT: format = ImageFormat_D32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32_FLOAT: format = ImageFormat_R32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32_UINT: format = ImageFormat_R32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32_SINT: format = ImageFormat_R32_SINT;
        break;
      case DDS_DXGI_FORMAT_R8G8_UNORM: format = ImageFormat_R8G8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8_UINT: format = ImageFormat_R8G8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8G8_SNORM: format = ImageFormat_R8G8_SNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8_SINT: format = ImageFormat_R8G8_SINT;
        break;
      case DDS_DXGI_FORMAT_R16_FLOAT: format = ImageFormat_R16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_D16_UNORM: format = ImageFormat_D16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16_UNORM: format = ImageFormat_R16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16_UINT: format = ImageFormat_R16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16_SNORM: format = ImageFormat_R16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16_SINT: format = ImageFormat_R16_SINT;
        break;
      case DDS_DXGI_FORMAT_R8_UNORM: format = ImageFormat_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8_UINT: format = ImageFormat_R8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8_SNORM: format = ImageFormat_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8_SINT: format = ImageFormat_R8_SINT;
        break;
      case DDS_DXGI_FORMAT_A8_UNORM: format = ImageFormat_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_BC1_UNORM: format = ImageFormat_BC1_RGB_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC1_UNORM_SRGB: format = ImageFormat_BC1_RGBA_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC2_UNORM: format = ImageFormat_BC2_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC2_UNORM_SRGB: format = ImageFormat_BC2_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC3_UNORM: format = ImageFormat_BC3_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC3_UNORM_SRGB: format = ImageFormat_BC2_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC4_UNORM: format = ImageFormat_BC4_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC4_SNORM: format = ImageFormat_BC4_SNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC5_UNORM: format = ImageFormat_BC5_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC5_SNORM: format = ImageFormat_BC5_SNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_B5G6R5_UNORM: format = ImageFormat_B5G6R5_UNORM_PACK16;
        break;
      case DDS_DXGI_FORMAT_B5G5R5A1_UNORM: format = ImageFormat_B5G5R5A1_UNORM_PACK16;
        break;
      case DDS_DXGI_FORMAT_B8G8R8A8_UNORM: format = ImageFormat_B8G8R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_B8G8R8X8_UNORM: format = ImageFormat_B8G8R8A8_UNORM;
        break;
      case DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: format = ImageFormat_B8G8R8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: format = ImageFormat_B8G8R8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_BC6H_UF16: format = ImageFormat_BC6H_UFLOAT_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC6H_SF16: format = ImageFormat_BC6H_SFLOAT_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC7_UNORM: format = ImageFormat_BC7_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC7_UNORM_SRGB: format = ImageFormat_BC7_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_P8: format = ImageFormat_UNDEFINED;
        break;
      case DDS_DXGI_FORMAT_B4G4R4A4_UNORM: format = ImageFormat_B4G4R4A4_UNORM_PACK16;
        break;
      default: return nullptr;
    }
  } else {
    switch (header.mPixelFormat.mDWFourCC) {
      case 34: format = ImageFormat_R16G16_UNORM;
        break;
      case 36: format = ImageFormat_R16G16B16A16_UNORM;
        break;
      case 111: format = ImageFormat_R16_SFLOAT;
        break;
      case 112: format = ImageFormat_R16G16_SFLOAT;
        break;
      case 113: format = ImageFormat_R16G16B16A16_SFLOAT;
        break;
      case 114: format = ImageFormat_R32_SFLOAT;
        break;
      case 115: format = ImageFormat_R32G32_SFLOAT;
        break;
      case 116: format = ImageFormat_R32G32B32A32_SFLOAT;
        break;

/*      case MAKE_CHAR4('A', 'T', 'C', ' '): format = ATC;
        break;
      case MAKE_CHAR4('A', 'T', 'C', 'A'): format = ATCA;
        break;
      case MAKE_CHAR4('A', 'T', 'C', 'I'): format = ATCI;
        break;
      case MAKE_CHAR4('A', 'T', 'I', '1'): format = ATI1N;
        break;
      case MAKE_CHAR4('A', 'T', 'I', '2'): format = ATI2N;
        break;
      case MAKE_CHAR4('E', 'T', 'C', ' '): format = ETC1;
        break; */ //TODO
      case MAKE_CHAR4('D', 'X', 'T', '1'): format = ImageFormat_BC1_RGBA_UNORM_BLOCK;
        break;
      case MAKE_CHAR4('D', 'X', 'T', '3'): format = ImageFormat_BC2_UNORM_BLOCK;
        break;
      case MAKE_CHAR4('D', 'X', 'T', '5'): format = ImageFormat_BC5_UNORM_BLOCK;
        break;
      default:
        switch (header.mPixelFormat.mDWRGBBitCount) {
          case 8: format = ImageFormat_R8_UNORM;
            break;
          case 16:
            // TODO need to swizzle
            format = (header.mPixelFormat.mDWRGBAlphaBitMask == 0xF000)
                     ? ImageFormat_B4G4R4A4_UNORM_PACK16
                     : (header.mPixelFormat.mDWRGBAlphaBitMask == 0xFF00)
                       ? ImageFormat_R8G8_UNORM
                       : (header.mPixelFormat.mDWBBitMask == 0x1F) ?
                         ImageFormat_R5G6B5_UNORM_PACK16 :
                         ImageFormat_R16_UNORM;
            break;
          case 24: format = ImageFormat_R8G8B8_UNORM;
            break;
          case 32:
            format = (header.mPixelFormat.mDWRBitMask == 0x3FF00000) ?
                     ImageFormat_A2R10G10B10_UNORM_PACK32 :
                     ImageFormat_R8G8B8A8_UNORM;
            break;
          default:return nullptr;
        }
    }
  }
  if (format == ImageFormat_UNDEFINED) { return nullptr; }

  image = Image_Create(header.mDWWidth,
                       header.mDWHeight,
                       (header.mDWDepth == 0) ? 1 : header.mDWDepth,
                       (header.mCaps.mDWCaps2 & DDSCAPS2_CUBEMAP) ? 6 : 1,
                       format);
  file->Read(Image_RawDataPtr(image), Image_ByteCountOf(image));

  if (header.mDWMipMapCount != 1) {
    Image_CreateMipMapChain(image, true);
  }

  /*TODO Deano use DDS mipmaps if store in file!
  size_t size = Image_BytesForImageAndMipMaps(image);
  if (header.mCaps.mDWCaps2 & DDSCAPS2_CUBEMAP) {
    for (int face = 0; face < 6; face++) {
      for (uint32_t mipMapLevel = 0; mipMapLevel < mMipMapCount; mipMapLevel++) {
        int faceSize = GetMipMappedSize(mipMapLevel, 1) / 6;
        unsigned char *src = GetPixels(pData, mipMapLevel) + face * faceSize;

        file->Read(src, faceSize);
        //MemFopen::FileRead(src, 1, faceSize, file);
      }
    }
  } else {
    file->Read(pData, size);
  }

  if ((mFormat == RGB8 || mFormat == RGBA8) && header.mPixelFormat.mDWBBitMask == 0xFF) {
    int nChannels = GetChannelCount(mFormat);
    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
  }
*/
  return image;
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_LoadPVR(VFile_Handle handle) {
  // TODO: Image
  // - no support for PVRTC2 at the moment since it isn't supported on iOS devices.
  // - only new PVR header V3 is supported at the moment.  Should we add legacy for V2 and V1?
  // - metadata is ignored for now.  Might be useful to implement it if the need for metadata arises (eg. padding, atlas coordinates, orientations, border data, etc...).
  // - flags are also ignored for now.  Currently a flag of 0x02 means that the color have been pre-multiplied byt the alpha values.

  // Assumptions:
  // - it's assumed that the texture is already twiddled (ie. Morton).  This should always be the case for PVRTC V3.

  PVR_Texture_Header header;
  VFile::File *file = VFile::File::FromHandle(handle);
  file->Read(&header, sizeof(header));

  if (header.mVersion != gPvrtexV3HeaderVersion) {
    LOGERRORF("Load PVR failed: Not a valid PVR V3 header.");
    return nullptr;
  }

  if (header.mPixelFormat > 3) {
    LOGERRORF("Load PVR failed: Not a supported PVR pixel format.  Only PVRTC is supported at the moment.");
    return nullptr;
  }

  if (header.mNumSurfaces > 1 && header.mNumFaces > 1) {
    LOGERRORF("Load PVR failed: Loading arrays of cubemaps isn't supported.");
    return nullptr;
  }

  uint32_t width = header.mWidth;
  uint32_t height = header.mHeight;
  uint32_t depth = header.mDepth;
  uint32_t slices = header.mNumSurfaces * header.mNumFaces;
  uint32_t mipMapCount = header.mNumMipMaps;
  ImageFormat format = ImageFormat_UNDEFINED;

  bool isSrgb = (header.mColorSpace == 1);

  switch (header.mPixelFormat) {
    case 0:format = isSrgb ? ImageFormat_PVR_2BPP_SRGB_BLOCK : ImageFormat_PVR_2BPP_BLOCK;
      break;
    case 1:format = isSrgb ? ImageFormat_PVR_2BPPA_SRGB_BLOCK : ImageFormat_PVR_2BPPA_BLOCK;
      break;
    case 2:format = isSrgb ? ImageFormat_PVR_4BPP_SRGB_BLOCK : ImageFormat_PVR_4BPP_BLOCK;
      break;
    case 3:format = isSrgb ? ImageFormat_PVR_4BPPA_SRGB_BLOCK : ImageFormat_PVR_4BPPA_BLOCK;
      break;
    default:    // NOT SUPPORTED
      LOGERRORF("Load PVR failed: pixel type not supported. ");
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

  int w = 0, h = 0, cmp = 0, requiredCmp = 0;
  stbi_info_from_callbacks(&callbacks, handle, &w, &h, &cmp);

  if (w == 0 || h == 0 || cmp == 0) {
    return nullptr;
  }

  requiredCmp = cmp;
  if (cmp == 3) {
    requiredCmp = 4;
  }

  ImageFormat format = ImageFormat_UNDEFINED;
  uint64_t memoryRequirement = sizeof(stbi_uc) * w * h * requiredCmp;

  switch (requiredCmp) {
    case 1: format = ImageFormat_R8_UNORM;
      break;
    case 2: format = ImageFormat_R8G8_UNORM;
      break;
    case 3: format = ImageFormat_R8G8B8_UNORM;
      break;
    case 4: format = ImageFormat_R8G8B8A8_UNORM;
      break;
  }

  VFile_Seek(handle, origin, VFile_SD_Begin);
  stbi_uc *uncompressed = stbi_load_from_callbacks(&callbacks, handle, &w, &h, &cmp, requiredCmp);
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

  ImageFormat format = ImageFormat_UNDEFINED;
  switch (requiredCmp) {
    case 1: format = ImageFormat_R32_SFLOAT;
      break;
    case 2: format = ImageFormat_R32G32_SFLOAT;
      break;
    case 3: format = ImageFormat_R32G32B32_SFLOAT;
      break;
    case 4: format = ImageFormat_R32G32B32A32_SFLOAT;
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
  VFile::File *file = VFile::File::FromHandle(handle);

  using namespace tinyexr;
  EXRVersion version;
  EXRHeader header;
  InitEXRHeader(&header);
  int ret = 0;
  ret = ParseEXRVersion(&version, handle);
  if (ret != 0) {
    LOGERRORF("Parse EXR error");
    return nullptr;
  }

  file->Seek(0, VFile_SD_Begin);
  ret = ParseEXRHeader(&header, &version, handle);
  if (ret != 0) {
    LOGERRORF("Parse EXR error");
    return nullptr;
  }

  // all support homogenous image (all formats the same)
  int firstPixelType;
  for (int i = 0; i < Math_MinI32(header.num_channels,4); i++) {
    if(i == 0) {
      firstPixelType = header.pixel_types[0];
    } else {
      if(header.pixel_types[i] != firstPixelType) {
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
    LOGERRORF("Load EXR error\n");
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

  ImageFormat format = ImageFormat_UNDEFINED;

  if(firstPixelType == TINYEXR_PIXELTYPE_FLOAT) {
    switch (numChannels) {
      case 1: format = ImageFormat_R32_SFLOAT;
        break;
      case 2: format = ImageFormat_R32G32_SFLOAT;
        break;
      case 3:format = ImageFormat_R32G32B32_SFLOAT;
        break;
      case 4: format = ImageFormat_R32G32B32A32_SFLOAT;
        break;
    }
  } else if(firstPixelType == TINYEXR_PIXELTYPE_HALF) {
    switch (numChannels) {
      case 1: format = ImageFormat_R16_SFLOAT;
        break;
      case 2: format = ImageFormat_R16G16_SFLOAT;
        break;
      case 3:format = ImageFormat_R16G16B16_SFLOAT;
        break;
      case 4: format = ImageFormat_R16G16B16A16_SFLOAT;
        break;
    }
  } else {
    LOGERROR("EXR unsupported pixel type");
    return nullptr;
  }

  // Create and extract the pixel data
  auto image = Image_Create(exrImage.width, exrImage.height, 1, 1, format);

  if(firstPixelType == TINYEXR_PIXELTYPE_FLOAT) {
    float* out = (float*)Image_RawDataPtr(image);

    for (uint32_t i = 0; i < image->width * image->height; i++) {
      for (int chn = 0; chn < numChannels; chn++) {
        out[i * numChannels + chn] = ((float**) exrImage.images)[idxChannels[chn]][i];
      }
    }
  } else if(firstPixelType == TINYEXR_PIXELTYPE_HALF) {
    uint16_t* out = (uint16_t*)Image_RawDataPtr(image);

    for (uint32_t i = 0; i < image->width * image->height; i++) {
      for (int chn = 0; chn < numChannels; chn++) {
        out[i * numChannels + chn] = ((uint16_t**) exrImage.images)[idxChannels[chn]][i];
      }
    }
  }

  return image;
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
static size_t tinyktxCallbackRead(void *user, void* data, size_t size) {
	auto handle = (VFile_Handle) user;
	return VFile_Read(handle, data, size);
}
static bool tinyktxCallbackSeek(void *user, int64_t offset) {
	auto handle = (VFile_Handle) user;
	return VFile_Seek(handle, offset, VFile_SD_Begin);

}
static int64_t tinyktxCallbackTell(void *user) {
	auto handle = (VFile_Handle) user;
	return VFile_Tell(handle);
}



static ImageFormat ImageFormatToTinyKtxFormat(TinyKtx_Format format) {
	switch(format) {
		case TKTX_UNDEFINED: return ImageFormat_UNDEFINED;
		case TKTX_R4G4_UNORM_PACK8: return ImageFormat_R4G4_UNORM_PACK8;
		case TKTX_R4G4B4A4_UNORM_PACK16: return ImageFormat_R4G4B4A4_UNORM_PACK16;
		case TKTX_B4G4R4A4_UNORM_PACK16: return ImageFormat_B4G4R4A4_UNORM_PACK16;
		case TKTX_R5G6B5_UNORM_PACK16: return ImageFormat_R5G6B5_UNORM_PACK16;
		case TKTX_B5G6R5_UNORM_PACK16: return ImageFormat_B5G6R5_UNORM_PACK16;
		case TKTX_R5G5B5A1_UNORM_PACK16: return ImageFormat_R5G5B5A1_UNORM_PACK16;
		case TKTX_B5G5R5A1_UNORM_PACK16: return ImageFormat_B5G5R5A1_UNORM_PACK16;
		case TKTX_A1R5G5B5_UNORM_PACK16: return ImageFormat_A1R5G5B5_UNORM_PACK16;
		case TKTX_R8_UNORM: return ImageFormat_R8_UNORM;
		case TKTX_R8_SNORM: return ImageFormat_R8_SNORM;
		case TKTX_R8_UINT: return ImageFormat_R8_UINT;
		case TKTX_R8_SINT: return ImageFormat_R8_SINT;
		case TKTX_R8_SRGB: return ImageFormat_R8_SRGB;
		case TKTX_R8G8_UNORM: return ImageFormat_R8G8_UNORM;
		case TKTX_R8G8_SNORM: return ImageFormat_R8G8_SNORM;
		case TKTX_R8G8_UINT: return ImageFormat_R8G8_UINT;
		case TKTX_R8G8_SINT: return ImageFormat_R8G8_SINT;
		case TKTX_R8G8_SRGB: return ImageFormat_R8G8_SRGB;
		case TKTX_R8G8B8_UNORM: return ImageFormat_R8G8B8_UNORM;
		case TKTX_R8G8B8_SNORM: return ImageFormat_R8G8B8_SNORM;
		case TKTX_R8G8B8_UINT: return ImageFormat_R8G8B8_UINT;
		case TKTX_R8G8B8_SINT: return ImageFormat_R8G8B8_SINT;
		case TKTX_R8G8B8_SRGB: return ImageFormat_R8G8B8_SRGB;
		case TKTX_B8G8R8_UNORM: return ImageFormat_B8G8R8_UNORM;
		case TKTX_B8G8R8_SNORM: return ImageFormat_B8G8R8_SNORM;
		case TKTX_B8G8R8_UINT: return ImageFormat_B8G8R8_UINT;
		case TKTX_B8G8R8_SINT: return ImageFormat_B8G8R8_SINT;
		case TKTX_B8G8R8_SRGB: return ImageFormat_B8G8R8_SRGB;
		case TKTX_R8G8B8A8_UNORM: return ImageFormat_R8G8B8A8_UNORM;
		case TKTX_R8G8B8A8_SNORM: return ImageFormat_R8G8B8A8_SNORM;
		case TKTX_R8G8B8A8_UINT: return ImageFormat_R8G8B8A8_UINT;
		case TKTX_R8G8B8A8_SINT: return ImageFormat_R8G8B8A8_SINT;
		case TKTX_R8G8B8A8_SRGB: return ImageFormat_R8G8B8A8_SRGB;
		case TKTX_B8G8R8A8_UNORM: return ImageFormat_B8G8R8A8_UNORM;
		case TKTX_B8G8R8A8_SNORM: return ImageFormat_B8G8R8A8_SNORM;
		case TKTX_B8G8R8A8_UINT: return ImageFormat_B8G8R8A8_UINT;
		case TKTX_B8G8R8A8_SINT: return ImageFormat_B8G8R8A8_SINT;
		case TKTX_B8G8R8A8_SRGB: return ImageFormat_B8G8R8A8_SRGB;
		case TKTX_A8B8G8R8_UNORM_PACK32: return ImageFormat_A8B8G8R8_UNORM_PACK32;
		case TKTX_A8B8G8R8_SNORM_PACK32: return ImageFormat_A8B8G8R8_SNORM_PACK32;
		case TKTX_A8B8G8R8_UINT_PACK32: return ImageFormat_A8B8G8R8_UINT_PACK32;
		case TKTX_A8B8G8R8_SINT_PACK32: return ImageFormat_A8B8G8R8_SINT_PACK32;
		case TKTX_A8B8G8R8_SRGB_PACK32: return ImageFormat_A8B8G8R8_SRGB_PACK32;
		case TKTX_E5B9G9R9_UFLOAT_PACK32: return ImageFormat_E5B9G9R9_UFLOAT_PACK32;
		case TKTX_A2R10G10B10_UNORM_PACK32: return ImageFormat_A2R10G10B10_UNORM_PACK32;
		case TKTX_A2R10G10B10_UINT_PACK32: return ImageFormat_A2R10G10B10_UINT_PACK32;
		case TKTX_A2B10G10R10_UNORM_PACK32: return ImageFormat_A2B10G10R10_UNORM_PACK32;
		case TKTX_A2B10G10R10_UINT_PACK32: return ImageFormat_A2B10G10R10_UINT_PACK32;
		case TKTX_B10G11R11_UFLOAT_PACK32: return ImageFormat_B10G11R11_UFLOAT_PACK32;
		case TKTX_R16_UNORM: return ImageFormat_R16_UNORM;
		case TKTX_R16_SNORM: return ImageFormat_R16_SNORM;
		case TKTX_R16_UINT: return ImageFormat_R16_UINT;
		case TKTX_R16_SINT: return ImageFormat_R16_SINT;
		case TKTX_R16_SFLOAT: return ImageFormat_R16_SFLOAT;
		case TKTX_R16G16_UNORM: return ImageFormat_R16G16_UNORM;
		case TKTX_R16G16_SNORM: return ImageFormat_R16G16_SNORM;
		case TKTX_R16G16_UINT: return ImageFormat_R16G16_UINT;
		case TKTX_R16G16_SINT: return ImageFormat_R16G16_SINT;
		case TKTX_R16G16_SFLOAT: return ImageFormat_R16G16_SFLOAT;
		case TKTX_R16G16B16_UNORM: return ImageFormat_R16G16B16_UNORM;
		case TKTX_R16G16B16_SNORM: return ImageFormat_R16G16B16_SNORM;
		case TKTX_R16G16B16_UINT: return ImageFormat_R16G16B16_UINT;
		case TKTX_R16G16B16_SINT: return ImageFormat_R16G16B16_SINT;
		case TKTX_R16G16B16_SFLOAT: return ImageFormat_R16G16B16_SFLOAT;
		case TKTX_R16G16B16A16_UNORM: return ImageFormat_R16G16B16A16_UNORM;
		case TKTX_R16G16B16A16_SNORM: return ImageFormat_R16G16B16A16_SNORM;
		case TKTX_R16G16B16A16_UINT: return ImageFormat_R16G16B16A16_UINT;
		case TKTX_R16G16B16A16_SINT: return ImageFormat_R16G16B16A16_SINT;
		case TKTX_R16G16B16A16_SFLOAT: return ImageFormat_R16G16B16A16_SFLOAT;
		case TKTX_R32_UINT: return ImageFormat_R32_UINT;
		case TKTX_R32_SINT: return ImageFormat_R32_SINT;
		case TKTX_R32_SFLOAT: return ImageFormat_R32_SFLOAT;
		case TKTX_R32G32_UINT: return ImageFormat_R32G32_UINT;
		case TKTX_R32G32_SINT: return ImageFormat_R32G32_SINT;
		case TKTX_R32G32_SFLOAT: return ImageFormat_R32G32_SFLOAT;
		case TKTX_R32G32B32_UINT: return ImageFormat_R32G32B32_UINT;
		case TKTX_R32G32B32_SINT: return ImageFormat_R32G32B32_SINT;
		case TKTX_R32G32B32_SFLOAT: return ImageFormat_R32G32B32_SFLOAT;
		case TKTX_R32G32B32A32_UINT: return ImageFormat_R32G32B32A32_UINT;
		case TKTX_R32G32B32A32_SINT: return ImageFormat_R32G32B32A32_SINT;
		case TKTX_R32G32B32A32_SFLOAT: return ImageFormat_R32G32B32A32_SFLOAT;
		case TKTX_BC1_RGB_UNORM_BLOCK: return ImageFormat_BC1_RGB_UNORM_BLOCK;
		case TKTX_BC1_RGB_SRGB_BLOCK: return ImageFormat_BC1_RGB_SRGB_BLOCK;
		case TKTX_BC1_RGBA_UNORM_BLOCK: return ImageFormat_BC1_RGBA_UNORM_BLOCK;
		case TKTX_BC1_RGBA_SRGB_BLOCK: return ImageFormat_BC1_RGBA_SRGB_BLOCK;
		case TKTX_BC2_UNORM_BLOCK: return ImageFormat_BC2_UNORM_BLOCK;
		case TKTX_BC2_SRGB_BLOCK: return ImageFormat_BC2_SRGB_BLOCK;
		case TKTX_BC3_UNORM_BLOCK: return ImageFormat_BC3_UNORM_BLOCK;
		case TKTX_BC3_SRGB_BLOCK: return ImageFormat_BC3_SRGB_BLOCK;
		case TKTX_BC4_UNORM_BLOCK: return ImageFormat_BC4_UNORM_BLOCK;
		case TKTX_BC4_SNORM_BLOCK: return ImageFormat_BC4_SNORM_BLOCK;
		case TKTX_BC5_UNORM_BLOCK: return ImageFormat_BC5_UNORM_BLOCK;
		case TKTX_BC5_SNORM_BLOCK: return ImageFormat_BC5_SNORM_BLOCK;
		case TKTX_BC6H_UFLOAT_BLOCK: return ImageFormat_BC6H_UFLOAT_BLOCK;
		case TKTX_BC6H_SFLOAT_BLOCK: return ImageFormat_BC6H_SFLOAT_BLOCK;
		case TKTX_BC7_UNORM_BLOCK: return ImageFormat_BC7_UNORM_BLOCK;
		case TKTX_BC7_SRGB_BLOCK: return ImageFormat_BC7_SRGB_BLOCK;
		case TKTX_PVR_2BPP_BLOCK: return ImageFormat_PVR_2BPP_BLOCK;
		case TKTX_PVR_2BPPA_BLOCK: return ImageFormat_PVR_2BPPA_BLOCK;
		case TKTX_PVR_4BPP_BLOCK: return ImageFormat_PVR_4BPP_BLOCK;
		case TKTX_PVR_4BPPA_BLOCK: return ImageFormat_PVR_4BPPA_BLOCK;
		case TKTX_PVR_2BPP_SRGB_BLOCK: return ImageFormat_PVR_2BPP_SRGB_BLOCK;
		case TKTX_PVR_2BPPA_SRGB_BLOCK: return ImageFormat_PVR_2BPPA_SRGB_BLOCK;
		case TKTX_PVR_4BPP_SRGB_BLOCK: return ImageFormat_PVR_4BPP_SRGB_BLOCK;
		case TKTX_PVR_4BPPA_SRGB_BLOCK: return ImageFormat_PVR_4BPPA_SRGB_BLOCK;
		/*
		case TKTX_ETC2_R8G8B8_UNORM_BLOCK: return ImageFormat_ETC2_R8G8B8_UNORM_BLOCK;
		case TKTX_ETC2_R8G8B8A1_UNORM_BLOCK: return ImageFormat_ETC2_R8G8B8A1_UNORM_BLOCK;
		case TKTX_ETC2_R8G8B8A8_UNORM_BLOCK: return ImageFormat_ETC2_R8G8B8A8_UNORM_BLOCK;
		case TKTX_ETC2_R8G8B8_SRGB_BLOCK: return ImageFormat_ETC2_R8G8B8_SRGB_BLOCK;
		case TKTX_ETC2_R8G8B8A1_SRGB_BLOCK: return ImageFormat_ETC2_R8G8B8A1_SRGB_BLOCK;
		case TKTX_ETC2_R8G8B8A8_SRGB_BLOCK: return ImageFormat_ETC2_R8G8B8A8_SRGB_BLOCK;
		case TKTX_EAC_R11_UNORM_BLOCK: return ImageFormat_EAC_R11_UNORM_BLOCK;
		case TKTX_EAC_R11G11_UNORM_BLOCK: return ImageFormat_EAC_R11G11_UNORM_BLOCK;
		case TKTX_EAC_R11_SNORM_BLOCK: return ImageFormat_EAC_R11_SNORM_BLOCK;
		case TKTX_EAC_R11G11_SNORM_BLOCK: return ImageFormat_EAC_R11G11_SNORM_BLOCK;
		case TKTX_ASTC_4x4_UNORM_BLOCK: return ImageFormat_ASTC_4x4_UNORM_BLOCK;
		case TKTX_ASTC_4x4_SRGB_BLOCK: return ImageFormat_ASTC_4x4_SRGB_BLOCK;
		case TKTX_ASTC_5x4_UNORM_BLOCK: return ImageFormat_ASTC_5x4_UNORM_BLOCK;
		case TKTX_ASTC_5x4_SRGB_BLOCK: return ImageFormat_ASTC_5x4_SRGB_BLOCK;
		case TKTX_ASTC_5x5_UNORM_BLOCK: return ImageFormat_ASTC_5x5_UNORM_BLOCK;
		case TKTX_ASTC_5x5_SRGB_BLOCK: return ImageFormat_ASTC_5x5_SRGB_BLOCK;
		case TKTX_ASTC_6x5_UNORM_BLOCK: return ImageFormat_ASTC_6x5_UNORM_BLOCK;
		case TKTX_ASTC_6x5_SRGB_BLOCK: return ImageFormat_ASTC_6x5_SRGB_BLOCK;
		case TKTX_ASTC_6x6_UNORM_BLOCK: return ImageFormat_ASTC_6x6_UNORM_BLOCK;
		case TKTX_ASTC_6x6_SRGB_BLOCK: return ImageFormat_ASTC_6x6_SRGB_BLOCK;
		case TKTX_ASTC_8x5_UNORM_BLOCK: return ImageFormat_ASTC_8x5_UNORM_BLOCK;
		case TKTX_ASTC_8x5_SRGB_BLOCK: return ImageFormat_ASTC_8x5_SRGB_BLOCK;
		case TKTX_ASTC_8x6_UNORM_BLOCK: return ImageFormat_ASTC_8x6_UNORM_BLOCK;
		case TKTX_ASTC_8x6_SRGB_BLOCK: return ImageFormat_ASTC_8x6_SRGB_BLOCK;
		case TKTX_ASTC_8x8_UNORM_BLOCK: return ImageFormat_ASTC_8x8_UNORM_BLOCK;
		case TKTX_ASTC_8x8_SRGB_BLOCK: return ImageFormat_ASTC_8x8_SRGB_BLOCK;
		case TKTX_ASTC_10x5_UNORM_BLOCK: return ImageFormat_ASTC_10x5_UNORM_BLOCK;
		case TKTX_ASTC_10x5_SRGB_BLOCK: return ImageFormat_ASTC_10x5_SRGB_BLOCK;
		case TKTX_ASTC_10x6_UNORM_BLOCK: return ImageFormat_ASTC_10x6_UNORM_BLOCK;
		case TKTX_ASTC_10x6_SRGB_BLOCK: return ImageFormat_ASTC_10x6_SRGB_BLOCK;
		case TKTX_ASTC_10x8_UNORM_BLOCK: return ImageFormat_ASTC_10x8_UNORM_BLOCK;
		case TKTX_ASTC_10x8_SRGB_BLOCK: return ImageFormat_ASTC_10x8_SRGB_BLOCK;
		case TKTX_ASTC_10x10_UNORM_BLOCK: return ImageFormat_ASTC_10x10_UNORM_BLOCK;
		case TKTX_ASTC_10x10_SRGB_BLOCK: return ImageFormat_ASTC_10x10_SRGB_BLOCK;
		case TKTX_ASTC_12x10_UNORM_BLOCK: return ImageFormat_ASTC_12x10_UNORM_BLOCK;
		case TKTX_ASTC_12x10_SRGB_BLOCK: return ImageFormat_ASTC_12x10_SRGB_BLOCK;
		case TKTX_ASTC_12x12_UNORM_BLOCK: return ImageFormat_ASTC_12x12_UNORM_BLOCK;
		case TKTX_ASTC_12x12_SRGB_BLOCK: return ImageFormat_ASTC_12x12_SRGB_BLOCK;
		 */
	default: return ImageFormat_UNDEFINED;
	}
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LoadKTX(VFile_Handle handle) {
	TinyKtx_Callbacks callbacks {
			&tinyktxCallbackError,
			&tinyktxCallbackAlloc,
			&tinyktxCallbackFree,
			tinyktxCallbackRead,
			&tinyktxCallbackSeek,
			&tinyktxCallbackTell
	};

	auto ctx =  TinyKtx_CreateContext( &callbacks, handle);
	TinyKtx_ReadHeader(ctx);
	uint32_t w = TinyKtx_Width(ctx);
	uint32_t h = TinyKtx_Height(ctx);
	uint32_t d = TinyKtx_Depth(ctx);
	uint32_t s = TinyKtx_ArraySlices(ctx);
	ImageFormat fmt = ImageFormatToTinyKtxFormat(TinyKtx_GetFormat(ctx));
	if(fmt == ImageFormat_UNDEFINED) {
		TinyKtx_DestroyContext(ctx);
		return nullptr;
	}

	Image_ImageHeader const* topImage = nullptr;
	Image_ImageHeader const* prevImage = nullptr;
	for(auto i = 0u; i < TinyKtx_NumberOfMipmaps(ctx);++i) {
		auto image = Image_CreateNoClear(w, h, d, s, fmt);
		if(i == 0) topImage = image;

		if(Image_ByteCountOf(image) != TinyKtx_ImageSize(ctx, i)) {
			LOGERRORF("KTX file %s mipmap %i size error", VFile_GetName(handle), i);
			Image_Destroy(topImage);
			TinyKtx_DestroyContext(ctx);
			return nullptr;
		}
		memcpy(Image_RawDataPtr(image), TinyKtx_ImageRawData(ctx, i), Image_ByteCountOf(image));
		if(prevImage) {
			auto p = (Image_ImageHeader *)prevImage;
			p->nextType = Image_NextType::Image_IT_MipMaps;
			p->nextImage = image;
		}
		if(w > 1) w = w / 2;
		if(h > 1) h = h / 2;
		if(d > 1) d = d / 2;
		prevImage = image;
	}

	TinyKtx_DestroyContext(ctx);
	return topImage;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_Load(VFile_Handle handle) {
  VFile::File *file = VFile::File::FromHandle(handle);
  tinystl::string_view name = file->GetName();

  if( auto pos = name.rfind('.') ) {
  	if(pos == tinystl::string_view::npos ) return nullptr;

    tinystl::string_view ext(name.data()+pos+1, name.size()-pos-1 );
    switch(Utils::CompileTimeHash(ext) ) {
      case "dds"_hash:
        return Image_LoadDDS(handle);
      case "pvr"_hash:
        return Image_LoadPVR(handle);
      case "exr"_hash:
        return Image_LoadEXR(handle);
      case "hdr"_hash:
        return Image_LoadHDR(handle);
      case "jpg"_hash:
      case "jpeg"_hash:
      case "png"_hash:
      case "tga"_hash:
      case "bmp"_hash:
      case "psd"_hash:
      case "gif"_hash:
      case "pic"_hash:
      case "pnm"_hash:
        return Image_LoadLDR(handle);
    case "ktx"_hash:
    	return Image_LoadKTX(handle);
      default:
        return nullptr;
    }
  } else {
    return nullptr;
  }
}
