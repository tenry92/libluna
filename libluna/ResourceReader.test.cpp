#include <string>
#include <vector>

#ifdef N64
#include <libdragon.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Test.hpp>

using namespace Luna;
using namespace Luna::Filesystem;

#ifdef __SWITCH__
static const char *assetsPath = "romfs:/assets";
#elif defined(N64)
static const char *assetsPath = "rom:/assets";
#else
static const char *assetsPath = "tests/assets";
#endif

int main(int, char **) {
  TEST("read full text file", []() {
    Application app(0, nullptr);
    app.setAssetsPath(assetsPath);

#ifdef N64
    dfs_init(DFS_DEFAULT_LOCATION);
#endif

    const char *filename = "test.txt";
    auto resourceReader = ResourceReader::make(filename);

    ASSERT_EQL(resourceReader->getSize(), 36, "getSize()");

    std::vector<std::uint8_t> buffer(resourceReader->getSize());
    resourceReader->read(
        buffer.data(), sizeof(std::uint8_t), resourceReader->getSize()
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
