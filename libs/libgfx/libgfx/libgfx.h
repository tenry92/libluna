#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef N64
#define LIBGFX_BIG_ENDIAN
#endif

#include <stdint.h>
#include <stdlib.h>

typedef enum {
  /**
   * @brief Image is encoded in 4 bits per pixel (16 colors).
   *
   * Each byte represents 2 color indices.
   */
  LIBGFX_FORMAT_4BIT_INDEXED = 0,

  /**
   * @brief Image is encoded in 8 bits per pixel (256 colors).
   *
   * Each byte represents 1 color index.
   */
  LIBGFX_FORMAT_8BIT_INDEXED = 1,

  /**
   * @brief Image is encoded in 4 bits per pixel (16 levels).
   *
   * Each byte represents 2 values.
   */
  LIBGFX_FORMAT_4BIT_GRAYSCALE = 2,
  
  /**
   * @brief Image is encoded in 8 bits per pixel (256 levels).
   *
   * Each byte represents 1 value.
   */
  LIBGFX_FORMAT_8BIT_GRAYSCALE = 3,

  /**
   * @brief Image is encoded in 16 bits per pixel.
   *
   * Red, green and blue are encoded in 5 bits each (32 levels) and alpha is 1 bit.
   */
  LIBGFX_FORMAT_16BIT_RGBA = 4,

  /**
   * @brief Image is encoded in 24 bits per pixel.
   *
   * Red, green and blue are encoded in 8 bits each.
   */
  LIBGFX_FORMAT_24BIT_RGB = 5,

  /**
   * @brief Image is encoded in 24 bits per pixel.
   *
   * Red, green, blue and alpha are encoded in 8 bits each.
   */
  LIBGFX_FORMAT_32BIT_RGBA = 6,
} libgfx_ColorFormat;

typedef enum {
  /**
   * @brief Useful if the GFX file only contains palettes.
   */
  LIBGFX_TYPE_NONE = 0,
  LIBGFX_TYPE_IMAGE = 1,
  LIBGFX_TYPE_TILESET = 2,
} libgfx_ImageType;

typedef struct {
  /**
   * @brief Tile index to animate. 0 refers to the first defined tile.
   */
  int tileIndex;
  int numFrames;
  /**
   * @brief List of frame or tile indices to use. 0 refers to the first defined frame.
  */
  int *frameIndices;
} libgfx_Animation;

typedef struct {
  libgfx_ColorFormat colorFormat;
  int numColors;
  void *colors;
} libgfx_Palette;

typedef struct {
  /**
   * @brief Width of a single frame or tile in pixels.
   */
  int width;

  /**
   * @brief Height of a single frame or tile in pixels.
   */
  int height;

  /**
   * @brief Color format used.
   */
  libgfx_ColorFormat colorFormat;

  /**
   * @brief Type of image.
   */
  libgfx_ImageType type;

  /**
   * @brief Number of frames.
   */
  int numFrames;

  /**
   * @brief Number of frames that are used as the base tiles.
   *
   * Any frames above this numbers are meant as animation frames.
   */
  int numTiles;

  /**
   * @brief Number of animations.
   *
   * For image, animations will refer to the frames already defined.
   *
   * For tileset, for each animation, a tile index is specified and additional
   * frame data is provided.
   */
  int numAnimations;

  /**
   * @brief Number of palettes included.
   */
  int numPalettes;

  /**
   * @brief List of pointers for frame or tile data.
   *
   * The size of each frame in bytes depends on the width, height and format of
   * the image.
   */
  void *frames;

  libgfx_Animation *animations;

  libgfx_Palette *palettes;
} libgfx_Image;

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

typedef int (*libgfx_ReadCallback)(void *dest, size_t size, void *userData);

typedef int (*libgfx_WriteCallback)(const void *src, size_t size, void *userData);

/**
 * @brief Load an image from the given filename.
 */
libgfx_Image *libgfx_loadImageFromFile(const char *filename);

/**
 * @brief Load an image from memory using the given buffer.
 */
libgfx_Image *libgfx_loadImageFromMemory(const void *buffer, size_t bufferSize);

/**
 * @brief Load an image using provided callback.
 */
libgfx_Image *libgfx_loadImageFromCallback(libgfx_ReadCallback read, void *userData);

/**
 * @brief Write the image to the given file.
 */
int libgfx_writeImageToFile(libgfx_Image *image, const char *filename);

/**
 * @brief Write the image into memory.
 */
int libgfx_writeImageToMemory(libgfx_Image *image, void *buffer, size_t bufferSize);

/**
 * @brief Write the image using the provided callback.
 */
int libgfx_writeImageToCallback(libgfx_Image *image, libgfx_WriteCallback, void *userData);

/**
 * @brief Convert an indexed image to an RGB/RGBA image using the provided palette.
 */
int libgfx_applyPalette(libgfx_Image *image, libgfx_ColorFormat colorFormat, const void *colors);

/**
 * @brief Convert the image format.
 * 
 * Conversion is only allowed between indexed or non-indexed formats.
 * Conversion from or to indexed and non-indexed is not allowed.
 *
 * @see libgfx_applyPalette() for converting an indexed image to an RGB/RGBA image.
 */
int libgfx_convertFormat(libgfx_Image *image, libgfx_ColorFormat colorFormat);

/**
 * @brief Create a new empty image using the given parameters.
 */
libgfx_Image *libgfx_createImage(int width, int height, libgfx_ColorFormat colorFormat, libgfx_ImageType type);

/**
 * @brief Allocate frames or tiles for the given image.
 *
 * This operation is only allowed once per image to initialize the frame data.
 */
int libgfx_allocFrames(libgfx_Image *image, int numFrames);

/**
 * @brief Allocate animations for the given image.
 *
 * This operation is only allowed once per image to initialize the animation data.
 */
int libgfx_allocAnimations(libgfx_Image *image, int numAnimations);

/**
 * @brief Allocate palettes for the given image.
 *
 * This operation is only allowed once per image to initialize the palette data.
 */
int libgfx_allocPalettes(libgfx_Image *image, int numPalettes);

/**
 * @brief Allocate animation.
 *
 * This operation is only allowed once per animation.
 */
int libgfx_allocAnimation(libgfx_Image *image, libgfx_Animation *animation, int numFrames);

/**
 * @brief Allocate palette.
 *
 * This operation is only allowed once per palette.
 */
int libgfx_allocPalette(libgfx_Image *image, libgfx_Palette *palette, libgfx_ColorFormat colorFormat, int numColors);

/**
 * @brief Get pointer to the given frame index.
 */
void *libgfx_getFramePointer(libgfx_Image *image, int frameIndex);

libgfx_Animation *libgfx_getAnimation(libgfx_Image *image, int index);

libgfx_Palette *libgfx_getPalette(libgfx_Image *image, int index);

/**
 * @brief Free memory that was allocated for this image.
 *
 * This also frees all memories allocated for frames, animations, palettes etc.
 */
void libgfx_freeImage(libgfx_Image *image);

libgfx_4BitPixel *libgfx_to4BitPixels(void *pixels);

libgfx_16BitPixel *libgfx_to16BitPixels(void *pixels);

libgfx_24BitPixel *libgfx_to24BitPixels(void *pixels);

libgfx_32BitPixel *libgfx_to32BitPixels(void *pixels);

/**
 * @brief Get a pointer to the latest error message string or NULL if there is none.
 */
const char *libgfx_getError();

#ifdef __cplusplus
}
#endif
