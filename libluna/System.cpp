#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#include <thread>

#include <libluna/System.hpp>

using namespace Luna;

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
