#pragma once

#include <functional>
#include <memory>

namespace Luna {
  /**
   * @brief Execute callbacks in an interval.
   */
  class IntervalManager {
    public:
    using ElapsedTime = float;
    using Callback = std::function<void(ElapsedTime)>;

    IntervalManager();
    ~IntervalManager();

    /**
     * @brief Execute \c callback at most \c ratePerSecond.
     */
    void addInterval(int ratePerSecond, Callback callback);

    /**
     * @brief Execute \c callback every time \c executePendingIntervals() is
     * called.
     *
     * This is usually in vsync.
     */
    void addAlways(Callback callback);

    /**
     * @brief Execute pending intervals.
     */
    void executePendingIntervals();

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
