#pragma once

#include <array>
#include <cstddef>

#include <math.h>

namespace Luna {
#pragma pack(push, 1)
  struct ColorRgb16 {
    uint8_t red : 5;
    uint8_t green : 5;
    uint8_t blue : 5;
    uint8_t alpha : 1;
  };

  struct ColorRgb24 {
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
  };

  struct ColorRgb32 {
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
    uint8_t alpha{255};
  };

  struct ColorRgb {
    float red{0};
    float green{0};
    float blue{0};
    float alpha{1};
  };

  struct ColorHsv {
    float hue;
    float saturation;
    float value;
    float alpha{1};
  };

  struct ColorHsl {
    float hue;
    float saturation;
    float lightness;
    float alpha{1};
  };

  struct ColorYuv {
    float y;
    float u;
    float v;
    float alpha{1};
  };
#pragma pack(pop)

  inline uint8_t expand5BitTo8Bit(uint8_t value) {
    return (value << 3) | (value >> 2);
  }

  inline uint8_t reduce8BitTo5Bit(uint8_t value) {
    return (value >> 3) & 0x1f;
  }

  inline ColorRgb16 makeColorRgb16(ColorRgb16 other) {
    return other;
  }

  inline ColorRgb16 makeColorRgb16(ColorRgb24 other) {
    ColorRgb16 result;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    result.red = reduce8BitTo5Bit(other.red);
    result.green = reduce8BitTo5Bit(other.green);
    result.blue = reduce8BitTo5Bit(other.blue);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    result.alpha = 1;
    return result;
  }

  inline ColorRgb16 makeColorRgb16(ColorRgb32 other) {
    ColorRgb16 result;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    result.red = reduce8BitTo5Bit(other.red);
    result.green = reduce8BitTo5Bit(other.green);
    result.blue = reduce8BitTo5Bit(other.blue);
    result.alpha = (other.alpha >> 7) & 0x1;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    return result;
  }

  inline ColorRgb16 makeColorRgb16(ColorRgb other) {
    ColorRgb16 result;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    result.red = static_cast<uint8_t>(other.red * 31);
    result.green = static_cast<uint8_t>(other.green * 31);
    result.blue = static_cast<uint8_t>(other.blue * 31);
    result.alpha = static_cast<uint8_t>(other.alpha * 1);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    return result;
  }

  inline ColorRgb24 makeColorRgb24(ColorRgb16 other) {
    ColorRgb24 result;
    result.red = expand5BitTo8Bit(other.red);
    result.green = expand5BitTo8Bit(other.green);
    result.blue = expand5BitTo8Bit(other.blue);
    return result;
  }

  inline ColorRgb24 makeColorRgb24(ColorRgb24 other) {
    return other;
  }

  inline ColorRgb24 makeColorRgb24(ColorRgb32 other) {
    ColorRgb24 result;
    result.red = other.red;
    result.green = other.green;
    result.blue = other.blue;
    return result;
  }

  inline ColorRgb24 makeColorRgb24(ColorRgb other) {
    ColorRgb24 result;
    result.red = static_cast<uint8_t>(other.red * 255);
    result.green = static_cast<uint8_t>(other.green * 255);
    result.blue = static_cast<uint8_t>(other.blue * 255);
    return result;
  }

  inline ColorRgb32 makeColorRgb32(ColorRgb16 other) {
    ColorRgb32 result;
    result.red = expand5BitTo8Bit(other.red);
    result.green = expand5BitTo8Bit(other.green);
    result.blue = expand5BitTo8Bit(other.blue);
    result.alpha = other.alpha ? 255 : 0;
    return result;
  }

  inline ColorRgb32 makeColorRgb32(ColorRgb24 other) {
    ColorRgb32 result;
    result.red = other.red;
    result.green = other.green;
    result.blue = other.blue;
    result.alpha = 255;
    return result;
  }

  inline ColorRgb32 makeColorRgb32(ColorRgb32 other) {
    return other;
  }

  inline ColorRgb32 makeColorRgb32(ColorRgb other) {
    ColorRgb32 result;
    result.red = static_cast<uint8_t>(other.red * 255);
    result.green = static_cast<uint8_t>(other.green * 255);
    result.blue = static_cast<uint8_t>(other.blue * 255);
    result.alpha = static_cast<uint8_t>(other.alpha * 255);
    return result;
  }
} // namespace Luna
