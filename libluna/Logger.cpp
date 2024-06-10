#include <libluna/Logger.hpp>

#include <iostream>
#include <mutex>

#include <libluna/Terminal.hpp>

using namespace Luna;
using namespace Luna::Terminal;
using Luna::Logger;

static std::mutex gLogMutex;
thread_local String gThreadIdentifier;

void Logger::log(LogLevel pLevel, const String &pMessage) {
  std::lock_guard<std::mutex> lock(gLogMutex);

  if (!gThreadIdentifier.isEmpty()) {
    std::cout << '[' << gThreadIdentifier.s_str() << "] ";
  }

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

  resetColor();

  std::cout << pMessage.s_str() << std::endl;
}

void Logger::setThreadIdentifier(const String &pThreadId) {
  gThreadIdentifier = pThreadId;
}

Logger &Logger::getInstance() {
  static Logger *logger = new Logger();

  return *logger;
}
