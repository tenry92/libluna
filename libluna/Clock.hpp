#pragma once

#ifdef N64
#include <libdragon.h>
#else
#include <chrono>
#endif

/**
 * @brief High-resolution clock utilities.
 *
 * @ingroup system
 */
namespace Luna::Clock {
#ifdef N64
  using TimePoint = long long;
  using Duration = long long;
#else
  using TimePoint = std::chrono::steady_clock::time_point;
  using Duration = std::chrono::steady_clock::duration;
#endif
  inline TimePoint now() {
#ifdef N64
    return timer_ticks();
#else
    return std::chrono::steady_clock::now();
#endif
  }

  /**
   * @brief Initializes the clock system.
   *
   * This function is used internally by Luna.
   *
   * @internal
   */
  inline void init() {
#ifdef N64
    timer_init();
#endif
  }

  /**
   * @brief Calculates the time span in seconds between two time points.
   */
  inline double timeSpan(TimePoint start, TimePoint end) {
#ifdef N64
    return static_cast<double>(end - start) / TICKS_PER_SECOND;
#else
    return std::chrono::duration<double>(end - start).count();
#endif
  }
} // namespace Luna::Clock
