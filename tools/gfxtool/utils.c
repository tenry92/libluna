#include "utils.h"

#include <libgfx/libgfx.h>

int isArg(const char *arg, const char *shortName, const char *longName) {
  if (shortName) {
    if (strcmp(arg, shortName) == 0) {
      return 1;
    }
  }

  if (longName) {
    if (strcmp(arg, longName) == 0) {
      return 1;
    }
  }

  return 0;
}

int parseCommaSeparatedIntegers(const char *input, int *output, int size) {
  int count = 0;
  char *token;
  char *inputCopy = strdup(input);

  token = strtok(inputCopy, ",");

  while (token != NULL && count < size) {
    output[count] = atoi(token);
    ++count;
    token = strtok(NULL, ",");
  }

  free(inputCopy);

  return count;
}

void setColor4Bit(void *buffer, int offset, int value) {
  uint8_t *ptr = (uint8_t *) buffer;
  uint8_t byte = ptr[offset / 2];

  if (offset % 2) {
    // set high nibble
    ptr[offset] = (byte & 0xf) | ((value & 0xf) << 4);
  } else {
    // set low nibble
    ptr[offset] = (byte & 0xf0) | (value & 0xf);
  }
}

void setColor8Bit(void *buffer, int offset, int value) {
  uint8_t *ptr = (uint8_t *) buffer;
  ptr[offset] = value;
}

void setColor16BitRgba(void *buffer, int offset, int red, int green, int blue, int alpha) {
  // note: using Visual Studio, sizeof(libgfx_16BitPixel) may not equal 2 bytes
  libgfx_16BitPixel *pixel = (libgfx_16BitPixel *) (&((uint16_t *) buffer)[offset]);

  pixel->red = red;
  pixel->green = green;
  pixel->blue = blue;
  pixel->alpha = alpha;
}

void setColor24BitRgb(void *buffer, int offset, int red, int green, int blue) {
  libgfx_24BitPixel *ptr = (libgfx_24BitPixel *) buffer;
  ptr[offset].red = red;
  ptr[offset].green = green;
  ptr[offset].blue = blue;
}

void setColor32BitRgba(void *buffer, int offset, int red, int green, int blue, int alpha) {
  libgfx_32BitPixel *ptr = (libgfx_32BitPixel *) buffer;
  ptr[offset].red = red;
  ptr[offset].green = green;
  ptr[offset].blue = blue;
  ptr[offset].alpha = alpha;
}
