// disable MSVC warning about using fopen_s and strncpy_s
// (these are not available on GCC)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include <libgfx/libgfx.h>

static const char *errorMessage = NULL;

#pragma pack(push, 1)
typedef struct {
  char signature[4];
  uint16_t version;
  uint16_t size; // >= sizeof(libgfx_FileHeader)
  uint16_t width;
  uint16_t height;
  uint8_t format : 4;
  uint8_t type : 4;
  uint16_t numFrames;
  uint16_t numTiles;
  uint16_t numAnimations; // for tileset, number of animated tiles
  uint16_t numPalettes; // can be 0 if no palette is included
} libgfx_FileHeader;
#pragma pack(pop)

static int getBitsPerPixel(libgfx_ColorFormat colorFormat) {
  int bitsPerPixel = 8;

  switch (colorFormat) {
    case LIBGFX_FORMAT_4BIT_INDEXED:
    case LIBGFX_FORMAT_4BIT_GRAYSCALE:
      bitsPerPixel = 4;
      break;
    case LIBGFX_FORMAT_8BIT_INDEXED:
    case LIBGFX_FORMAT_8BIT_GRAYSCALE:
      bitsPerPixel = 8;
      break;
    case LIBGFX_FORMAT_16BIT_RGBA:
      bitsPerPixel = 16;
      break;
    case LIBGFX_FORMAT_24BIT_RGB:
      bitsPerPixel = 24;
      break;
    case LIBGFX_FORMAT_32BIT_RGBA:
      bitsPerPixel = 32;
      break;
  }

  return bitsPerPixel;
}

static inline float getBytesPerPixel(libgfx_ColorFormat colorFormat) {
  return ((float) getBitsPerPixel(colorFormat)) / 8;
}

static int getBytesPerFrame(libgfx_Image *image) {
  return (int) (image->width * image->height * getBytesPerPixel(image->colorFormat));
}

static int readFileCallback(void *dest, size_t size, void *userData) {
  FILE *file = (FILE *) userData;
  return (int) (fread(dest, size, 1, file) * size);
}

static int readMemCallback(void *dest, size_t size, void *userData) {
  const uint8_t **srcPointer = (const uint8_t **) userData;
  
  const uint8_t *src = *srcPointer;
  memcpy(dest, src, size);

  srcPointer += size;

  return (int) size;
}

static int writeFileCallback(const void *src, size_t size, void *userData) {
  FILE *file = (FILE *) userData;
  return (int) (fwrite(src, size, 1, file) * size);
}

static int writeMemCallback(const void *src, size_t size, void *userData) {
  uint8_t **destPointer = (uint8_t **) userData;
  
  uint8_t *dest = *destPointer;
  memcpy(dest, src, size);

  destPointer += size;

  return (int) size;
}

libgfx_Image *libgfx_loadImageFromFile(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    errorMessage = "Couldn't open file";
    return NULL;
  }

  libgfx_Image *image = libgfx_loadImageFromCallback(readFileCallback, fp);
  fclose(fp);
  return image;
}

libgfx_Image *libgfx_loadImageFromMemory(const void *buffer, size_t bufferSize) {
  return libgfx_loadImageFromCallback(readMemCallback, (void *) &buffer);
}

libgfx_Image *libgfx_loadImageFromCallback(libgfx_ReadCallback read, void *userData) {
  libgfx_FileHeader header;
  libgfx_Image *image = NULL;

  if (read(&header, sizeof(libgfx_FileHeader), userData) == 0) {
    errorMessage = "Couldn't read GFX header";
    return NULL;
  }

  if (strncmp("AGFX", header.signature, 4) != 0) {
    errorMessage = "Not a GFX file (invalid signature)";
    return NULL;
  }

  if (header.version != 1) {
    errorMessage = "Unsupported version";
    return NULL;
  }

  if (header.size != sizeof(header)) {
    errorMessage = "Incompatible header size";
    return NULL;
  }

  if (header.format > LIBGFX_FORMAT_32BIT_RGBA) {
    errorMessage = "Invalid color format";
    return NULL;
  }

  if (header.type > LIBGFX_TYPE_TILESET) {
    errorMessage = "Invalid type";
    return NULL;
  }

  if (header.type == LIBGFX_TYPE_NONE) {
    if (header.numFrames > 0 || header.numAnimations > 0) {
      errorMessage = "Frames or animations not allowed for palette only";
      return NULL;
    }
  } else {
    if (header.numFrames == 0) {
      errorMessage = "No frames included";
      return NULL;
    }
  }

  image = libgfx_createImage(header.width, header.height, header.format, header.type);

  libgfx_allocFrames(image, header.numFrames);
  if (read(image->frames, image->numFrames * getBytesPerFrame(image), userData) == 0) {
    libgfx_freeImage(image);
    errorMessage = "Error reading frames";
    return NULL;
  }

  libgfx_allocAnimations(image, header.numAnimations);

  for (int i = 0; i < image->numAnimations; ++i) {
    libgfx_Animation *anim = libgfx_getAnimation(image, i);

    uint16_t tileIndex;
    read(&tileIndex, sizeof(tileIndex), userData);

    uint8_t numFrames;
    read(&numFrames, sizeof(numFrames), userData);

    libgfx_allocAnimation(image, anim, numFrames);

    anim->tileIndex = tileIndex;

    for (int f = 0; f < numFrames; ++f) {
      uint16_t frameIndex;
      if (read(&frameIndex, sizeof(frameIndex), userData) == 0) {
        libgfx_freeImage(image);
        errorMessage = "Error reading animation";
        return NULL;
      }
      anim->frameIndices[i] = f;
    }
  }

  libgfx_allocPalettes(image, header.numPalettes);

  for (int i = 0; i < header.numPalettes; ++i) {
    libgfx_Palette *pal = libgfx_getPalette(image, i);

    uint8_t format;
    uint8_t numColors;
    if (read(&format, sizeof(format), userData) == 0) {
      libgfx_freeImage(image);
      errorMessage = "Error reading palette";
      return NULL;
    }
    if (read(&numColors, sizeof(numColors), userData) == 0) {
      libgfx_freeImage(image);
      errorMessage = "Error reading palette";
      return NULL;
    }

    pal->numColors = (numColors > 0 ? numColors : 256);

    libgfx_allocPalette(image, pal, format, pal->numColors);
    if (read(pal->colors, (size_t) (pal->numColors * getBytesPerPixel(pal->colorFormat)), userData) == 0) {
      libgfx_freeImage(image);
      errorMessage = "Error reading palette";
      return NULL;
    }
  }

  return image;
}

int libgfx_writeImageToFile(libgfx_Image *image, const char *filename) {
  FILE *fp = fopen(filename, "wb");
  
  if (!fp) {
    return 1;
  }

  int result = libgfx_writeImageToCallback(image, writeFileCallback, fp);
  fclose(fp);

  return result;
}

int libgfx_writeImageToMemory(libgfx_Image *image, void *buffer, size_t bufferSize) {
  return libgfx_writeImageToCallback(image, writeMemCallback, &buffer);
}

int libgfx_writeImageToCallback(libgfx_Image *image, libgfx_WriteCallback write, void *userData) {
  libgfx_FileHeader header;

  strncpy(header.signature, "AGFX", 4);
  header.version = 1;
  header.size = sizeof(header);
  header.width = image->width;
  header.height = image->height;
  header.format = image->colorFormat;
  header.type = image->type;
  header.numFrames = image->numFrames;
  header.numTiles = image->numTiles || image->numFrames;
  header.numAnimations = image->numAnimations;
  header.numPalettes = image->numPalettes;
  write(&header, sizeof(header), userData);

  int bytesPerFrame = getBytesPerFrame(image);
  write(image->frames, image->numFrames * bytesPerFrame, userData);

  for (int i = 0; i < image->numAnimations; ++i) {
    libgfx_Animation *anim = libgfx_getAnimation(image, i);
    uint16_t tileIndex = anim->tileIndex;
    uint8_t numFrames = anim->numFrames;
    write(&tileIndex, sizeof(tileIndex), userData);
    write(&numFrames, sizeof(numFrames), userData);

    for (int f = 0; f < anim->numFrames; ++f) {
      uint16_t frameIndex = anim->frameIndices[f];
      write(&frameIndex, sizeof(frameIndex), userData);
    }
  }

  for (int i = 0; i < image->numPalettes; ++i) {
    libgfx_Palette *pal = libgfx_getPalette(image, i);
    uint8_t format = pal->colorFormat;
    uint8_t numColors = pal->numColors;
    write(&format, sizeof(format), userData);
    write(&numColors, sizeof(numColors), userData);
    write(pal->colors, (size_t) (pal->numColors * getBytesPerPixel(pal->colorFormat)), userData);
  }
  
  return 0;
}

libgfx_Image *libgfx_createImage(int width, int height, libgfx_ColorFormat colorFormat, libgfx_ImageType type) {
  libgfx_Image *image = malloc(sizeof(libgfx_Image));

  image->width = width;
  image->height = height;
  image->colorFormat = colorFormat;
  image->type = type;
  image->numFrames = 0;
  image->numTiles = 0;
  image->numAnimations = 0;
  image->numPalettes = 0;
  image->frames = NULL;
  image->animations = NULL;
  image->palettes = NULL;

  return image;
}

int libgfx_allocFrames(libgfx_Image *image, int numFrames) {
  int bytesPerFrame = getBytesPerFrame(image);
  image->numFrames = numFrames;
  image->frames = calloc(numFrames, bytesPerFrame);

  return 0;
}

int libgfx_allocAnimations(libgfx_Image *image, int numAnimations) {
  image->numAnimations = numAnimations;
  image->animations = calloc(numAnimations, sizeof(libgfx_Animation));

  return 0;
}

int libgfx_allocPalettes(libgfx_Image *image, int numPalettes) {
  image->numPalettes = numPalettes;
  image->palettes = calloc(numPalettes, sizeof(libgfx_Palette));

  return 0;
}

int libgfx_allocAnimation(libgfx_Image *image, libgfx_Animation *animation, int numFrames) {
  animation->tileIndex = 0;
  animation->numFrames = numFrames;
  animation->frameIndices = calloc(sizeof(int), numFrames);

  return 0;
}

int libgfx_allocPalette(libgfx_Image *image, libgfx_Palette *palette, libgfx_ColorFormat colorFormat, int numColors) {
  int bytesPerColor = (int) getBytesPerPixel(colorFormat);
  
  palette->colorFormat = colorFormat;
  palette->numColors = numColors;
  palette->colors = calloc(numColors, bytesPerColor);

  return 0;
}

void *libgfx_getFramePointer(libgfx_Image *image, int frameIndex) {
  return (void *) (((char *) image->frames) + getBytesPerFrame(image) * frameIndex);
}

libgfx_Animation *libgfx_getAnimation(libgfx_Image *image, int index) {
  return image->animations + index * sizeof(libgfx_Animation);
}

libgfx_Palette *libgfx_getPalette(libgfx_Image *image, int index) {
  return image->palettes + index * sizeof(libgfx_Palette);
}

void libgfx_freeImage(libgfx_Image *image) {
  if (image->frames) {
    free(image->frames);
  }

  if (image->animations) {
    for (int i = 0; i < image->numAnimations; ++i) {
      libgfx_Animation *anim = libgfx_getAnimation(image, i);
      free(anim->frameIndices);
    }

    free(image->animations);
  }

  if (image->palettes) {
    for (int i = 0; i < image->numPalettes; ++i) {
      libgfx_Palette *pal = libgfx_getPalette(image, i);
      free(pal->colors);
    }

    free(image->palettes);
  }

  free(image);
}

libgfx_4BitPixel *libgfx_to4BitPixels(void *pixels) {
  return (libgfx_4BitPixel *) pixels;
}

libgfx_16BitPixel *libgfx_to16BitPixels(void *pixels) {
  return (libgfx_16BitPixel *) pixels;
}

libgfx_24BitPixel *libgfx_to24BitPixels(void *pixels) {
  return (libgfx_24BitPixel *) pixels;
}

libgfx_32BitPixel *libgfx_to32BitPixels(void *pixels) {
  return (libgfx_32BitPixel *) pixels;
}

const char *libgfx_getError() {
  return errorMessage;
}
