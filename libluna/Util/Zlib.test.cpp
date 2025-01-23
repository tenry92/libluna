#include <iostream>
#include <string>

#include <libluna/MemoryReader.hpp>
#include <libluna/Test.hpp>
#include <libluna/Util/Zlib.hpp>

using namespace Luna;
using namespace Luna::Util;

static uint8_t data[] = {0x78, 0x9c, 0x0b, 0xc9, 0xc8, 0x2c, 0x56, 0x28,
                         0x49, 0xad, 0x28, 0x51, 0xc8, 0x2c, 0x56, 0x28,
                         0x4a, 0x4d, 0x4c, 0x51, 0x48, 0x2b, 0xca, 0xcf,
                         0x55, 0x48, 0x84, 0x88, 0xa5, 0x65, 0xe6, 0xa4,
                         0xea, 0x71, 0x01, 0x00, 0xee, 0x0b, 0x0c, 0x68};

static const char *expectedResult = "This text is read from a text file.\n";

int main(int, char **) {
  TEST("inflate", []() {
    auto reader = std::make_unique<MemoryReader>(data, sizeof(data));
    Zlib zlib(std::move(reader));
    auto result = zlib.inflate();

    std::string decodedString(
      reinterpret_cast<const char *>(result.data()), result.size()
    );

    ASSERT_EQL(decodedString, std::string(expectedResult), "");
  });

  return runTests();
}
