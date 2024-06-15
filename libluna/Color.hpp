#pragma once

#include <array>
#include <cstddef>

#include <math.h>

namespace Luna {
  struct Color {
    public:
    constexpr Color() : Color(0, 0, 0) {}
    constexpr Color(float pRed, float pGreen, float pBlue, float pAlpha = 1.f)
        : red(pRed), green(pGreen), blue(pBlue), alpha(pAlpha) {}

    static inline int reduceDepth(float value, std::size_t bits) {
      if (bits == 0) {
        return 0;
      }

      return static_cast<int>(round(value * static_cast<float>((1 << bits) - 1))
      );
    }

    template <typename Type = int>
    inline std::array<Type, 4>
    reduceAll(std::size_t bits = sizeof(Type) / 8) const {
      return {
          this->reduceRed<Type>(bits), this->reduceGreen<Type>(bits),
          this->reduceBlue<Type>(bits), this->reduceAlpha<Type>(bits)};
    }

    template <typename Type = int>
    inline Type reduceRed(std::size_t bits = sizeof(Type) / 8) const {
      return static_cast<Type>(reduceDepth(red, bits));
    }

    template <typename Type = int>
    inline Type reduceGreen(std::size_t bits = sizeof(Type) / 8) const {
      return static_cast<Type>(reduceDepth(green, bits));
    }

    template <typename Type = int>
    inline Type reduceBlue(std::size_t bits = sizeof(Type) / 8) const {
      return static_cast<Type>(reduceDepth(blue, bits));
    }

    template <typename Type = int>
    inline Type reduceAlpha(std::size_t bits = sizeof(Type) / 8) const {
      return static_cast<Type>(reduceDepth(alpha, bits));
    }

    static inline Color Transparent() { return Color(0.0f, 0.0f, 0.0f, 0.0f); };

    float red;
    float green;
    float blue;
    float alpha;
  };
} // namespace Luna
