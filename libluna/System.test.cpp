#include <libluna/Console.hpp>
#include <libluna/System.hpp>

using namespace Luna;

int main(int argc, char **argv) {
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
  while (true) {
    Console::update();
  }
#endif

  return 0;
}
