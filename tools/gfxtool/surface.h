#pragma once

#include <stdint.h>

typedef struct {
  int width;
  int height;
  uint8_t *data;
} Surface;

#pragma pack(push, 1)
typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
} ColorRgba32;
#pragma pack(pop)

Surface *allocSurface(int width, int height);

void freeSurface(Surface *surface);

ColorRgba32 *getSurfacePixel(Surface *surface, int x, int y);

Surface *cropSurface(Surface *surface, int left, int top, int width, int height);
