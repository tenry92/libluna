#include <libluna/Texture.hpp>

#include <cstring> // memcpy
#include <iostream>
#include <vector>

#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/String.hpp>

using namespace Luna;

Texture::Texture() = default;

Texture::Texture(int bitsPerPixel, const Vector2i& size) {
  // logDebug("create texture {}x{} {}bpp", size.width, size.height, bitsPerPixel);
  mBitsPerPixel = bitsPerPixel;
  mSize = size;
  mData.resize(getByteCount());
}

Texture::Texture(const Texture& other) : Texture(other.mBitsPerPixel, other.mSize) {
  logWarn("copy texture {}x{} {}bpp", mSize.width, mSize.height, mBitsPerPixel);
  mData = other.mData;
}

Texture::Texture(Texture&& other) {
  // logDebug("move texture {}x{} {}bpp", other.mSize.width, other.mSize.height, other.mBitsPerPixel);
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = std::move(other.mData);
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;
  other.mSize = Vector2i::zero();
}

Texture Texture::operator=(const Texture& other) {
  logWarn(
    "copy texture {}x{} {}bpp", other.mSize.width, other.mSize.height,
    other.mBitsPerPixel
  );
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = other.mData;
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;

  return *this;
}

Texture& Texture::operator=(Texture&& other) {
  // logDebug("move texture {}x{} {}bpp", other.mSize.width, other.mSize.height, other.mBitsPerPixel);
  mBitsPerPixel = other.mBitsPerPixel;
  mSize = other.mSize;
  mData = std::move(other.mData);
  mPalette = other.mPalette;
  mInterpolate = other.mInterpolate;
  other.mSize = Vector2i::zero();

  return *this;
}

Texture::~Texture() = default;

int Texture::getBitsPerPixel() const { return mBitsPerPixel; }

Vector2i Texture::getSize() const { return mSize; }

void Texture::setPalette(PalettePtr palette) { mPalette = palette; }

PalettePtr Texture::getPalette() const { return mPalette; }

const uint8_t* Texture::getData() const { return mData.data(); }

uint8_t* Texture::getData() { return mData.data(); }

const ColorRgb16* Texture::getRgb16() const {
  return reinterpret_cast<const ColorRgb16*>(getData());
}

ColorRgb16* Texture::getRgb16() {
  return reinterpret_cast<ColorRgb16*>(getData());
}

const ColorRgb24* Texture::getRgb24() const {
  return reinterpret_cast<const ColorRgb24*>(getData());
}

ColorRgb24* Texture::getRgb24() {
  return reinterpret_cast<ColorRgb24*>(getData());
}

const ColorRgb32* Texture::getRgb32() const {
  return reinterpret_cast<const ColorRgb32*>(getData());
}

ColorRgb32* Texture::getRgb32() {
  return reinterpret_cast<ColorRgb32*>(getData());
}

Texture Texture::toRgb16() {
  switch (mBitsPerPixel) {
  case 16:
    return *this;
  default:
    // todo: error
    return *this;
  }
}

Texture Texture::crop(Vector2i size, Vector2i offset) const {
  auto maxSize = this->getSize() - offset;
  size = Vector2i(
    std::min(size.width, maxSize.width), std::min(size.height, maxSize.height)
  );

  auto croppedTexture = Texture(mBitsPerPixel, size);
  int bytesPerRowFull = this->getBytesPerRow();
  int bytesPerRowCropped = croppedTexture.getBytesPerRow();
  int bytesPerPixel = this->getBitsPerPixel() / 8;
  int xOffset = offset.x * bytesPerPixel;

  for (int y = 0; y < size.height; ++y) {
    auto inputRow =
      this->getData() + xOffset + bytesPerRowFull * (offset.y + y);
    auto outputRow = croppedTexture.getData() + bytesPerRowCropped * y;
    std::memcpy(outputRow, inputRow, bytesPerRowCropped);
  }

  return croppedTexture;
}

std::vector<Texture> Texture::slice(Vector2i maxSliceSize, Vector2i& sliceCount) const {
  std::vector<Texture> slices;

  // add 1 to round integer calculation up
  sliceCount.x =
    (getSize().width + maxSliceSize.width - 1) / maxSliceSize.width;
  sliceCount.y =
    (getSize().height + maxSliceSize.height - 1) / maxSliceSize.height;

  slices.reserve(sliceCount.x * sliceCount.y);

  for (int y = 0; y < getSize().height; y += maxSliceSize.height) {
    for (int x = 0; x < getSize().width; x += maxSliceSize.width) {
      auto sliceSize = Vector2i(
        std::min(maxSliceSize.width, getSize().width - x),
        std::min(maxSliceSize.height, getSize().height - y)
      );
      slices.push_back(crop(sliceSize, Vector2i(x, y)));
    }
  }

  return slices;
}

uint8_t Texture::getNibbleAt(int x, int y) const {
  auto& byte = getData()[(x + y * getSize().width) / 2];

  if (x % 2) {
    return (byte >> 4) & 0xf;
  } else {
    return byte & 0xf;
  }
}

void Texture::setNibbleAt(int x, int y, uint8_t value) {
  auto& byte = getData()[(x + y * getSize().width) / 2];

  if (x % 2) {
    byte = (value & 0xf) | (byte & 0xf0);
  } else {
    byte = ((value << 4) & 0xf0) | (byte & 0xf);
  }
}

uint8_t& Texture::byteAt(int x, int y) {
  return getData()[x + y * getSize().width];
}

ColorRgb16& Texture::rgb16At(int x, int y) {
  return getRgb16()[x + y * getSize().width];
}

ColorRgb24& Texture::rgb24At(int x, int y) {
  return getRgb24()[x + y * getSize().width];
}

ColorRgb32& Texture::rgb32At(int x, int y) {
  return getRgb32()[x + y * getSize().width];
}

void Texture::setInterpolation(bool enabled) { mInterpolate = enabled; }

bool Texture::isInterpolated() const { return mInterpolate; }
