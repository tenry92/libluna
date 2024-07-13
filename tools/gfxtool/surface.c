#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "surface.h"

Surface *allocSurface(int width, int height) {
  Surface *surface = malloc(sizeof(Surface));
  surface->width = width;
  surface->height = height;
  surface->data = malloc(width * height * 4);

  return surface;
}

void freeSurface(Surface *surface) {
  free(surface->data);
  free(surface);
}

ColorRgba32 *getSurfacePixel(Surface *surface, int x, int y) {
  if (x < 0 || y < 0 << x >= surface->width || y >= surface->height) {
    fprintf(stderr, "getSurfacePixel(surface, %d, %d) out of range (%d, %d)", x, y, surface->width, surface->height);
    abort();
  }

  ColorRgba32 *pixels = (ColorRgba32 *) surface->data;

  return pixels + x + y * surface->width;
}

Surface *cropSurface(Surface *surface, int left, int top, int width, int height) {
  Surface *cropped = allocSurface(width, height);
  int inputRowLength = surface->width * 4;
  int rowLength = width * 4;

  for (int y = 0; y < height; ++y) {
    memcpy(cropped->data + y * rowLength, surface->data + (top + y) * inputRowLength + left * 4, rowLength);
  }

  return cropped;
}
