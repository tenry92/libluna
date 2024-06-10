#include <libluna/Platform.hpp>

using namespace Luna;

std::vector<String> Platform::getCompilerInfo() {
  std::vector<String> info;

#ifdef _MSC_VER
  info.emplace_back(String("compiled with MSVC version {}").format(_MSC_VER));
#endif

#ifdef __GNUC__
  info.emplace_back(String("compiled with GNU C compatible compiler version {}")
                        .format(__GNUC__));
#endif

#ifdef __clang__
  info.emplace_back(String("compiled with clang compatible compiler version {}")
                        .format(__clang_version__));
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
        String("C++ version: C++{}").format(__cplusplus / 100 - 2000L)
    );
  } else {
    info.emplace_back(String("C++ version: legacy ({})").format(__cplusplus));
  }
#endif

  return info;
}
