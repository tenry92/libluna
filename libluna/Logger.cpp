#include <libluna/Logger.hpp>

#include <iostream>
#include <mutex>

#include <libluna/Console.hpp>

using namespace Luna;
using namespace Luna::Console;
using Luna::Logger;

#ifndef N64
static std::mutex gLogMutex;
thread_local String gThreadIdentifier;
#endif

void Logger::log(LogLevel pLevel, const String &pMessage) {
#ifndef N64
  std::lock_guard<std::mutex> lock(gLogMutex);

  if (!gThreadIdentifier.isEmpty()) {
    std::cout << '[' << gThreadIdentifier.s_str() << "] ";
  }
#endif

#ifdef N64
  switch (pLevel) {
  case kDebug:
    setColor(purple);
    printf("debug:   ");
    break;
  case kVerbose:
    setColor(cyan);
    printf("verbose: ");
    break;
  case kInfo:
    setColor(blue);
    printf("info:    ");
    break;
  case kWarn:
    setColor(yellow);
    printf("warn:    ");
    break;
  case kError:
    setColor(red);
    printf("error:   ");
    break;
  }
#else
  switch (pLevel) {
  case kDebug:
    setColor(purple);
    std::cout << "debug:   ";
    break;
  case kVerbose:
    setColor(cyan);
    std::cout << "verbose: ";
    break;
  case kInfo:
    setColor(blue);
    std::cout << "info:    ";
    break;
  case kWarn:
    setColor(yellow);
    std::cout << "warn:    ";
    break;
  case kError:
    setColor(red);
    std::cout << "error:   ";
    break;
  }
#endif

  resetColor();

#ifdef N64
  printf("%s\n", pMessage.c_str());
#else
  std::cout << pMessage.s_str() << std::endl;
#endif
}

void Logger::setThreadIdentifier([[maybe_unused]] const String &pThreadId) {
#ifndef N64
  gThreadIdentifier = pThreadId;
#endif
}

Logger &Logger::getInstance() {
  static Logger *logger = new Logger();

  return *logger;
}
