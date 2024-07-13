#pragma once

#include <string.h>
#include <stdint.h>

int isArg(const char *arg, const char *shortName, const char *longName);

inline uint8_t expand5BitTo8Bit(uint8_t value) {
  return (value << 3) | (value >> 2);
}

/**
 * @brief Parse comma-separated integers.
 *
 * @param input Input string
 * @param output Output array with size elements
 * @param size Maximum number of elements to extract
 * @return Number of elements extracted
 */
int parseCommaSeparatedIntegers(const char *input, int *output, int size);

void setColor4Bit(void *buffer, int offset, int value);
void setColor8Bit(void *buffer, int offset, int value);
void setColor16BitRgba(void *buffer, int offset, int red, int green, int blue, int alpha);
void setColor24BitRgb(void *buffer, int offset, int red, int green, int blue);
void setColor32BitRgba(void *buffer, int offset, int red, int green, int blue, int alpha);
