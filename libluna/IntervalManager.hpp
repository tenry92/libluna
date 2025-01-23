#pragma once

#include <functional>

#include <libluna/Clock.hpp>

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
    struct Interval {
      int ratePerSecond;
      IntervalManager::Callback callback;
      Clock::TimePoint nextExecution;
      Clock::TimePoint lastExecution;

      /**
       * @brief Check whether a is to be executed before b.
       */
      friend bool operator<(const Interval& a, const Interval& b) {
        return a.nextExecution > b.nextExecution;
      }
    };

    std::priority_queue<Interval> mIntervalQueue;
    std::list<Interval> mExecuteAlways;
  };
} // namespace Luna
