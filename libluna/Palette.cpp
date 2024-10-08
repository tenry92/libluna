#include <libluna/Palette.hpp>

using namespace Luna;

Palette::Palette(int bitsPerColor, int colorCount) {
  mBitsPerColor = bitsPerColor;
  mColorCount = colorCount;
  mColors.resize(colorCount * bitsPerColor / 8);
}

Palette::~Palette() = default;

PalettePtr Palette::make(int bitsPerColor, int colorCount) {
  return std::make_shared<Palette>(bitsPerColor, colorCount);
}

ColorRgb16 *Palette::colorsRgb16() {
  return reinterpret_cast<ColorRgb16 *>(mColors.data());
}

ColorRgb24 *Palette::colorsRgb24() {
  return reinterpret_cast<ColorRgb24 *>(mColors.data());
}

ColorRgb32 *Palette::colorsRgb32() {
  return reinterpret_cast<ColorRgb32 *>(mColors.data());
}
