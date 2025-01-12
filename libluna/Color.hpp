#pragma once

#include <cstdint>

namespace Luna {
  /**
   * @defgroup colors Colors
   *
   * @brief Everything about colors.
   *
   * When dealing with colors in general, use ColorRgb. When working with actual
   * image data, use the respective packed formats.
   *
   * @{
   */
#pragma pack(push, 1)
  /**
   * @brief 16-bit packed "high" color in RGBA (5551) format.
   *
   * Uses 5 bits for each color channel and 1 bit for alpha channel.
   */
  struct ColorRgb16 {
    uint8_t red : 5; ///< Value between 0 and 31.
    uint8_t green : 5; ///< Value between 0 and 31.
    uint8_t blue : 5; ///< Value between 0 and 31.
    uint8_t alpha : 1; ///< 0 for transparent, 1 for opaque.
  };

  /**
   * @brief 24-bit packed "true" color in RGB (888) format.
   */
  struct ColorRgb24 {
    uint8_t red{0}; ///< Value between 0 and 255.
    uint8_t green{0}; ///< Value between 0 and 255.
    uint8_t blue{0}; ///< Value between 0 and 255.
  };

  /**
   * @brief 32-bit packed "true" color in RGBA (8888) format.
   */
  struct ColorRgb32 {
    uint8_t red{0}; ///< Value between 0 and 255.
    uint8_t green{0}; ///< Value between 0 and 255.
    uint8_t blue{0}; ///< Value between 0 and 255.
    uint8_t alpha{255}; ///< Value between 0 and 255.
  };

  /**
   * @brief RGBA color using floating point values.
   */
  struct ColorRgb {
    float red{0}; ///< Value between 0 and 1.
    float green{0}; ///< Value between 0 and 1.
    float blue{0}; ///< Value between 0 and 1.
    float alpha{1}; ///< Value between 0 and 1.
  };

  /**
   * @brief HSV color using floating point values.
   */
  struct ColorHsv {
    float hue; ///< Value between 0 and 360.
    float saturation; ///< Value between 0 and 1.
    float value; ///< Value between 0 and 1.
    float alpha{1}; ///< Value between 0 and 1.
  };

  /**
   * @brief HSL color using floating point values.
   */
  struct ColorHsl {
    float hue; ///< Value between 0 and 360.
    float saturation; ///< Value between 0 and 1.
    float lightness; ///< Value between 0 and 1.
    float alpha{1}; ///< Value between 0 and 1.
  };

  /**
   * @brief YUV color using floating point values.
   */
  struct ColorYuv {
    float y; ///< Value between 0 and 1.
    float u; ///< Value between -0.5 and 0.5.
    float v; ///< Value between -0.5 and 0.5.
    float alpha{1}; ///< Value between 0 and 1.
  };
#pragma pack(pop)

  /**
   * @brief Convert 5-bit color component (0-31) to 8-bit (0-255).
   */
  inline uint8_t expand5BitTo8Bit(uint8_t value) {
    return (value << 3) | (value >> 2);
  }

  /**
   * @brief Convert 8-bit color component (0-255) to 5-bit (0-31).
   */
  inline uint8_t reduce8BitTo5Bit(uint8_t value) { return (value >> 3) & 0x1f; }

  /**
   * @name Construct ColorRgb16
   */
  ///@{
  inline ColorRgb16 makeColorRgb16(ColorRgb16 other) { return other; }

  /**
   * @brief Reduce 24-bit color (8 bits per component) to 16-bit color (5 bits per component).
   */
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

  /**
   * @brief Reduce 32-bit color (8 bits per component) to 16-bit color (5 bits per component).
   *
   * @note Alpha channel is reduced to a single bit.
   */
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

  /**
   * @brief Reduce floating point (8 bits per component) to 16-bit color (5 bits per component).
   *
   * @note Alpha channel is reduced to a single bit.
   */
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
  ///@}

  /**
   * @brief Construct uint16_t color value from any color type.
   */
  template <typename T> inline uint16_t makeColorUint16(T other) {
    ColorRgb16 tmp = makeColorRgb16(other);

    return tmp.red | (tmp.green << 5) | (tmp.blue << 10) | (tmp.alpha << 15);
  }

  /**
   * @name Construct ColorRgb24
   */
  ///@{
  /**
   * @brief Expand 16-bit color (5 bits per component) to 24-bit color (8 bits per component).
   *
   * @note Alpha channel is removed.
   */
  inline ColorRgb24 makeColorRgb24(ColorRgb16 other) {
    ColorRgb24 result;
    result.red = expand5BitTo8Bit(other.red);
    result.green = expand5BitTo8Bit(other.green);
    result.blue = expand5BitTo8Bit(other.blue);
    return result;
  }

  inline ColorRgb24 makeColorRgb24(ColorRgb24 other) { return other; }

  /**
   * @brief Convert RGBA color to RGB color (remove alpha channel).
   */
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
  ///@}

  /**
   * @name Construct ColorRgb32
   */
  ///@{
  /**
   * @brief Expand 16-bit color (5 bits per component) to 32-bit color (8 bits per component).
   */
  inline ColorRgb32 makeColorRgb32(ColorRgb16 other) {
    ColorRgb32 result;
    result.red = expand5BitTo8Bit(other.red);
    result.green = expand5BitTo8Bit(other.green);
    result.blue = expand5BitTo8Bit(other.blue);
    result.alpha = other.alpha ? 255 : 0;
    return result;
  }

  /**
   * @brief Convert RGB color to RGBA color (add alpha channel).
   */
  inline ColorRgb32 makeColorRgb32(ColorRgb24 other) {
    ColorRgb32 result;
    result.red = other.red;
    result.green = other.green;
    result.blue = other.blue;
    result.alpha = 255;
    return result;
  }

  inline ColorRgb32 makeColorRgb32(ColorRgb32 other) { return other; }

  inline ColorRgb32 makeColorRgb32(ColorRgb other) {
    ColorRgb32 result;
    result.red = static_cast<uint8_t>(other.red * 255);
    result.green = static_cast<uint8_t>(other.green * 255);
    result.blue = static_cast<uint8_t>(other.blue * 255);
    result.alpha = static_cast<uint8_t>(other.alpha * 255);
    return result;
  }
  ///@}
  /**
   * @}
   */
} // namespace Luna
