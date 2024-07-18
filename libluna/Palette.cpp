#include <libluna/Palette.hpp>

using namespace Luna;

class Palette::impl {
  public:
  int mBitsPerColor;
  int mColorCount;
  std::vector<uint8_t> mColors;
};

Palette::Palette(int bitsPerColor, int colorCount)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mBitsPerColor = bitsPerColor;
  mImpl->mColorCount = colorCount;
  mImpl->mColors.resize(colorCount * bitsPerColor / 8);
}

Palette::~Palette() = default;

PalettePtr Palette::make(int bitsPerColor, int colorCount) {
  return std::make_shared<Palette>(bitsPerColor, colorCount);
}

ColorRgb16 *Palette::colorsRgb16() const {
  return reinterpret_cast<ColorRgb16 *>(mImpl->mColors.data());
}

ColorRgb24 *Palette::colorsRgb24() const {
  return reinterpret_cast<ColorRgb24 *>(mImpl->mColors.data());
}

ColorRgb32 *Palette::colorsRgb32() const {
  return reinterpret_cast<ColorRgb32 *>(mImpl->mColors.data());
}
