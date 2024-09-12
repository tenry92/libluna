#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef N64
#define LIBGFX_BIG_ENDIAN
#endif

#include <stdint.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
#ifdef LIBGFX_BIG_ENDIAN
  uint8_t second : 4;
  uint8_t first : 4;
#else
  uint8_t first : 4;
  uint8_t second : 4;
#endif
} libgfx_4BitPixel;

typedef struct {
#ifdef LIBGFX_BIG_ENDIAN
  uint8_t alpha : 1;
  uint8_t blue : 5;
  uint8_t green : 5;
  uint8_t red : 5;
#else
  uint8_t red : 5;
  uint8_t green : 5;
  uint8_t blue : 5;
  uint8_t alpha : 1;
#endif
} libgfx_16BitPixel;

typedef struct {
#ifdef LIBGFX_BIG_ENDIAN
  uint8_t blue;
  uint8_t green;
  uint8_t red;
#else
  uint8_t red;
  uint8_t green;
  uint8_t blue;
#endif
} libgfx_24BitPixel;

typedef struct {
#ifdef LIBGFX_BIG_ENDIAN
  uint8_t alpha;
  uint8_t blue;
  uint8_t green;
  uint8_t red;
#else
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
#endif
} libgfx_32BitPixel;
#pragma pack(pop)

typedef enum {
  LIBGFX_COLOR_4BIT_INDEXED = 0,
  LIBGFX_COLOR_8BIT_INDEXED = 1,
  LIBGFX_COLOR_4BIT_GRAYSCALE = 2,
  LIBGFX_COLOR_8BIT_GRAYSCALE = 3,
  LIBGFX_COLOR_16BIT_RGBA = 4,
  LIBGFX_COLOR_24BIT_RGB = 5,
  LIBGFX_COLOR_32BIT_RGBA = 6,
} libgfx_ColorFormat;

typedef struct {
  libgfx_ColorFormat colorFormat;
  int numColors;
  void *colors;
} libgfx_Palette;

/**
 * @brief A series of individual frames that share the same dimensions.
 */
typedef struct {
  libgfx_ColorFormat colorFormat;
  uint16_t width;
  uint16_t height;
  uint16_t numFrames;
  void *data;
} libgfx_Frameset;

typedef struct {
  uint16_t numFrames;
  uint16_t *frames;
} libgfx_Animation;

typedef struct {
  uint16_t tileIndex;
  uint16_t animationIndex;
} libgfx_TileAnimation;

typedef struct {
  uint16_t baseLine;
  uint16_t lineHeight;
} libgfx_Font;

typedef struct {
  uint32_t codePoint;
  uint16_t frameIndex;
  int16_t xOffset;
  int16_t yOffset;
  int16_t advance;
} libgfx_Character;

typedef struct {
  uint16_t numPalettes;
  libgfx_Palette *palettes;

  uint16_t numFramesets;
  libgfx_Frameset *framesets;

  uint16_t numTiles;
  uint16_t *tiles;

  uint16_t numAnimations;
  libgfx_Animation *animations;

  uint16_t numTileAnimations;
  libgfx_TileAnimation *tileAnimations;

  libgfx_Font font;

  uint16_t numCharacters;
  libgfx_Character *characters;
} libgfx_Gfx;

typedef int (*libgfx_ReadCallback)(void *dest, size_t size, void *userData);

typedef int (*libgfx_WriteCallback)(
    const void *src, size_t size, void *userData
);

libgfx_Gfx *libgfx_loadImageFromFile(const char *filename);

libgfx_Gfx *libgfx_loadImageFromMemory(const void *buffer, size_t bufferSize);

libgfx_Gfx *
libgfx_loadImageFromCallback(libgfx_ReadCallback read, void *userData);

int libgfx_writeImageToFile(libgfx_Gfx *gfx, const char *filename);

int libgfx_writeImageToMemory(libgfx_Gfx *gfx, void *buffer, size_t bufferSize);

int libgfx_writeImageToCallback(
    libgfx_Gfx *gfx, libgfx_WriteCallback write, void *userData
);

/**
 * @brief Load an image from the given filename.
 */
libgfx_Gfx *libgfx_loadImageFromFile(const char *filename);

/**
 * @brief Load an image from memory using the given buffer.
 */
libgfx_Gfx *libgfx_loadImageFromMemory(const void *buffer, size_t bufferSize);

/**
 * @brief Load an image using provided callback.
 */
libgfx_Gfx *
libgfx_loadImageFromCallback(libgfx_ReadCallback read, void *userData);

/**
 * @brief Write the image to the given file.
 */
int libgfx_writeImageToFile(libgfx_Gfx *gfx, const char *filename);

/**
 * @brief Write the image into memory.
 */
int libgfx_writeImageToMemory(libgfx_Gfx *gfx, void *buffer, size_t bufferSize);

/**
 * @brief Write the image using the provided callback.
 */
int libgfx_writeImageToCallback(
    libgfx_Gfx *gfx, libgfx_WriteCallback, void *userData
);

libgfx_Gfx *libgfx_allocGfx();

void libgfx_freeGfx(libgfx_Gfx *gfx);

int libgfx_allocPalettes(libgfx_Gfx *gfx, int numPalettes);

int libgfx_allocPaletteColors(libgfx_Palette *pal, int numColors);

int libgfx_allocFramesets(libgfx_Gfx *gfx, int numFramesets);

int libgfx_allocFrames(libgfx_Frameset *frameset, int numFrames);

int libgfx_allocTiles(libgfx_Gfx *gfx, int numTiles);

int libgfx_allocAnimations(libgfx_Gfx *gfx, int numAnimations);

int libgfx_allocAnimation(libgfx_Animation *anim, int numFrames);

int libgfx_allocTileAnimations(libgfx_Gfx *gfx, int numTileAnimations);

int libgfx_allocCharacters(libgfx_Gfx *gfx, int numCharacters);

void *libgfx_getFramePointer(libgfx_Frameset *frameset, int frameIndex);

libgfx_4BitPixel *libgfx_to4BitPixels(void *pixels);

libgfx_16BitPixel *libgfx_to16BitPixels(void *pixels);

libgfx_24BitPixel *libgfx_to24BitPixels(void *pixels);

libgfx_32BitPixel *libgfx_to32BitPixels(void *pixels);

const char *libgfx_getError();

int libgfx_bigEndian();

#ifdef __cplusplus
}
#endif
