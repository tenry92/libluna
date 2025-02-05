#include <string>

#include <libluna/Filesystem/Path.hpp>
#include <libluna/Test.hpp>

using namespace Luna::Filesystem;

namespace {
  std::string toString(const Path& path) {
    return std::string(path.getRawPath().c_str());
  }
} // namespace

int main(int, char**) {
  TEST("basic raw paths", []() {
    Path usrPath("/usr");
    ASSERT_EQL(toString(usrPath), "/usr", "");

    Path libPath("/usr/lib");
    ASSERT_EQL(toString(libPath), "/usr/lib", "");

    Path romfsRootPath("romfs:");
    ASSERT_EQL(toString(romfsRootPath), "romfs:", "romfs:");

    Path romfsAssetsPath("romfs:/assets");
    ASSERT_EQL(toString(romfsAssetsPath), "romfs:/assets", "romfs:/assets");
  });

  TEST("cd", []() {
    Path usrPath("/usr");
    auto usrLocalPath = usrPath.cd("local");
    ASSERT_EQL(toString(usrLocalPath), "/usr/local", "");

    auto usrLocalIncludePath = usrPath.cd("local", "include");
    ASSERT_EQL(toString(usrLocalIncludePath), "/usr/local/include", "");

    auto usrLocalIncludePath2 = usrPath.cd("local/include");
    ASSERT_EQL(toString(usrLocalIncludePath2), "/usr/local/include", "");

    auto rootPath = usrPath.cd("/root");
    ASSERT_EQL(toString(rootPath), "/root", "");

    Path romfsRootPath("romfs:");
    ASSERT_EQL(
      toString(romfsRootPath.cd("assets")), "romfs:/assets", "romfs:/assets"
    );
  });

  TEST("up", []() {
    Path basePath("/usr/local/include");

    auto localPath = basePath.up();
    ASSERT_EQL(toString(localPath), "/usr/local", "");

    auto usrPath = basePath.up(2);
    ASSERT_EQL(toString(usrPath), "/usr", "");

    Path romfsAssetsPath("romfs:/assets");
    auto romfsAssetsUpPath = romfsAssetsPath.up();
    ASSERT_EQL(toString(romfsAssetsUpPath), "romfs:", "romfs:");
  });

  return runTests();
}