#include <string>
#include <vector>

#include <libluna/Application.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Test.hpp>

using namespace Luna;
using namespace Luna::Filesystem;

static const char *assetsPath = "tests/assets";

int main(int, char **) {
  TEST("read full text file", []() {
    Application app(0, nullptr);
    app.setAssetsPath(assetsPath);

    const char *filename = "test.txt";
    ResourceReader resourceReader(filename);

    ASSERT_EQL(resourceReader.getSize(), 36, "getSize()");

    std::vector<std::uint8_t> buffer(resourceReader.getSize());
    resourceReader.read(
        buffer.data(), sizeof(std::uint8_t), resourceReader.getSize()
    );
    std::string contents(
        reinterpret_cast<const char *>(buffer.data()), buffer.size()
    );
    ASSERT_EQL(
        contents, "This text is read from a text file.\n", "file contents"
    );
  });

  return runTests();
}
