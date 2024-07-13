#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpng16/png.h>

#include "loadpng.h"

Surface *loadPng(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s for reading.\n", filename);
    return NULL;
  }

  png_structp png = png_create_read_struct(
    PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
  );

  if (!png) {
    fclose(fp);
    fprintf(stderr, "Error creating png struct.\n");
    return NULL;
  }

  png_infop info = png_create_info_struct(png);

  if (!info) {
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    fprintf(stderr, "Error creating png info struct.\n");
    return NULL;
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int inputWidth = png_get_image_width(png, info);
  int inputHeight = png_get_image_height(png, info);
  png_byte inputColorType = png_get_color_type(png, info);
  png_byte inputBitDepth = png_get_bit_depth(png, info);

  if (inputBitDepth == 16) {
    png_set_strip_16(png);
    inputBitDepth = 8;
  }

  if (inputColorType == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }

  if (inputColorType == PNG_COLOR_TYPE_GRAY) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }

  if (inputColorType == PNG_COLOR_TYPE_GRAY ||
    inputColorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  if (inputColorType == PNG_COLOR_TYPE_RGB ||
    inputColorType == PNG_COLOR_TYPE_GRAY ||
    inputColorType == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  png_read_update_info(png, info);

  png_bytep *inputRowPointers = (png_bytep *) malloc(inputHeight * sizeof(png_bytep));
  for (int row = 0; row < inputHeight; ++row) {
    inputRowPointers[row] = (png_byte *) malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, inputRowPointers);

  Surface *surface = allocSurface(inputWidth, inputHeight);
  int rowSize = inputWidth * 4;

  for (int row = 0; row < inputHeight; ++row) {
    memcpy(surface->data + row * rowSize, inputRowPointers[row], rowSize);
  }

  png_destroy_read_struct(&png, NULL, NULL);

  fclose(fp);

  return surface;
}
