#pragma once

#include <cstdint>

/**
 * @brief Utilities for handling endianness.
 *
 * @ingroup system
 */
namespace Luna::Endian {
  enum Endian { Little, Big };

  // constexpr Endian getEndian() {
  //   constexpr uint32_t value = 0x01020304;
  //   return *reinterpret_cast<const uint8_t *>(&value) == 0x01 ? Endian::Big : Endian::Little;
  // }

  /**
   * @brief Get the system's endianness at compile time.
   */
  constexpr Endian getEndian() {
#ifdef N64
    return Endian::Big;
#else
    return Endian::Little;
#endif
  }

  /**
   * @brief Swap the endianness of a value.
   *
   * @param value The value to swap.
   */
  template <typename T> T swapEndian(T value) {
    if constexpr (sizeof(T) == 1) {
      return value;
    } else if constexpr (sizeof(T) == 2) {
      return (value >> 8) | (value << 8);
    } else if constexpr (sizeof(T) == 4) {
      return (value >> 24) | ((value >> 8) & 0x0000FF00) |
             ((value << 8) & 0x00FF0000) | (value << 24);
    } else if constexpr (sizeof(T) == 8) {
      return (value >> 56) | ((value >> 40) & 0x000000000000FF00) |
             ((value >> 24) & 0x0000000000FF0000) |
             ((value >> 8) & 0x00000000FF000000) |
             ((value << 8) & 0x000000FF00000000) |
             ((value << 24) & 0x0000FF0000000000) |
             ((value << 40) & 0x00FF000000000000) | (value << 56);
    }
  }

  /**
   * @brief Convert a value from little-endian to the system's endianness.
   *
   * If the system is little-endian, the value is returned as-is.
   */
  template <typename T> T fromLittleEndian(T value) {
    if constexpr (getEndian() == Endian::Little) {
      return value;
    } else {
      return swapEndian(value);
    }
  }

  /**
   * @brief Convert a value from big-endian to the system's endianness.
   *
   * If the system is big-endian, the value is returned as-is.
   */
  template <typename T> T fromBigEndian(T value) {
    if constexpr (getEndian() == Endian::Big) {
      return value;
    } else {
      return swapEndian(value);
    }
  }
} // namespace Luna::Endian
