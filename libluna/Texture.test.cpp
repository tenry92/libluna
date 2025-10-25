#include <libluna/Texture.hpp>
#include <libluna/Test.hpp>

using namespace std;
using namespace Luna;

int main(int, char**) {
  TEST("empty texture", []() {
    auto texture = Texture();
    ASSERT(texture.getSize().width == 0, "getWidth() == 0");
    ASSERT(texture.getSize().height == 0, "getHeight() == 0");
  });

  TEST("texture size", []() {
    auto texture = Texture(4, {16, 32});
    ASSERT(texture.getBitsPerPixel() == 4, "getBitsPerPixel() == 4");
    ASSERT(texture.getSize().width == 16, "getWidth() == 16");
    ASSERT(texture.getSize().height == 32, "getHeight() == 32");
  });

  TEST("getByteCount()", []() {
    // (2 * 2) / 2 = 2
    auto texture4 = Texture(4, {2, 2});
    ASSERT(texture4.getByteCount() == 2, "texture4->getByteCount() == 2");

    // (2 * 2) / 1 = 4
    auto texture8 = Texture(8, {2, 2});
    ASSERT(texture8.getByteCount() == 4, "texture8->getByteCount() == 4");

    // (2 * 2) * 3 = 12
    auto texture24 = Texture(24, {2, 2});
    ASSERT(texture24.getByteCount() == 12, "texture24->getByteCount() == 12");

    // (2 * 2) * 4 = 16
    auto texture32 = Texture(32, {2, 2});
    ASSERT(texture32.getByteCount() == 16, "texture32->getByteCount() == 16");
  });

  TEST("pixels (4bpp indexed)", []() {
    auto texture = Texture(4, {2, 2});
    uint8_t frame[] = {0 | (1 << 4), 2 | (3 << 4)};
    memcpy(texture.getData(), frame, texture.getByteCount());

    ASSERT_EQL(texture.getNibbleAt(0, 0), 0, "(0, 0)");
    ASSERT_EQL(texture.getNibbleAt(1, 0), 1, "(1, 0)");
    ASSERT_EQL(texture.getNibbleAt(0, 1), 2, "(0, 1)");
    ASSERT_EQL(texture.getNibbleAt(1, 1), 3, "(1, 1)");
  });

  TEST("pixels (8bpp indexed)", []() {
    auto texture = Texture(8, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3};
    memcpy(texture.getData(), frame, texture.getByteCount());

    ASSERT_EQL(texture.byteAt(0, 0), 0, "(0, 0)");
    ASSERT_EQL(texture.byteAt(1, 0), 1, "(1, 0)");
    ASSERT_EQL(texture.byteAt(0, 1), 2, "(0, 1)");
    ASSERT_EQL(texture.byteAt(1, 1), 3, "(1, 1)");
  });

  TEST("pixels (24bpp true)", []() {
    auto texture = Texture(24, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    memcpy(texture.getData(), frame, texture.getByteCount());

    ASSERT_EQL(texture.rgb24At(0, 0).red, 0, "(0, 0) red");
    ASSERT_EQL(texture.rgb24At(0, 0).green, 1, "(0, 0) green");
    ASSERT_EQL(texture.rgb24At(0, 0).blue, 2, "(0, 0) blue");

    ASSERT_EQL(texture.rgb24At(1, 0).red, 3, "(1, 0) red");
    ASSERT_EQL(texture.rgb24At(1, 0).green, 4, "(1, 0) green");
    ASSERT_EQL(texture.rgb24At(1, 0).blue, 5, "(1, 0) blue");

    ASSERT_EQL(texture.rgb24At(0, 1).red, 6, "(0, 1) red");
    ASSERT_EQL(texture.rgb24At(0, 1).green, 7, "(0, 1) green");
    ASSERT_EQL(texture.rgb24At(0, 1).blue, 8, "(0, 1) blue");

    ASSERT_EQL(texture.rgb24At(1, 1).red, 9, "(1, 1) red");
    ASSERT_EQL(texture.rgb24At(1, 1).green, 10, "(1, 1) green");
    ASSERT_EQL(texture.rgb24At(1, 1).blue, 11, "(1, 1) blue");
  });

  TEST("pixels (32bpp true)", []() {
    auto texture = Texture(32, {2, 2});
    uint8_t frame[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    memcpy(texture.getData(), frame, texture.getByteCount());

    ASSERT_EQL(texture.rgb32At(0, 0).red, 0, "(0, 0) red");
    ASSERT_EQL(texture.rgb32At(0, 0).green, 1, "(0, 0) green");
    ASSERT_EQL(texture.rgb32At(0, 0).blue, 2, "(0; 0, 0) blue");
    ASSERT_EQL(texture.rgb32At(0, 0).alpha, 3, "(0, 0) alpha");

    ASSERT_EQL(texture.rgb32At(1, 0).red, 4, "(1, 0) red");
    ASSERT_EQL(texture.rgb32At(1, 0).green, 5, "(1, 0) green");
    ASSERT_EQL(texture.rgb32At(1, 0).blue, 6, "(0; 1, 0) blue");
    ASSERT_EQL(texture.rgb32At(1, 0).alpha, 7, "(1, 0) alpha");

    ASSERT_EQL(texture.rgb32At(0, 1).red, 8, "(0, 1) red");
    ASSERT_EQL(texture.rgb32At(0, 1).green, 9, "(0, 1) green");
    ASSERT_EQL(texture.rgb32At(0, 1).blue, 10, "(0; 0, 1) blue");
    ASSERT_EQL(texture.rgb32At(0, 1).alpha, 11, "(0, 1) alpha");

    ASSERT_EQL(texture.rgb32At(1, 1).red, 12, "(1, 1) red");
    ASSERT_EQL(texture.rgb32At(1, 1).green, 13, "(1, 1) green");
    ASSERT_EQL(texture.rgb32At(1, 1).blue, 14, "(0; 0, 1) blue");
    ASSERT_EQL(texture.rgb32At(1, 1).alpha, 15, "(1, 1) alpha");
  });

  return runTests();
}
