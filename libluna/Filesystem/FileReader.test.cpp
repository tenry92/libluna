#include <string>
#include <vector>

#ifdef N64
#include <libdragon.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/Filesystem/Path.hpp>
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
#ifdef __SWITCH__
  if (R_FAILED(romfsInit())) {
    consoleInit(NULL);
    printf("romfsInit error\n");
    while (appletMainLoop()) {
      consoleUpdate(NULL);
    }
  }
#endif
#ifdef N64
  dfs_init(DFS_DEFAULT_LOCATION);
#endif

  TEST("read full text file", []() {
    Application app(0, nullptr);
    app.setAssetsPath(assetsPath);

    auto fileReader = FileReader::make(app.getAssetsPath().cd("test.txt"));

    ASSERT_EQL(fileReader->getSize(), 36, "getSize()");

    std::vector<std::uint8_t> buffer(fileReader->getSize());
    fileReader->read(
        buffer.data(), sizeof(std::uint8_t), fileReader->getSize()
    );
    std::string contents(
        reinterpret_cast<const char *>(buffer.data()), buffer.size()
    );
    ASSERT_EQL(
        contents, "This text is read from a text file.\n", "file contents"
    );
  });

  TEST("seek way off the boundary", []() {
    Application app(0, nullptr);
    app.setAssetsPath(assetsPath);

    auto fileReader = FileReader::make(app.getAssetsPath().cd("test.txt"));

    fileReader->seek(1000000);
  });

  return runTests();
}
