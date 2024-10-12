#pragma once

#include <libluna/Color.hpp>
#include <libluna/Palette.hpp>
#include <libluna/Resource.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Image;
  using ImagePtr = std::shared_ptr<Image>;
  using ImageResPtr = ResourcePtr<Image>;

  /**
   * @brief Store a 2D pixel bitmap on the memory.
   *
   * An image is defined by its two dimensional size in pixels and number of
   * bits per pixel (bpp).
   *
   * The number of bits per pixel does not imply a specific color format.
   * In general, these bit values are associated with these possible color
   * formats:
   *
   * - 4 bpp: 16 colors using palette or grayscale
   * - 8 bpp: 256 colors using palette or grayscale
   * - 16 bpp: RGBA16, using 5 bits for red/green/blue and 1 bit for alpha
   * - 24 bpp: RGB24, using 8 bits for red/green/blue
   * - 32 bpp: RGBA32, using 8 bits for red/green/blue/alpha
   */
  class Image {
    public:
    /**
     * @brief Create an empty image.
     */
    Image();

    /**
     * @brief Create an image with given bits per pixel and size.
     */
    Image(int bitsPerPixel, const Vector2i &size);

    Image(const Image &other);

    Image(Image &&other);

    Image operator=(const Image &other);

    Image &operator=(Image &&other);

    ~Image();

    int getBitsPerPixel() const;

    Vector2i getSize() const;

    void setPalette(PalettePtr palette);
    PalettePtr getPalette() const;

    const uint8_t *getData() const;
    uint8_t *getData();
    const ColorRgb16 *getRgb16() const;
    ColorRgb16 *getRgb16();
    const ColorRgb24 *getRgb24() const;
    ColorRgb24 *getRgb24();
    const ColorRgb32 *getRgb32() const;
    ColorRgb32 *getRgb32();

    /**
     * @brief Convert the image to RGB16.
     *
     * If the image is in RGB24 or RGB32, the bits per channel are reduced.
     * If the image is already in RGB16, a reference to the same instance is
     * returned.
     * If the image is using a palette, a palette must be assigned first.
     */
    Image toRgb16();

    /**
     * @brief Convert the image to RGB24.
     *
     * If the image is in RGB16 or RGB32, the bits per channel are adjusted.
     * The alpha channel is discared.
     * If the image is already in RGB24, a reference to the same instance is
     * returned.
     * If the image is using a palette, a palette must be assigned first.
     */
    Image toRgb24();

    /**
     * @brief Convert the image to RGB32.
     *
     * If the image is in RGB16 or RGB24, the bits per channel are expanded.
     * If the image is already in RGB32, a reference to the same instance is
     * returned.
     * If the image is using a palette, a palette must be assigned first.
     */
    Image toRgb32();

    /**
     * @brief Get a cropped potion of the image in the same color format.
     *
     * If the given size exceeds the image dimensions from offset, the cropped
     * image may be smaller than the given size.
     */
    Image crop(Vector2i size, Vector2i offset = Vector2i(0, 0));

    int getByteCount() const {
      return getSize().width * getSize().height * (getBitsPerPixel() / 4) / 2;
    }

    int getBytesPerRow() const {
      return getSize().width * (getBitsPerPixel() / 4) / 2;
    }

    /**
     * @brief Get nibble (4-bit value) at given coordinates.
     *
     * Unlike the other at-methods, this one does not return a reference since
     * references to half a byte is not possible.
     */
    uint8_t getNibbleAt(int x, int y) const;

    void setNibbleAt(int x, int y, uint8_t value);

    uint8_t &byteAt(int x, int y);

    ColorRgb16 &rgb16At(int x, int y);

    ColorRgb24 &rgb24At(int x, int y);

    ColorRgb32 &rgb32At(int x, int y);

    void setInterpolation(bool enabled);

    inline void enableInterpolation() { setInterpolation(true); }

    inline void disableInterpolation() { setInterpolation(false); }

    bool isInterpolated() const;

    private:
    int mBitsPerPixel; ///< Should be (indexed) 4, 8, (true) 24 or 32.
    Vector2i mSize;
    std::vector<uint8_t> mData;
    PalettePtr mPalette;
    bool mInterpolate{false};
  };
} // namespace Luna
