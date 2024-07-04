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

    ColorRgb16 *colorsRgb16() const;
    ColorRgb24 *colorsRgb24() const;
    ColorRgb32 *colorsRgb32() const;

    inline ColorRgb16 &rgb16At(int index) const {
      return colorsRgb16()[index];
    }

    inline ColorRgb24 &rgb24At(int index) const {
      return colorsRgb24()[index];
    }

    inline ColorRgb32 &rgb32At(int index) const {
      return colorsRgb32()[index];
    }

    private:
    Palette(int bitsPerColor, int colorCount);
    class impl;
    std::unique_ptr<impl> mImpl;
  };
}
