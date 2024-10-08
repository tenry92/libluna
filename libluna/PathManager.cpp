#include <libluna/config.h>

#include <libluna/PathManager.hpp>

#ifdef _WIN32
#include <windows.h> // GetModuleFileNameW
#endif

#ifdef __linux__
#include <limits.h> // PATH_MAX
#include <unistd.h> // readlink
#endif

using namespace Luna;
using namespace Luna::Filesystem;

void PathManager::detectProgramPath() {
#ifdef _WIN32
  wchar_t selfPath[MAX_PATH] = {0};
  GetModuleFileNameW(nullptr, selfPath, MAX_PATH);
  mProgramPath = Path(selfPath);
#elif defined(__SWITCH__) || defined(__3DS__) || defined(NDS) || defined(N64)
  return;
#else
  char selfPath[PATH_MAX];
  auto length =
      static_cast<std::size_t>(readlink("/proc/self/exe", selfPath, PATH_MAX));

  if (length <= 0) {
    throw std::runtime_error("unable to read link at /proc/self/exe");
  }

  mProgramPath = Path(String(selfPath, length));
#endif
}

void PathManager::detectAssetsPath() {
#ifdef _NDS
  mAssetsPath = Path("/assets");
#elif defined(__3DS__) || defined(__SWITCH__)
  mAssetsPath = Path("romfs:/assets");
#elif defined(N64)
  mAssetsPath = Path("rom:/assets");
#else
  auto programPath = mProgramPath;
  if (programPath.up().basename() == "bin") {
    // UNIX style (bin/, share/assets/)
    mAssetsPath = programPath.up().cd("share/assets");
  } else {
    // Windows style (exe, data/assets/)
    mAssetsPath = programPath.up().cd("data/assets");
  }
#endif
}

PathManager::PathManager() {
  detectProgramPath();
  detectAssetsPath();
}

PathManager::~PathManager() = default;

Path PathManager::getProgramPath() const { return mProgramPath; }

Path PathManager::getAssetsPath() const { return mAssetsPath; }

void PathManager::setAssetsPath(Path assetsPath) {
  mAssetsPath = mProgramPath.up().cd(assetsPath);
}
