#include <libluna/Image.hpp>

#include <cstring> // memcpy
#include <iostream>
#include <vector>

#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/String.hpp>

using namespace Luna;

Image::Image() = default;

Image::Image(int bitsPerPixel, const Vector2i &size) {
  // logDebug("create image {}x{} {}bpp", size.width, size.height, bitsPerPixel);
  mBitsPerPixel = bitsPerPixel;
  mSize = size;
  mData.resize(getByteCount());
}

Image::Image(const Image &other) : Image(other.mBitsPerPixel, other.mSize) {
  logWarn("copy image {}x{} {}bpp", mSize.width, mSize.height, mBitsPerPixel);
  mData = other.mData;
}

Image::Image(Image &&other) {
  // logDebug("move image {}x{} {}bpp", other.mSize.width, other.mSize.height, other.mBitsPerPixel);
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = std::move(other.mData);
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;
  other.mSize = Vector2i::zero();
}

Image Image::operator=(const Image &other) {
  logWarn("copy image {}x{} {}bpp", other.mSize.width, other.mSize.height, other.mBitsPerPixel);
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = other.mData;
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;

  return *this;
}

Image &Image::operator=(Image &&other) {
  // logDebug("move image {}x{} {}bpp", other.mSize.width, other.mSize.height, other.mBitsPerPixel);
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = std::move(other.mData);
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;
  other.mSize = Vector2i::zero();

  return *this;
}

Image::~Image() = default;

int Image::getBitsPerPixel() const { return mBitsPerPixel; }

Vector2i Image::getSize() const { return mSize; }

void Image::setPalette(PalettePtr palette) { mPalette = palette; }

PalettePtr Image::getPalette() const { return mPalette; }

const uint8_t *Image::getData() const { return mData.data(); }

uint8_t *Image::getData() { return mData.data(); }

const ColorRgb16 *Image::getRgb16() const {
  return reinterpret_cast<const ColorRgb16 *>(getData());
}

ColorRgb16 *Image::getRgb16() {
  return reinterpret_cast<ColorRgb16 *>(getData());
}

const ColorRgb24 *Image::getRgb24() const {
  return reinterpret_cast<const ColorRgb24 *>(getData());
}

ColorRgb24 *Image::getRgb24() {
  return reinterpret_cast<ColorRgb24 *>(getData());
}

const ColorRgb32 *Image::getRgb32() const {
  return reinterpret_cast<const ColorRgb32 *>(getData());
}

ColorRgb32 *Image::getRgb32() {
  return reinterpret_cast<ColorRgb32 *>(getData());
}

Image Image::toRgb16() {
  switch (mBitsPerPixel) {
  case 16:
    return *this;
  default:
    // todo: error
    return *this;
  }
}

Image Image::crop(Vector2i size, Vector2i offset) {
  auto maxSize = this->getSize() - offset;
  size = Vector2i(
      std::min(size.width, maxSize.width), std::min(size.height, maxSize.height)
  );

  auto croppedImage = Image(mBitsPerPixel, size);
  int bytesPerRowFull = this->getBytesPerRow();
  int bytesPerRowCropped = croppedImage.getBytesPerRow();
  int bytesPerPixel = this->getBitsPerPixel() / 8;
  int xOffset = offset.x * bytesPerPixel;

  for (int y = 0; y < size.height; ++y) {
    auto inputRow =
        this->getData() + xOffset + bytesPerRowFull * (offset.y + y);
    auto outputRow = croppedImage.getData() + bytesPerRowCropped * y;
    std::memcpy(outputRow, inputRow, bytesPerRowCropped);
  }

  return croppedImage;
}

uint8_t Image::getNibbleAt(int x, int y) const {
  auto &byte = getData()[(x + y * getSize().width) / 2];

  if (x % 2) {
    return (byte >> 4) & 0xf;
  } else {
    return byte & 0xf;
  }
}

void Image::setNibbleAt(int x, int y, uint8_t value) {
  auto &byte = getData()[(x + y * getSize().width) / 2];

  if (x % 2) {
    byte = (value & 0xf) | (byte & 0xf0);
  } else {
    byte = ((value << 4) & 0xf0) | (byte & 0xf);
  }
}

uint8_t &Image::byteAt(int x, int y) {
  return getData()[x + y * getSize().width];
}

ColorRgb16 &Image::rgb16At(int x, int y) {
  return getRgb16()[x + y * getSize().width];
}

ColorRgb24 &Image::rgb24At(int x, int y) {
  return getRgb24()[x + y * getSize().width];
}

ColorRgb32 &Image::rgb32At(int x, int y) {
  return getRgb32()[x + y * getSize().width];
}

void Image::setInterpolation(bool enabled) { mInterpolate = enabled; }

bool Image::isInterpolated() const { return mInterpolate; }
