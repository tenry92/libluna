#include <list>
#include <queue>

#include <libluna/IntervalManager.hpp>
#include <libluna/Clock.hpp>

using namespace Luna;

struct Interval {
  int ratePerSecond;
  IntervalManager::Callback callback;
  Clock::TimePoint nextExecution;
  Clock::TimePoint lastExecution;

  /**
   * @brief Check whether a is to be executed before b.
   */
  friend bool operator<(const Interval &a, const Interval &b) {
    return a.nextExecution > b.nextExecution;
  }
};

class IntervalManager::impl {
  public:
  std::priority_queue<Interval> mIntervalQueue;
  std::list<Interval> mExecuteAlways;
};

IntervalManager::IntervalManager() : mImpl{std::make_unique<impl>()} {}

IntervalManager::~IntervalManager() = default;

void IntervalManager::addInterval(int ratePerSecond, Callback callback) {
  mImpl->mIntervalQueue.emplace(Interval{
      ratePerSecond, callback, Clock::now(),
      Clock::now()});
}

void IntervalManager::addAlways(Callback callback) {
  mImpl->mExecuteAlways.push_back(Interval{
      0, callback, Clock::now(),
      Clock::now()});
}

void IntervalManager::executePendingIntervals() {
  auto now = Clock::now();

  for (auto &&currentInterval : mImpl->mExecuteAlways) {
#ifdef NDS
    float deltaSeconds = 1.0f / 60.0f;
#elif defined N64
    float deltaSeconds = static_cast<float>(Clock::timeSpan(currentInterval.lastExecution, now));
#else
    auto delta = now - currentInterval.lastExecution;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;
#endif

    currentInterval.lastExecution = now;

    currentInterval.callback(deltaSeconds);
  }

  while (!mImpl->mIntervalQueue.empty()) {
    auto currentInterval = mImpl->mIntervalQueue.top();

#ifndef NDS
    if (currentInterval.nextExecution > now) {
      // no more intervals to be executed for now
      return;
    }
#endif

#ifdef NDS
    float deltaSeconds = 1.0f / 60.0f;
#elif defined N64
    float deltaSeconds = static_cast<float>(Clock::timeSpan(currentInterval.lastExecution, now));
#else
    auto delta = now - currentInterval.lastExecution;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;
#endif

    currentInterval.lastExecution = now;

#ifndef NDS
    // increase nextExecution until it's in the past
    while (currentInterval.nextExecution <= now) {
#ifdef N64
      currentInterval.nextExecution += TICKS_PER_SECOND / currentInterval.ratePerSecond;
#else
      currentInterval.nextExecution += std::chrono::nanoseconds(
          std::nano::den / currentInterval.ratePerSecond
      );
#endif
    }
#endif

    mImpl->mIntervalQueue.pop();
    mImpl->mIntervalQueue.emplace(currentInterval);
    currentInterval.callback(deltaSeconds);

#ifdef NDS
    break;
#endif
  }
}
