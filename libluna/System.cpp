#include <stdio.h>  // fgets, fopen
#include <string.h> // strncmp, strlen

#ifdef _WIN32
#include <windows.h> // GetCurrentProcess, GetProcessMemoryInfo, GetSystemInfo
#include <psapi.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#include <filesystem>
#include <thread>

#include <libluna/Application.hpp>
#include <libluna/System.hpp>

using namespace Luna;
namespace fs = std::filesystem;

unsigned int System::getProcessorCount() {
#ifdef __MINGW32__
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return std::thread::hardware_concurrency();
#endif
}

unsigned long System::getPhysicalMemoryUsage() {
#ifdef __linux__
  FILE *fp = fopen("/proc/self/status", "rt");
  unsigned long result = 0;
  char line[128];

  while (fgets(line, 128, fp) != nullptr) {
    if (strncmp(line, "VmRSS:", 6) == 0) {
      auto i = strlen(line);
      const char *p = line;
      while (*p < '0' || *p > '9') {
        p++;
      }
      line[i - 3] = '\0'; // remove " kB"
      result = static_cast<decltype(result)>(atol(p) * 1024);
      break;
    }
  }

  fclose(fp);
  return result;
#elif defined(_WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(
      GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)
  );
  return static_cast<unsigned long>(pmc.WorkingSetSize);
#else
  return 0;
#endif
}

unsigned long System::getVirtualMemoryUsage() {
#ifdef __linux__
  FILE *fp = fopen("/proc/self/status", "rt");
  unsigned long result = 0;
  char line[128];

  while (fgets(line, 128, fp) != nullptr) {
    if (strncmp(line, "VmSize:", 6) == 0) {
      auto i = strlen(line);
      const char *p = line;
      while (*p < '0' || *p > '9') {
        p++;
      }
      line[i - 3] = '\0'; // remove " kB"
      result = static_cast<decltype(result)>(atol(p) * 1024);
      break;
    }
  }

  fclose(fp);
  return result;
#elif defined(_WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(
      GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)
  );
  return static_cast<unsigned long>(pmc.PrivateUsage);
#else
  return 0;
#endif
}

unsigned long System::getTotalMemorySize() {
#ifdef N64
  return get_memory_size();
#else
  return 0;
#endif
}

std::list<String> System::getAssetFiles() {
  auto assetsPath =
      Application::getInstance()->getAssetsPath().getRawPath().s_str();
  std::list<String> list;

#ifdef N64
  dir_t dir;
  int result = dir_findfirst(assetsPath.c_str(), &dir);

  while (result == 0) {
    list.emplace_back(dir.d_name);
    result = dir_findnext(assetsPath.c_str(), &dir);
  }
#else
  if (fs::exists(assetsPath) && fs::is_directory(assetsPath)) {
    for (const auto &entry : fs::recursive_directory_iterator(assetsPath)) {
      list.emplace_back(entry.path().c_str());
    }
  }
#endif

  return list;
}
