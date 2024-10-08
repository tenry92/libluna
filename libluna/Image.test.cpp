#include <libluna/Image.hpp>
#include <libluna/Test.hpp>

using namespace std;
using namespace Luna;

int main(int, char **) {
  TEST("empty image", []() {
    auto image = Image::make();
    ASSERT(image->getSize().width == 0, "getWidth() == 0");
    ASSERT(image->getSize().height == 0, "getHeight() == 0");
  });

  TEST("image size", []() {
    auto image = Image::make(4, {16, 32});
    ASSERT(image->getBitsPerPixel() == 4, "getBitsPerPixel() == 4");
    ASSERT(image->getSize().width == 16, "getWidth() == 16");
    ASSERT(image->getSize().height == 32, "getHeight() == 32");
  });

  TEST("getByteCount()", []() {
    // (2 * 2) / 2 = 2
    auto image4 = Image::make(4, {2, 2});
    ASSERT(image4->getByteCount() == 2, "image4->getByteCount() == 2");

    // (2 * 2) / 1 = 4
    auto image8 = Image::make(8, {2, 2});
    ASSERT(image8->getByteCount() == 4, "image8->getByteCount() == 4");

    // (2 * 2) * 3 = 12
    auto image24 = Image::make(24, {2, 2});
    ASSERT(image24->getByteCount() == 12, "image24->getByteCount() == 12");

    // (2 * 2) * 4 = 16
    auto image32 = Image::make(32, {2, 2});
    ASSERT(image32->getByteCount() == 16, "image32->getByteCount() == 16");
  });

  TEST("pixels (4bpp indexed)", []() {
    auto image = Image::make(4, {2, 2});
    uint8_t frame[] = {0 | (1 << 4), 2 | (3 << 4)};
    memcpy(image->getData(), frame, image->getByteCount());

    ASSERT_EQL(image->getNibbleAt(0, 0), 0, "(0, 0)");
    ASSERT_EQL(image->getNibbleAt(1, 0), 1, "(1, 0)");
    ASSERT_EQL(image->getNibbleAt(0, 1), 2, "(0, 1)");
    ASSERT_EQL(image->getNibbleAt(1, 1), 3, "(1, 1)");
  });

  TEST("pixels (8bpp indexed)", []() {
    auto image = Image::make(8, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3};
    memcpy(image->getData(), frame, image->getByteCount());

    ASSERT_EQL(image->byteAt(0, 0), 0, "(0, 0)");
    ASSERT_EQL(image->byteAt(1, 0), 1, "(1, 0)");
    ASSERT_EQL(image->byteAt(0, 1), 2, "(0, 1)");
    ASSERT_EQL(image->byteAt(1, 1), 3, "(1, 1)");
  });

  TEST("pixels (24bpp true)", []() {
    auto image = Image::make(24, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    memcpy(image->getData(), frame, image->getByteCount());

    ASSERT_EQL(image->rgb24At(0, 0).red, 0, "(0, 0) red");
    ASSERT_EQL(image->rgb24At(0, 0).green, 1, "(0, 0) green");
    ASSERT_EQL(image->rgb24At(0, 0).blue, 2, "(0, 0) blue");

    ASSERT_EQL(image->rgb24At(1, 0).red, 3, "(1, 0) red");
    ASSERT_EQL(image->rgb24At(1, 0).green, 4, "(1, 0) green");
    ASSERT_EQL(image->rgb24At(1, 0).blue, 5, "(1, 0) blue");

    ASSERT_EQL(image->rgb24At(0, 1).red, 6, "(0, 1) red");
    ASSERT_EQL(image->rgb24At(0, 1).green, 7, "(0, 1) green");
    ASSERT_EQL(image->rgb24At(0, 1).blue, 8, "(0, 1) blue");

    ASSERT_EQL(image->rgb24At(1, 1).red, 9, "(1, 1) red");
    ASSERT_EQL(image->rgb24At(1, 1).green, 10, "(1, 1) green");
    ASSERT_EQL(image->rgb24At(1, 1).blue, 11, "(1, 1) blue");
  });

  TEST("pixels (32bpp true)", []() {
    auto image = Image::make(32, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    memcpy(image->getData(), frame, image->getByteCount());

    ASSERT_EQL(image->rgb32At(0, 0).red, 0, "(0, 0) red");
    ASSERT_EQL(image->rgb32At(0, 0).green, 1, "(0, 0) green");
    ASSERT_EQL(image->rgb32At(0, 0).blue, 2, "(0; 0, 0) blue");
    ASSERT_EQL(image->rgb32At(0, 0).alpha, 3, "(0, 0) alpha");

    ASSERT_EQL(image->rgb32At(1, 0).red, 4, "(1, 0) red");
    ASSERT_EQL(image->rgb32At(1, 0).green, 5, "(1, 0) green");
    ASSERT_EQL(image->rgb32At(1, 0).blue, 6, "(0; 1, 0) blue");
    ASSERT_EQL(image->rgb32At(1, 0).alpha, 7, "(1, 0) alpha");

    ASSERT_EQL(image->rgb32At(0, 1).red, 8, "(0, 1) red");
    ASSERT_EQL(image->rgb32At(0, 1).green, 9, "(0, 1) green");
    ASSERT_EQL(image->rgb32At(0, 1).blue, 10, "(0; 0, 1) blue");
    ASSERT_EQL(image->rgb32At(0, 1).alpha, 11, "(0, 1) alpha");

    ASSERT_EQL(image->rgb32At(1, 1).red, 12, "(1, 1) red");
    ASSERT_EQL(image->rgb32At(1, 1).green, 13, "(1, 1) green");
    ASSERT_EQL(image->rgb32At(1, 1).blue, 14, "(0; 0, 1) blue");
    ASSERT_EQL(image->rgb32At(1, 1).alpha, 15, "(1, 1) alpha");
  });

  return runTests();
}
