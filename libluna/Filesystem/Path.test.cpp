#include <libluna/Filesystem/Path.hpp>
#include <libluna/Test.hpp>

using namespace Luna::Filesystem;

int main(int, char **) {
  TEST("basic raw paths", []() {
    Path usrPath("/usr");
    ASSERT_EQL(usrPath.getRawPath().s_str(), "/usr", "");

    Path libPath("/usr/lib");
    ASSERT_EQL(libPath.getRawPath().s_str(), "/usr/lib", "");

    Path romfsRootPath("romfs:");
    ASSERT_EQL(romfsRootPath.getRawPath().s_str(), "romfs:", "romfs:");

    Path romfsAssetsPath("romfs:/assets");
    ASSERT_EQL(
        romfsAssetsPath.getRawPath().s_str(), "romfs:/assets", "romfs:/assets"
    );
  });

  TEST("cd", []() {
    Path usrPath("/usr");
    auto usrLocalPath = usrPath.cd("local");
    ASSERT_EQL(usrLocalPath.getRawPath().s_str(), "/usr/local", "");

    auto usrLocalIncludePath = usrPath.cd("local", "include");
    ASSERT_EQL(
        usrLocalIncludePath.getRawPath().s_str(), "/usr/local/include", ""
    );

    auto usrLocalIncludePath2 = usrPath.cd("local/include");
    ASSERT_EQL(
        usrLocalIncludePath2.getRawPath().s_str(), "/usr/local/include", ""
    );

    auto rootPath = usrPath.cd("/root");
    ASSERT_EQL(rootPath.getRawPath().s_str(), "/root", "");

    Path romfsRootPath("romfs:");
    ASSERT_EQL(
        romfsRootPath.cd("assets").getRawPath().s_str(), "romfs:/assets",
        "romfs:/assets"
    );
  });

  TEST("up", []() {
    Path basePath("/usr/local/include");

    auto localPath = basePath.up();
    ASSERT_EQL(localPath.getRawPath().s_str(), "/usr/local", "");

    auto usrPath = basePath.up(2);
    ASSERT_EQL(usrPath.getRawPath().s_str(), "/usr", "");

    Path romfsAssetsPath("romfs:/assets");
    auto romfsAssetsUpPath = romfsAssetsPath.up();
    ASSERT_EQL(romfsAssetsUpPath.getRawPath().s_str(), "romfs:", "romfs:");
  });

  return runTests();
}