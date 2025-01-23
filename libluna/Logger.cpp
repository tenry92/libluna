#include <libluna/Logger.hpp>

#include <iostream>
#include <mutex>

#include <libluna/Console.hpp>

using namespace Luna;
using namespace Luna::Console;
using Luna::Logger;

#if !N64 && !NDS
static std::mutex gLogMutex;
thread_local String gThreadIdentifier;
#endif

void Logger::log(LogLevel pLevel, const String& pMessage) {
#if !N64 && !NDS
  std::lock_guard<std::mutex> lock(gLogMutex);

  if (!gThreadIdentifier.isEmpty()) {
    Console::write("[{}] ", gThreadIdentifier.c_str());
  }
#endif

  switch (pLevel) {
  case kDebug:
    setColor(purple);
    Console::write("debug:   ");
    break;
  case kVerbose:
    setColor(cyan);
    Console::write("verbose: ");
    break;
  case kInfo:
    setColor(blue);
    Console::write("info:    ");
    break;
  case kWarn:
    setColor(yellow);
    Console::write("warn:    ");
    break;
  case kError:
    setColor(red);
    Console::write("error:   ");
    break;
  }

  resetColor();

  Console::writeLine(pMessage);
}

void Logger::setThreadIdentifier([[maybe_unused]] const String& pThreadId) {
#if !N64 && !NDS
  gThreadIdentifier = pThreadId;
#endif
}

Logger& Logger::getInstance() {
  static Logger* logger = new Logger();

  return *logger;
}
