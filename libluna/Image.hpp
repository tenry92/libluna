#pragma once

#include <memory>

#include <libluna/Color.hpp>
#include <libluna/Palette.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Image {
    public:
    Image();
    Image(int bitsPerPixel, const Vector2i &size, int frameCount = 1);
    Image(const Image &other);
    Image(Image &&other);
    static inline Image makeIndexed(const Vector2i &size, int frameCount = 1) {
      return Image(4, size, frameCount);
    }
    static inline Image makeTrue(const Vector2i &size, int frameCount = 1) {
      return Image(32, size, frameCount);
    }
    ~Image();
    int getBitsPerPixel() const;
    Vector2i getSize() const;
    int getFrameCount() const;
    const uint8_t *getFrameData(int frameIndex = 0) const;
    void setFrameData(int frameIndex, const uint8_t *frameData);
    Image toTrue(std::shared_ptr<Palette> palette);

    inline bool isIndexed() const { return getBitsPerPixel() <= 8; }

    inline bool isTrue() const { return getBitsPerPixel() >= 24; }

    int getBytesPerFrame() const {
      return getSize().x() * getSize().y() / 2 * (getBitsPerPixel() / 4);
    }

    // todo: rename to "getBytesPerRow"
    int getBytesPerLine() const {
      return getSize().x() / 2 * (getBitsPerPixel() / 4);
    }

    inline int getPixelValueAt(int frameIndex, int x, int y) const {
      const uint8_t *firstByte =
          getFrameData(frameIndex) +
          (x + y * getSize().x()) * (getBitsPerPixel() / 4) / 2;

      if (getBitsPerPixel() == 4) {
        if (x % 2 == 0) {
          return *firstByte & 0xf;
        } else {
          return (*firstByte >> 4) & 0xf;
        }
      } else {
        return *firstByte;
      }
    }

    inline Color getPixelColorAt(int frameIndex, int x, int y) const {
      const uint8_t *firstByte =
          getFrameData(frameIndex) +
          (x + y * getSize().x()) * (getBitsPerPixel() / 4) / 2;

      float red = static_cast<float>(firstByte[0]) / 255;
      float green = static_cast<float>(firstByte[1]) / 255;
      float blue = static_cast<float>(firstByte[2]) / 255;
      float alpha = getBitsPerPixel() == 32
                        ? static_cast<float>(firstByte[3]) / 255
                        : 1.f;

      return Color(red, green, blue, alpha);
    }

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
