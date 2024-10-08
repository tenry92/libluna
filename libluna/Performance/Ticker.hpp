#pragma once

#include <libluna/String.hpp>

#include <chrono>
#include <deque>
#include <map>

namespace Luna::Performance {
  /**
   * \brief Measure average ticks per second and tick duration.
   */
  class Ticker {
    public:
    Ticker(const String &name = "");
    ~Ticker();

    /**
     * \brief Do a tick.
     *
     * This should happen exactly once per cycle.
     *
     * Use a following \c measure() to measure the time elapsed between last
     * \c tick() and \c measure() calls.
     *
     * \par Example
     * \code{.cpp}
     * while (true) {
     *   ticker.tick();
     *   updateObjects();
     *   drawObjects();
     *   ticker.measure();
     *   auto averageTimePerTick = ticker.getTickDuration();
     *   auto ticksPerSecond = ticker.getTicksPerSecond();
     * }
     * \endcode
     */
    void tick();

    /**
     * \brief Measure time elapsed since last \c tick() call.
     *
     * Use \c getTickDuration() to get average elapsed time per tick.
     */
    void measure();

    /**
     * \brief Get average duration between \c tick() and \c measure() calls in
     * seconds.
     */
    float getTickDuration() const;

    /**
     * \brief Get average duration between two \c tick() calls.
     */
    float getTicksPerSecond() const;

    static Ticker *getTickerByName(const String &name);

    private:
    unsigned long mTickCount = 0;
    std::deque<std::chrono::steady_clock::time_point> mTicks;
    std::deque<std::chrono::steady_clock::duration> mTickTimes;
    String mName;
  };
} // namespace Luna::Performance
