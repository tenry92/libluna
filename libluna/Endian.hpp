#pragma once

#include <cstdint>

namespace Luna::Endian {
  enum class Endian { Little, Big };

  // constexpr Endian getEndian() {
  //   constexpr uint32_t value = 0x01020304;
  //   return *reinterpret_cast<const uint8_t *>(&value) == 0x01 ? Endian::Big : Endian::Little;
  // }

  constexpr Endian getEndian() {
#ifdef N64
    return Endian::Big;
#else
    return Endian::Little;
#endif
  }

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

  template <typename T> T fromLittleEndian(T value) {
    if constexpr (getEndian() == Endian::Little) {
      return value;
    } else {
      return swapEndian(value);
    }
  }

  template <typename T> T fromBigEndian(T value) {
    if constexpr (getEndian() == Endian::Big) {
      return value;
    } else {
      return swapEndian(value);
    }
  }
} // namespace Luna::Endian
