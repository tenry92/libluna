// disable MSVC warning about using fopen_s and strncpy_s
// (these are not available on GCC)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include <libgfx/libgfx.h>

#ifdef LIBGFX_BIG_ENDIAN
uint16_t swap_endian_16(uint16_t value) {
  return ((value << 8) & 0xff00) |
    ((value >> 8) & 0x00ff);
}

uint32_t swap_endian_32(uint32_t value) {
  return ((value << 24) & 0xff000000) |
    ((value << 8) & 0x00ff0000) |
    ((value >> 8) & 0x0000ff00) |
    ((value >> 24) & 0x000000ff);
}
#else
#define swap_endian_16(value) value
#define swap_endian_32(value) value
#endif

static const char *errorMessage = NULL;

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

static int read_u16(uint16_t *value, libgfx_ReadCallback read, void *userData) {
#ifdef LIBGFX_BIG_ENDIAN
  int result = read(value, sizeof(uint16_t), userData);
  *value = swap_endian_16(*value);
  return result;
#else
  return read(value, sizeof(uint16_t), userData);
#endif
}

static int read_u32(uint32_t *value, libgfx_ReadCallback read, void *userData) {
#ifdef LIBGFX_BIG_ENDIAN
  int result = read(value, sizeof(uint32_t), userData);
  *value = swap_endian_32(*value);
  return result;
#else
  return read(value, sizeof(uint32_t), userData);
#endif
}

static int write_u16(uint16_t *value, libgfx_WriteCallback write, void *userData) {
#ifdef LIBGFX_BIG_ENDIAN
  uint16_t swap = swap_endian_16(*value);
  return write(&swap, sizeof(uint16_t), userData);
#else
  return write(value, sizeof(uint16_t), userData);
#endif
}

static int write_u32(uint32_t *value, libgfx_WriteCallback write, void *userData) {
#ifdef LIBGFX_BIG_ENDIAN
  uint32_t swap = swap_endian_32(*value);
  return write(&swap, sizeof(uint32_t), userData);
#else
  return write(value, sizeof(uint32_t), userData);
#endif
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

static int getBitsPerPixel(libgfx_ColorFormat colorFormat) {
  int bitsPerPixel = 8;

  switch (colorFormat) {
    case LIBGFX_COLOR_4BIT_INDEXED:
    case LIBGFX_COLOR_4BIT_GRAYSCALE:
      bitsPerPixel = 4;
      break;
    case LIBGFX_COLOR_8BIT_INDEXED:
    case LIBGFX_COLOR_8BIT_GRAYSCALE:
      bitsPerPixel = 8;
      break;
    case LIBGFX_COLOR_16BIT_RGBA:
      bitsPerPixel = 16;
      break;
    case LIBGFX_COLOR_24BIT_RGB:
      bitsPerPixel = 24;
      break;
    case LIBGFX_COLOR_32BIT_RGBA:
      bitsPerPixel = 32;
      break;
  }

  return bitsPerPixel;
}

static int calcColorBytes(libgfx_ColorFormat format, int numColors) {
  return getBitsPerPixel(format) * numColors / 8;
}

libgfx_Gfx *libgfx_loadImageFromFile(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    errorMessage = "Couldn't open file";
    return NULL;
  }

  libgfx_Gfx *image = libgfx_loadImageFromCallback(readFileCallback, fp);
  fclose(fp);
  return image;
}

libgfx_Gfx *libgfx_loadImageFromMemory(const void *buffer, size_t bufferSize) {
  return libgfx_loadImageFromCallback(readMemCallback, (void *) &buffer);
}

libgfx_Gfx *libgfx_loadImageFromCallback(libgfx_ReadCallback read, void *userData) {
  char signature[5];
  if (read(signature, 5, userData) == 0) {
    errorMessage = "Error reading signature";
    return NULL;
  }

  if (strncmp("AGFX", signature, 5) != 0) {
    errorMessage = "Not a GFX file";
    return NULL;
  }

  uint16_t version;
  if (read_u16(&version, read, userData) == 0) {
    errorMessage = "Error reading version";
    return NULL;
  }

  if (version != 0x0001) {
    errorMessage = "Incompatible version";
    return NULL;
  }

  libgfx_Gfx *gfx = libgfx_allocGfx();

  read_u16(&gfx->numPalettes, read, userData);
  read_u16(&gfx->numFramesets, read, userData);
  read_u16(&gfx->numTiles, read, userData);
  read_u16(&gfx->numAnimations, read, userData);
  read_u16(&gfx->numTileAnimations, read, userData);
  read_u16(&gfx->numCharacters, read, userData);

  libgfx_allocPalettes(gfx, gfx->numPalettes);
  for (int i = 0; i < gfx->numPalettes; ++i) {
    libgfx_Palette *pal = &gfx->palettes[i];
    if (pal->numColors == 0) {
      pal->numColors = 256;
    }

    read(&pal->colorFormat, 1, userData);
    read(&pal->numColors, 1, userData);

    libgfx_allocPaletteColors(pal, pal->numColors);
    read(pal->colors, calcColorBytes(pal->colorFormat, pal->numColors), userData);
  }

  libgfx_allocFramesets(gfx, gfx->numFramesets);
  for (int i = 0; i < gfx->numFramesets; ++i) {
    libgfx_Frameset *frameset = &gfx->framesets[i];
    read(&frameset->colorFormat, 1, userData);
    read_u16(&frameset->width, read, userData);
    read_u16(&frameset->height, read, userData);
    read_u16(&frameset->numFrames, read, userData);

    int numPixels = frameset->width * frameset->height;
    libgfx_allocFrames(frameset, frameset->numFrames);
    read(frameset->data, frameset->numFrames * calcColorBytes(frameset->colorFormat, numPixels), userData);

#ifdef LIBGFX_BIG_ENDIAN
    if (frameset->colorFormat == LIBGFX_COLOR_16BIT_RGBA) {
      uint16_t *pixels = (uint16_t *) frameset->data;

      for (int offset = 0; offset < frameset->numFrames * calcColorBytes(frameset->colorFormat, numPixels) / 2; ++offset) {
        pixels[offset] = swap_endian_16(pixels[offset]);
        libgfx_16BitPixel pixel = libgfx_to16BitPixels(frameset->data)[offset];

        //                  hi            lo
        // input:           gggrrrrr abbbbbgg
        // input (swapped): abbbbbgg gggrrrrr
        // N64 format:      rrrrrggg ggbbabbb
        //                  h   lh    lh    l

        pixels[offset] = pixel.red << 11;
        pixels[offset] |= pixel.green << 6;
        pixels[offset] |= ((pixel.blue << 1) & 0x30) | (pixel.blue & 0x7);
        pixels[offset] |= 0x8; // alpha
      }
    }
#endif
  }

  libgfx_allocTiles(gfx, gfx->numTiles);
  for (int i = 0; i < gfx->numTiles; ++i) {
    read_u16(&gfx->tiles[i], read, userData);
  }

  libgfx_allocAnimations(gfx, gfx->numAnimations);
  for (int i = 0; i < gfx->numAnimations; ++i) {
    libgfx_Animation *anim = &gfx->animations[i];
    read_u16(&anim->numFrames, read, userData);

    libgfx_allocAnimation(anim, anim->numFrames);

    for (int j = 0; j < anim->numFrames; ++j) {
      read_u16(&anim->frames[j], read, userData);
    }
  }

  libgfx_allocTileAnimations(gfx, gfx->numTileAnimations);
  for (int i = 0; i < gfx->numTileAnimations; ++i) {
    libgfx_TileAnimation *tileAnim = &gfx->tileAnimations[i];
    read_u16(&tileAnim->tileIndex, read, userData);
    read_u16(&tileAnim->animationIndex, read, userData);
  }

  libgfx_allocCharacters(gfx, gfx->numCharacters);
  for (int i = 0; i < gfx->numCharacters; ++i) {
    libgfx_Character *character = &gfx->characters[i];
    read_u32(&character->codePoint, read, userData);
    read_u16(&character->frameIndex, read, userData);

    int16_t xOffset = (int16_t) character->xOffset;
    int16_t yOffset = (int16_t) character->yOffset;
    int16_t advance = (int16_t) character->advance;

    read_u16(&xOffset, read, userData);
    read_u16(&yOffset, read, userData);
    read_u16(&advance, read, userData);
  }

  return gfx;
}

int libgfx_writeImageToCallback(libgfx_Gfx *gfx, libgfx_WriteCallback write, void *userData) {
  // signature
  write("AGFX", 5, userData);

  // version
  uint16_t version = 0x0001;
  write_u16(&version, write, userData);

  write_u16((uint16_t *) &gfx->numPalettes, write, userData);
  write_u16((uint16_t *) &gfx->numFramesets, write, userData);
  write_u16((uint16_t *) &gfx->numTiles, write, userData);
  write_u16((uint16_t *) &gfx->numAnimations, write, userData);
  write_u16((uint16_t *) &gfx->numTileAnimations, write, userData);
  write_u16((uint16_t *) &gfx->numCharacters, write, userData);

  for (int i = 0; i < gfx->numPalettes; ++i) {
    libgfx_Palette *pal = &gfx->palettes[i];
    write(&pal->colorFormat, 1, userData);

    if (pal->numColors < 256) {
      write(&pal->numColors, 1, userData);
    } else {
      uint8_t numColors = 0;
      write(&numColors, 1, userData);
      write(pal->colors, calcColorBytes(pal->colorFormat, numColors), userData);
    }
  }

  for (int i = 0; i < gfx->numFramesets; ++i) {
    libgfx_Frameset *frameset = &gfx->framesets[i];
    write(&frameset->colorFormat, 1, userData);
    write_u16(&frameset->width, write, userData);
    write_u16(&frameset->height, write, userData);
    write_u16(&frameset->numFrames, write, userData);

    int numPixels = frameset->width * frameset->height;
    write(frameset->data, frameset->numFrames * calcColorBytes(frameset->colorFormat, numPixels), userData);
  }

  for (int i = 0; i < gfx->numTiles; ++i) {
    write_u16(&gfx->tiles[i], write, userData);
  }

  for (int i = 0; i < gfx->numAnimations; ++i) {
    libgfx_Animation *anim = &gfx->animations[i];
    write_u16(&anim->numFrames, write, userData);

    for (int j = 0; j < anim->numFrames; ++j) {
      write_u16(&anim->frames[j], write, userData);
    }
  }

  for (int i = 0; i < gfx->numTileAnimations; ++i) {
    libgfx_TileAnimation *tileAnim = &gfx->tileAnimations[i];
    write_u16(&tileAnim->tileIndex, write, userData);
    write_u16(&tileAnim->animationIndex, write, userData);
  }

  for (int i = 0; i < gfx->numCharacters; ++i) {
    libgfx_Character *character = &gfx->characters[i];
    write_u32(&character->codePoint, write, userData);
    write_u16(&character->frameIndex, write, userData);

    int16_t xOffset = (int16_t) character->xOffset;
    int16_t yOffset = (int16_t) character->yOffset;
    int16_t advance = (int16_t) character->advance;

    write_u16(&xOffset, write, userData);
    write_u16(&yOffset, write, userData);
    write_u16(&advance, write, userData);
  }

  return 0;
}

int libgfx_writeImageToFile(libgfx_Gfx *image, const char *filename) {
  FILE *fp = fopen(filename, "wb");

  if (!fp) {
    return 1;
  }

  int result = libgfx_writeImageToCallback(image, writeFileCallback, fp);
  fclose(fp);

  return result;
}

int libgfx_writeImageToMemory(libgfx_Gfx *image, void *buffer, size_t bufferSize) {
  return libgfx_writeImageToCallback(image, writeMemCallback, &buffer);
}

libgfx_Gfx *libgfx_allocGfx() {
  libgfx_Gfx *gfx = calloc(1, sizeof(libgfx_Gfx));

  return gfx;
}

void libgfx_freeGfx(libgfx_Gfx *gfx) {
  if (gfx->palettes) {
    for (int i = 0; i < gfx->numPalettes; ++i) {
      if (gfx->palettes[i].colors) {
        free(gfx->palettes[i].colors);
      }
    }
    free(gfx->palettes);
  }

  if (gfx->framesets) {
    for (int i = 0; i < gfx->numFramesets; ++i) {
      if (gfx->framesets[i].data) {
        free(gfx->framesets[i].data);
      }
    }

    free(gfx->framesets);
  }

  if (gfx->tiles) {
    free(gfx->tiles);
  }

  if (gfx->animations) {
    for (int i = 0; i < gfx->numAnimations; ++i) {
      if (gfx->animations[i].frames) {
        free(gfx->animations[i].frames);
      }
    }

    free(gfx->animations);
  }

  if (gfx->tileAnimations) {
    free(gfx->tileAnimations);
  }

  if (gfx->characters) {
    free(gfx->characters);
  }

  free(gfx);
}

int libgfx_allocPalettes(libgfx_Gfx *gfx, int numPalettes) {
  gfx->numPalettes = numPalettes;
  gfx->palettes = calloc(numPalettes, sizeof(libgfx_Palette));

  return 0;
}

int libgfx_allocPaletteColors(libgfx_Palette *pal, int numColors) {
  pal->numColors = numColors;
  pal->colors = calloc(1, calcColorBytes(pal->colorFormat, numColors));

  return 0;
}

int libgfx_allocFramesets(libgfx_Gfx *gfx, int numFramesets) {
  gfx->numFramesets = numFramesets;
  gfx->framesets = calloc(numFramesets, sizeof(libgfx_Frameset));

  return 0;
}

int libgfx_allocFrames(libgfx_Frameset *frameset, int numFrames) {
  frameset->numFrames = numFrames;
  int numPixels = frameset->width * frameset->height;
  frameset->data = calloc(numFrames, calcColorBytes(frameset->colorFormat, numPixels));

  return 0;
}

int libgfx_allocTiles(libgfx_Gfx *gfx, int numTiles) {
  gfx->numTiles = numTiles;
  gfx->tiles = calloc(numTiles, sizeof(int));

  return 0;
}

int libgfx_allocAnimations(libgfx_Gfx *gfx, int numAnimations) {
  gfx->numAnimations = numAnimations;
  gfx->animations = calloc(numAnimations, sizeof(libgfx_Animation));

  return 0;
}

int libgfx_allocAnimation(libgfx_Animation *anim, int numFrames) {
  anim->numFrames = numFrames;
  anim->frames = calloc(numFrames, sizeof(int));

  return 0;
}

int libgfx_allocTileAnimations(libgfx_Gfx *gfx, int numTileAnimations) {
  gfx->numTileAnimations = numTileAnimations;
  gfx->tileAnimations = calloc(numTileAnimations, sizeof(libgfx_TileAnimation));

  return 0;
}

int libgfx_allocCharacters(libgfx_Gfx *gfx, int numCharacters) {
  gfx->numCharacters = numCharacters;
  gfx->characters = calloc(numCharacters, sizeof(libgfx_Character));

  return 0;
}

void *libgfx_getFramePointer(libgfx_Frameset *frameset, int frameIndex) {
  char *ptr = (char *) frameset->data;
  int numPixels = frameset->width * frameset->height;
  int bytesPerFrame = calcColorBytes(frameset->colorFormat, numPixels);

  return (void *) ptr + frameIndex * bytesPerFrame;
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

int libgfx_bigEndian() {
#ifdef LIBGFX_BIG_ENDIAN
  return 1;
#else
  return 0;
#endif
}
