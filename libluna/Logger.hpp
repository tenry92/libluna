#pragma once

#include <libluna/String.hpp>

namespace Luna {
  /**
   * @brief Use for writing debug, info and error messages to the console and
   * possibly other outputs.
   *
   * There are the following severities of logs available:
   *
   * - debug: information intended for developers
   * - verbose: extra information on what the system is doing; disabled by
   * default
   * - info: general information on what is happening
   * - warn: non-critical incidents and exceptions
   * - error: critical incidends and exceptions; application should stop
   */

  /**
   * @brief Logging class.
   */
  class Logger {
    public:
    enum LogLevel { kDebug, kVerbose, kInfo, kWarn, kError };

    void log(LogLevel pLevel, const String &pMessage);

    /**
     * @brief Set identifier for current thread.
     *
     * The thread identifier will be added to all log messages emited from this
     * thread.
     */
    void setThreadIdentifier(const String &pThreadId);

    inline void debug(const String &pMessage) { log(kDebug, pMessage); }

    inline void verbose(const String &pMessage) { log(kVerbose, pMessage); }

    inline void info(const String &pMessage) { log(kInfo, pMessage); }

    inline void warn(const String &pMessage) { log(kWarn, pMessage); }

    inline void error(const String &pMessage) { log(kError, pMessage); }

    template <typename... ArgTypes>
    void log(LogLevel pLevel, const String &pMessage, ArgTypes... pArgs) {
      log(pLevel, pMessage.format(pArgs...));
    }

    template <typename... ArgTypes>
    void debug(const String &pMessage, ArgTypes... pArgs) {
      debug(pMessage.format(pArgs...));
    }

    template <typename... ArgTypes>
    void verbose(const String &pMessage, ArgTypes... pArgs) {
      verbose(pMessage.format(pArgs...));
    }

    template <typename... ArgTypes>
    void info(const String &pMessage, ArgTypes... pArgs) {
      info(pMessage.format(pArgs...));
    }

    template <typename... ArgTypes>
    void warn(const String &pMessage, ArgTypes... pArgs) {
      warn(pMessage.format(pArgs...));
    }

    template <typename... ArgTypes>
    void error(const String &pMessage, ArgTypes... pArgs) {
      error(pMessage.format(pArgs...));
    }

    static Logger &getInstance();

    private:
    Logger() = default;

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;
  };

  template <typename... ArgTypes>
  inline void logDebug(const String &pMessage, ArgTypes... pArgs) {
    Logger::getInstance().debug(pMessage.format(pArgs...));
  }

  template <typename... ArgTypes>
  inline void logVerbose(const String &pMessage, ArgTypes... pArgs) {
    Logger::getInstance().verbose(pMessage.format(pArgs...));
  }

  template <typename... ArgTypes>
  inline void logInfo(const String &pMessage, ArgTypes... pArgs) {
    Logger::getInstance().info(pMessage.format(pArgs...));
  }

  template <typename... ArgTypes>
  inline void logWarn(const String &pMessage, ArgTypes... pArgs) {
    Logger::getInstance().warn(pMessage.format(pArgs...));
  }

  template <typename... ArgTypes>
  inline void logError(const String &pMessage, ArgTypes... pArgs) {
    Logger::getInstance().error(pMessage.format(pArgs...));
  }

  /** @} */
} // namespace Luna
