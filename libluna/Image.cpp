#include <libluna/Image.hpp>

#include <cstring>
#include <iostream>
#include <vector>

#include <libluna/ResourceReader.hpp>
#include <libluna/String.hpp>

using namespace Luna;

class Image::impl {
  public:
  int mBitsPerPixel; ///< Should be (indexed) 4, 8, (true) 24 or 32.
  Vector2i mSize;
  std::vector<uint8_t> mData;
  PalettePtr mPalette;
  bool mInterpolate{false};
};

ImagePtr Image::make() { return ImagePtr(new Image()); }
ImagePtr Image::make(int bitsPerPixel, const Vector2i &size) {
  return ImagePtr(new Image(bitsPerPixel, size));
}
ImagePtr Image::make(const Image &other) { return ImagePtr(new Image(other)); }
ImagePtr Image::make(Image &&other) {
  return ImagePtr(new Image(std::move(other)));
}

Image::Image() : mImpl{std::make_unique<impl>()} {}

Image::Image(int bitsPerPixel, const Vector2i &size)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mBitsPerPixel = bitsPerPixel;
  mImpl->mSize = size;
  mImpl->mData.resize(getByteCount());
}

Image::Image(const Image &other)
    : Image(other.mImpl->mBitsPerPixel, other.mImpl->mSize) {
  mImpl->mData = other.mImpl->mData;
}

Image::Image(Image &&other) {
  mImpl = std::move(other.mImpl);
  other.mImpl = std::make_unique<impl>();
}

Image::~Image() = default;

int Image::getBitsPerPixel() const { return mImpl->mBitsPerPixel; }

Vector2i Image::getSize() const { return mImpl->mSize; }

void Image::setPalette(PalettePtr palette) { mImpl->mPalette = palette; }

PalettePtr Image::getPalette() const { return mImpl->mPalette; }

uint8_t *Image::getData() const { return mImpl->mData.data(); }

ColorRgb16 *Image::getRgb16() const {
  return reinterpret_cast<ColorRgb16 *>(getData());
}

ColorRgb24 *Image::getRgb24() const {
  return reinterpret_cast<ColorRgb24 *>(getData());
}

ColorRgb32 *Image::getRgb32() const {
  return reinterpret_cast<ColorRgb32 *>(getData());
}

ImagePtr Image::toRgb16() {
  switch (mImpl->mBitsPerPixel) {
  case 16:
    return shared_from_this();
  default:
    // todo: error
    return shared_from_this();
  }
}

ImagePtr Image::crop(Vector2i size, Vector2i offset) {
  auto maxSize = this->getSize() - offset;
  size = Vector2i(std::min(size.x(), maxSize.x()), std::min(size.y(), maxSize.y()));

  auto croppedImage = Image::make(mImpl->mBitsPerPixel, size);
  int bytesPerRowFull = this->getBytesPerRow();
  int bytesPerRowCropped = croppedImage->getBytesPerRow();
  int bytesPerPixel = this->getBitsPerPixel() / 8;
  int xOffset = offset.x() * bytesPerPixel;

  for (int y = 0; y < size.y(); ++y) {
    auto inputRow = this->getData() + xOffset + bytesPerRowFull * (offset.y() + y);
    auto outputRow = croppedImage->getData() + bytesPerRowCropped * y;
    std::memcpy(outputRow, inputRow, bytesPerRowCropped);
  }

  return croppedImage;
}

uint8_t Image::getNibbleAt(int x, int y) const {
  auto &byte = getData()[(x + y * getSize().x()) / 2];

  if (x % 2) {
    return (byte >> 4) & 0xf;
  } else {
    return byte & 0xf;
  }
}

void Image::setNibbleAt(int x, int y, uint8_t value) {
  auto &byte = getData()[(x + y * getSize().x()) / 2];

  if (x % 2) {
    byte = (value & 0xf) | (byte & 0xf0);
  } else {
    byte = ((value << 4) & 0xf0) | (byte & 0xf);
  }
}

uint8_t &Image::byteAt(int x, int y) const {
  return getData()[x + y * getSize().x()];
}

ColorRgb16 &Image::rgb16At(int x, int y) const {
  return getRgb16()[x + y * getSize().x()];
}

ColorRgb24 &Image::rgb24At(int x, int y) const {
  return getRgb24()[x + y * getSize().x()];
}

ColorRgb32 &Image::rgb32At(int x, int y) const {
  return getRgb32()[x + y * getSize().x()];
}

void Image::setInterpolation(bool enabled) {
  mImpl->mInterpolate = enabled;
}

bool Image::isInterpolated() const {
  return mImpl->mInterpolate;
}
