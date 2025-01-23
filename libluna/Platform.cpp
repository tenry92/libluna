#include <libluna/Platform.hpp>

#include <fmt/format.h>

using namespace Luna;

std::vector<String> Platform::getCompilerInfo() {
  std::vector<String> info;

#ifdef _MSC_VER
  info.emplace_back(fmt::format("compiled with MSVC version {}", _MSC_VER));
#endif

#ifdef __GNUC__
  info.emplace_back(
    fmt::format("compiled with GNU C compatible compiler version {}", __GNUC__)
  );
#endif

#ifdef __clang__
  info.emplace_back(fmt::format(
    "compiled with clang compatible compiler version {}", __clang_version__
  ));
#endif

#ifdef __MINGW32__
  info.emplace_back(String("compiled with MinGW compiler"));
#endif

#ifdef __MINGW64__
  info.emplace_back(String("compiled with MinGW-w64 64-bit compiler"));
#endif

#ifdef __cplusplus
  if constexpr (__cplusplus >= 201103L) {
    info.emplace_back(
      fmt::format("C++ version: C++{}", __cplusplus / 100 - 2000L)
    );
  } else {
    info.emplace_back(fmt::format("C++ version: legacy ({})", __cplusplus));
  }
#endif

  return info;
}
