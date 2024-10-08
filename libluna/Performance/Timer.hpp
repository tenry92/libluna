#pragma once

#include <libluna/Clock.hpp>

namespace Luna::Performance {
  /**
   * \brief Measure time between two locations of code.
   */
  class Timer {
    public:
    Timer();
    ~Timer();

    /**
     * \brief Start timer.
     */
    void start();

    /**
     * \brief Get elapsed time since last \c start() or \c elapse() .
     *
     * \return Elapsed time in seconds.
     */
    double elapse();

    private:
    Clock::TimePoint lastUpdate;
  };
} // namespace Luna::Performance
