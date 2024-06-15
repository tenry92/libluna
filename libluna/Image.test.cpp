#include <memory>

#include <libluna/Image.hpp>
#include <libluna/Test.hpp>

using namespace std;
using namespace Luna;

int main(int, char **) {
  TEST("empty image", []() {
    auto image = Image::make();
    ASSERT(image->getSize().x() == 0, "getWidth() == 0");
    ASSERT(image->getSize().y() == 0, "getHeight() == 0");
    ASSERT(image->getFrameCount() == 0, "getFrameCount() == 0");
  });

  TEST("image size", []() {
    auto image = Image::make(4, {16, 32}, 3);
    ASSERT(image->getBitsPerPixel() == 4, "getBitsPerPixel() == 4");
    ASSERT(image->getSize().x() == 16, "getWidth() == 16");
    ASSERT(image->getSize().y() == 32, "getHeight() == 32");
    ASSERT(image->getFrameCount() == 3, "getFrameCount() == 3");
  });

  TEST("isIndexed()", []() {
    auto image4 = Image::make(4, {16, 16}, 1);
    ASSERT(image4->isIndexed(), "image4->isIndexed()");

    auto image8 = Image::make(8, {16, 16}, 1);
    ASSERT(image8->isIndexed(), "image8->isIndexed()");
  });

  TEST("isTrue()", []() {
    auto image24 = Image::make(24, {16, 16}, 1);
    ASSERT(image24->isTrue(), "image24->isTrue()");

    auto image32 = Image::make(32, {16, 16}, 1);
    ASSERT(image32->isTrue(), "image32->isTrue()");
  });

  TEST("getBytesPerFrame()", []() {
    // (2 * 2) / 2 = 2
    auto image4 = Image::make(4, {2, 2}, 1);
    ASSERT(image4->getBytesPerFrame() == 2, "image4->getBytesPerFrame() == 2");

    // (2 * 2) / 1 = 4
    auto image8 = Image::make(8, {2, 2}, 1);
    ASSERT(image8->getBytesPerFrame() == 4, "image8->getBytesPerFrame() == 4");

    // (2 * 2) * 3 = 12
    auto image24 = Image::make(24, {2, 2}, 1);
    ASSERT(
        image24->getBytesPerFrame() == 12, "image24->getBytesPerFrame() == 12"
    );

    // (2 * 2) * 4 = 16
    auto image32 = Image::make(32, {2, 2}, 1);
    ASSERT(
        image32->getBytesPerFrame() == 16, "image32->getBytesPerFrame() == 16"
    );
  });

  TEST("pixels (4bpp indexed)", []() {
    auto image = Image::make(4, {2, 2}, 2);
    uint8_t frame1[] = {0 | (1 << 4), 2 | (3 << 4)};
    uint8_t frame2[] = {4 | (5 << 4), 6 | (7 << 4)};
    image->setFrameData(0, frame1);
    image->setFrameData(1, frame2);

    ASSERT_EQL(image->getPixelValueAt(0, 0, 0), 0, "(0; 0, 0)");
    ASSERT_EQL(image->getPixelValueAt(0, 1, 0), 1, "(0; 1, 0)");
    ASSERT_EQL(image->getPixelValueAt(0, 0, 1), 2, "(0; 0, 1)");
    ASSERT_EQL(image->getPixelValueAt(0, 1, 1), 3, "(0; 1, 1)");

    ASSERT_EQL(image->getPixelValueAt(1, 0, 0), 4, "(1; 0, 0)");
    ASSERT_EQL(image->getPixelValueAt(1, 1, 0), 5, "(1; 1, 0)");
    ASSERT_EQL(image->getPixelValueAt(1, 0, 1), 6, "(1; 0, 1)");
    ASSERT_EQL(image->getPixelValueAt(1, 1, 1), 7, "(1; 1, 1)");
  });

  TEST("pixels (8bpp indexed)", []() {
    auto image = Image::make(8, {2, 2}, 2);
    uint8_t frame1[] = {0, 1, 2, 3};
    uint8_t frame2[] = {4, 5, 6, 7};
    image->setFrameData(0, frame1);
    image->setFrameData(1, frame2);

    ASSERT_EQL(image->getPixelValueAt(0, 0, 0), 0, "(0; 0, 0)");
    ASSERT_EQL(image->getPixelValueAt(0, 1, 0), 1, "(0; 1, 0)");
    ASSERT_EQL(image->getPixelValueAt(0, 0, 1), 2, "(0; 0, 1)");
    ASSERT_EQL(image->getPixelValueAt(0, 1, 1), 3, "(0; 1, 1)");

    ASSERT_EQL(image->getPixelValueAt(1, 0, 0), 4, "(1; 0, 0)");
    ASSERT_EQL(image->getPixelValueAt(1, 1, 0), 5, "(1; 1, 0)");
    ASSERT_EQL(image->getPixelValueAt(1, 0, 1), 6, "(1; 0, 1)");
    ASSERT_EQL(image->getPixelValueAt(1, 1, 1), 7, "(1; 1, 1)");
  });

  TEST("pixels (24bpp true)", []() {
    auto image = Image::make(24, {2, 2}, 1);
    uint8_t frame1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    image->setFrameData(0, frame1);

    ASSERT_EQL(image->getPixelColorAt(0, 0, 0).reduceRed(8), 0, "(0; 0, 0) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 0).reduceGreen(8), 1, "(0; 0, 0) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 0).reduceBlue(8), 2, "(0; 0, 0) blue"
    );

    ASSERT_EQL(image->getPixelColorAt(0, 1, 0).reduceRed(8), 3, "(0; 1, 0) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 0).reduceGreen(8), 4, "(0; 1, 0) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 0).reduceBlue(8), 5, "(0; 1, 0) blue"
    );

    ASSERT_EQL(image->getPixelColorAt(0, 0, 1).reduceRed(8), 6, "(0; 0, 1) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 1).reduceGreen(8), 7, "(0; 0, 1) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 1).reduceBlue(8), 8, "(0; 0, 1) blue"
    );

    ASSERT_EQL(image->getPixelColorAt(0, 1, 1).reduceRed(8), 9, "(0; 1, 1) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceGreen(8), 10, "(0; 1, 1) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceBlue(8), 11, "(0; 1, 1) blue"
    );
  });

  TEST("pixels (32bpp true)", []() {
    auto image = Image::make(32, {2, 2}, 1);
    uint8_t frame1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    image->setFrameData(0, frame1);

    ASSERT_EQL(image->getPixelColorAt(0, 0, 0).reduceRed(8), 0, "(0; 0, 0) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 0).reduceGreen(8), 1, "(0; 0, 0) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 0).reduceBlue(8), 2, "(0; 0, 0) blue"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 0).reduceAlpha(8), 3, "(0; 0, 0) alpha"
    );

    ASSERT_EQL(image->getPixelColorAt(0, 1, 0).reduceRed(8), 4, "(0; 1, 0) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 0).reduceGreen(8), 5, "(0; 1, 0) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 0).reduceBlue(8), 6, "(0; 1, 0) blue"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 0).reduceAlpha(8), 7, "(0; 1, 0) alpha"
    );

    ASSERT_EQL(image->getPixelColorAt(0, 0, 1).reduceRed(8), 8, "(0; 0, 1) red");
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 1).reduceGreen(8), 9, "(0; 0, 1) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 1).reduceBlue(8), 10, "(0; 0, 1) blue"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 0, 1).reduceAlpha(8), 11, "(0; 0, 1) alpha"
    );

    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceRed(8), 12, "(0; 1, 1) red"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceGreen(8), 13, "(0; 1, 1) green"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceBlue(8), 14, "(0; 1, 1) blue"
    );
    ASSERT_EQL(
        image->getPixelColorAt(0, 1, 1).reduceAlpha(8), 15, "(0; 1, 1) alpha"
    );
  });

  TEST("toTrue", []() {
    auto bitmap = Image::make(4, {2, 2}, 1);
    uint8_t frame1[] = {0 | (1 << 4), 2 | (3 << 4)};
    bitmap->setFrameData(0, frame1);

    auto palette = std::make_shared<Palette>();
    *palette = {
        Color(1.f, 0.f, 0.f), Color(0.f, 1.f, 0.f), Color(0.f, 0.f, 1.f),
        Color(1.f, 1.f, 0.f)};

    auto trueImage = bitmap->toTrue(palette);

    ASSERT_EQL(
        trueImage->getPixelColorAt(0, 0, 0).reduceRed(8), 255, "(0; 0, 0)"
    );
    ASSERT_EQL(
        trueImage->getPixelColorAt(0, 1, 0).reduceGreen(8), 255, "(0; 1, 0)"
    );
    ASSERT_EQL(
        trueImage->getPixelColorAt(0, 0, 1).reduceBlue(8), 255, "(0; 0, 1)"
    );
    ASSERT_EQL(
        trueImage->getPixelColorAt(0, 1, 1).reduceRed(8), 255, "(0; 1, 1) red"
    );
    ASSERT_EQL(
        trueImage->getPixelColorAt(0, 1, 1).reduceGreen(8), 255,
        "(0; 1, 1) green"
    );
  });

  return runTests();
}
