#include <cstring>

#include <libpng16/png.h>

#include <libluna/Util/Png.hpp>

using namespace Luna;
using namespace Luna::Util;

class Png::impl {
  public:
  void init() {
    mPngPtr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr
    );

    mInfoPtr = png_create_info_struct(mPngPtr);

    png_set_read_fn(mPngPtr, mReader.get(), pngReadFn);

    // set max height/width to 2^14 (16,384) pixels (1 GiB with 8-bit RGBA)
    // default would be 1,000,000 (over 3725 GiB with 8-bit RGBA)
    // larger images are rejected immediately with a png_error() call
    png_set_user_limits(mPngPtr, 1 << 14, 1 << 14);

    // add alpha if missing
    png_set_add_alpha(mPngPtr, 0xFF, PNG_FILLER_AFTER);
  }

  static void
  pngReadFn(png_struct *pngPtr, png_byte *buffer, png_size_t length) {
    auto reader = reinterpret_cast<Luna::InputStream *>(png_get_io_ptr(pngPtr));
    reader->read(buffer, length);
  }

  void close() {
    if (mInfoPtr) {
      png_destroy_info_struct(mPngPtr, &mInfoPtr);
    }

    if (mPngPtr) {
      png_destroy_read_struct(&mPngPtr, nullptr, nullptr);
    }
  }

  void read() {
    png_read_png(mPngPtr, mInfoPtr, PNG_TRANSFORM_STRIP_16, nullptr);
  }

  int getWidth() { return png_get_image_width(mPngPtr, mInfoPtr); }

  int getHeight() { return png_get_image_height(mPngPtr, mInfoPtr); }

  png_bytepp getData() { return png_get_rows(mPngPtr, mInfoPtr); }

  std::unique_ptr<Luna::InputStream> mReader;
  png_structp mPngPtr{nullptr};
  png_infop mInfoPtr{nullptr};
};

Png::Png(std::unique_ptr<Luna::InputStream> reader)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mReader = std::move(reader);
}

Png::~Png() = default;

ImagePtr Png::decode(int framesPerRow, int framesPerColumn) {
  mImpl->init();
  mImpl->read();

  int frameWidth = mImpl->getWidth() / framesPerRow;
  int frameHeight = mImpl->getHeight() / framesPerColumn;
  int frameCount = framesPerRow * framesPerColumn;

  auto image = Image::make(
      32, Vector2i(frameWidth, frameHeight), frameCount
  );

  for (int frameY = 0; frameY < framesPerColumn; ++frameY) {
    for (int frameX = 0; frameX < framesPerRow; ++frameX) {
      std::vector<uint8_t> frameData(image->getBytesPerFrame());

      for (int row = 0; row < frameHeight; ++row) {
        auto rowPointer = mImpl->getData()[frameY * frameHeight + row] +
                          frameX * image->getBytesPerLine();
        std::memcpy(
            frameData.data() + row * image->getBytesPerLine(), rowPointer,
            image->getBytesPerLine()
        );
      }

      image->setFrameData(frameX + frameY * framesPerRow, frameData.data());
    }
  }

  mImpl->close();

  return image;
}
