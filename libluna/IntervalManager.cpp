#include <list>
#include <queue>

#include <libluna/Clock.hpp>
#include <libluna/IntervalManager.hpp>

using namespace Luna;

IntervalManager::IntervalManager() = default;

IntervalManager::~IntervalManager() = default;

void IntervalManager::addInterval(int ratePerSecond, Callback callback) {
  mIntervalQueue.emplace(Interval{
    ratePerSecond, callback, Clock::now(), Clock::now()});
}

void IntervalManager::addAlways(Callback callback) {
  mExecuteAlways.push_back(Interval{0, callback, Clock::now(), Clock::now()});
}

void IntervalManager::executePendingIntervals() {
  auto now = Clock::now();

  for (auto&& currentInterval : mExecuteAlways) {
#ifdef __NDS__
    float deltaSeconds = 1.0f / 60.0f;
#elif defined N64
    float deltaSeconds =
      static_cast<float>(Clock::timeSpan(currentInterval.lastExecution, now));
#else
    auto delta = now - currentInterval.lastExecution;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;
#endif

    currentInterval.lastExecution = now;

    currentInterval.callback(deltaSeconds);
  }

  while (!mIntervalQueue.empty()) {
    auto currentInterval = mIntervalQueue.top();

#ifndef __NDS__
    if (currentInterval.nextExecution > now) {
      // no more intervals to be executed for now
      return;
    }
#endif

#ifdef __NDS__
    float deltaSeconds = 1.0f / 60.0f;
#elif defined N64
    float deltaSeconds =
      static_cast<float>(Clock::timeSpan(currentInterval.lastExecution, now));
#else
    auto delta = now - currentInterval.lastExecution;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;
#endif

    currentInterval.lastExecution = now;

#ifndef __NDS__
    // increase nextExecution until it's in the past
    while (currentInterval.nextExecution <= now) {
#ifdef N64
      currentInterval.nextExecution +=
        TICKS_PER_SECOND / currentInterval.ratePerSecond;
#else
      currentInterval.nextExecution += std::chrono::nanoseconds(
        std::nano::den / currentInterval.ratePerSecond
      );
#endif
    }
#endif

    mIntervalQueue.pop();
    mIntervalQueue.emplace(currentInterval);
    currentInterval.callback(deltaSeconds);

#ifdef __NDS__
    break;
#endif
  }
}
