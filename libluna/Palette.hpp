#pragma once

#include <memory>
#include <vector>

#include <libluna/Color.hpp>

namespace Luna {
  class Palette;
  using PalettePtr = std::shared_ptr<Palette>;

  class Palette {
    public:
    static PalettePtr make(int bitsPerColor, int colorCount);
    ~Palette();

    ColorRgb16 *colorsRgb16();
    ColorRgb24 *colorsRgb24();
    ColorRgb32 *colorsRgb32();

    inline ColorRgb16 &rgb16At(int index) { return colorsRgb16()[index]; }

    inline ColorRgb24 &rgb24At(int index) { return colorsRgb24()[index]; }

    inline ColorRgb32 &rgb32At(int index) { return colorsRgb32()[index]; }

    private:
    Palette(int bitsPerColor, int colorCount);
    int mBitsPerColor;
    int mColorCount;
    std::vector<uint8_t> mColors;
  };
} // namespace Luna
