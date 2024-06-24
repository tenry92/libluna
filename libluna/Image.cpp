#include <libluna/Image.hpp>

#include <cstring>
#include <iostream>
#include <vector>

#include <libluna/ResourceReader.hpp>
#include <libluna/String.hpp>

using std::string;

using namespace Luna;

class Image::impl {
  public:
  int mBitsPerPixel; ///< Should be (indexed) 4, 8, (true) 24 or 32.
  Vector2i mSize;
  int mFrameCount{0};
  std::vector<std::vector<uint8_t>> mFrames;
};

ImagePtr Image::make() {
  return ImagePtr(new Image());
}
ImagePtr Image::make(int bitsPerPixel, const Vector2i &size, int frameCount) {
  return ImagePtr(new Image(bitsPerPixel, size, frameCount));
}
ImagePtr Image::make(const Image &other) {
  return ImagePtr(new Image(other));
}
ImagePtr Image::make(Image &&other) {
  return ImagePtr(new Image(std::move(other)));
}

Image::Image() : mImpl{std::make_unique<impl>()} {}

Image::Image(int bitsPerPixel, const Vector2i &size, int frameCount)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mBitsPerPixel = bitsPerPixel;
  mImpl->mSize = size;
  mImpl->mFrameCount = frameCount;
  mImpl->mFrames.resize(frameCount);

  for (int i = 0; i < frameCount; ++i) {
    mImpl->mFrames[i].resize(getBytesPerFrame());
  }
}

Image::Image(const Image &other)
    : Image(
          other.mImpl->mBitsPerPixel, other.mImpl->mSize,
          other.mImpl->mFrameCount
      ) {
  mImpl->mFrames = other.mImpl->mFrames;
}

Image::Image(Image &&other) {
  mImpl = std::move(other.mImpl);
  other.mImpl = std::make_unique<impl>();
}

Image::~Image() = default;

int Image::getBitsPerPixel() const { return mImpl->mBitsPerPixel; }

Vector2i Image::getSize() const { return mImpl->mSize; }

int Image::getFrameCount() const { return mImpl->mFrameCount; }

const uint8_t *Image::getFrameData(int frameIndex) const {
  return mImpl->mFrames[frameIndex].data();
}

void Image::setFrameData(int frameIndex, const uint8_t *frameData) {
  std::memcpy(mImpl->mFrames[frameIndex].data(), frameData, getBytesPerFrame());
}

ImagePtr Image::toTrue(std::shared_ptr<Palette> palette) {
  ImagePtr newImage = makeTrue(getSize(), getFrameCount());

  for (int frameIndex = 0; frameIndex < getFrameCount(); ++frameIndex) {
    std::vector<uint8_t> frameData;
    frameData.reserve(newImage->getBytesPerFrame());
    for (int y = 0; y < getSize().x(); ++y) {
      for (int x = 0; x < getSize().y(); ++x) {
        auto pixelValue = getPixelValueAt(frameIndex, x, y);
        auto color = palette->at(pixelValue);
        frameData.push_back(static_cast<uint8_t>(color.reduceRed(8)));
        frameData.push_back(static_cast<uint8_t>(color.reduceGreen(8)));
        frameData.push_back(static_cast<uint8_t>(color.reduceBlue(8)));
        frameData.push_back(static_cast<uint8_t>(color.reduceAlpha(8)));
      }
    }
    newImage->setFrameData(frameIndex, frameData.data());
  }

  return newImage;
}
