#include <libluna/Console.hpp>
#include <libluna/System.hpp>

#ifdef __NDS__
#include <nds.h>
#endif

using namespace Luna;

int main(int argc, char** argv) {
  Console::init();

  Console::writeLine("Arguments: {}", argc);

  for (int i = 0; i < argc; ++i) {
    Console::writeLine("  #{}: {}", i, argv[i]);
  }

  Console::writeLine("");

  Console::writeLine("Processors: {}", System::getProcessorCount());
  Console::writeLine(
    "Physical Mem Usage: {} kB", System::getPhysicalMemoryUsage() / 1024
  );
  Console::writeLine(
    "Virtual Mem Usage: {} kB", System::getVirtualMemoryUsage() / 1024
  );
  Console::writeLine(
    "Total Memory: {} MiB", System::getTotalMemorySize() / 1024 / 1024
  );

#ifdef N64
  Console::writeLine("Total Heap: {} kB", System::getTotalHeapSize() / 1024);
  Console::writeLine("Used Heap: {} kB", System::getUsedHeapSize() / 1024);
#endif

#ifdef N64
  while (true) {
    Console::update();
  }
#endif

#ifdef __NDS__
  while (pmMainLoop()) {
    swiWaitForVBlank();
  }
#endif

  return 0;
}
