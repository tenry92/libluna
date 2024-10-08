#pragma once

#ifdef N64
#include <libdragon.h>
#else
#include <chrono>
#endif

namespace Luna::Clock {
#ifdef N64
  using TimePoint = long long;
#else
  using TimePoint = std::chrono::steady_clock::time_point;
#endif
  inline TimePoint now() {
#ifdef N64
    return timer_ticks();
#else
    return std::chrono::steady_clock::now();
#endif
  }

  inline void init() {
#ifdef N64
    timer_init();
#endif
  }

  inline double timeSpan(TimePoint start, TimePoint end) {
#ifdef N64
    return static_cast<double>(end - start) / TICKS_PER_SECOND;
#else
    return std::chrono::duration<double>(end - start).count();
#endif
  }
} // namespace Luna::Clock
