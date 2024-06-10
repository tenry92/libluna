#include <chrono>
#include <list>
#include <queue>

#include <libluna/IntervalManager.hpp>

using namespace Luna;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

struct Interval {
  int ratePerSecond;
  IntervalManager::Callback callback;
  TimePoint nextExecution;
  std::chrono::steady_clock::time_point lastExecution;

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
      ratePerSecond, callback, std::chrono::steady_clock::now(),
      std::chrono::steady_clock::now()});
}

void IntervalManager::addAlways(Callback callback) {
  mImpl->mExecuteAlways.push_back(Interval{
      0, callback, std::chrono::steady_clock::now(),
      std::chrono::steady_clock::now()});
}

void IntervalManager::executePendingIntervals() {
  auto now = std::chrono::steady_clock::now();

  for (auto &&currentInterval : mImpl->mExecuteAlways) {
    auto delta = now - currentInterval.lastExecution;
    currentInterval.lastExecution = now;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;

    currentInterval.callback(deltaSeconds);
  }

  while (!mImpl->mIntervalQueue.empty()) {
    auto currentInterval = mImpl->mIntervalQueue.top();

    if (currentInterval.nextExecution > now) {
      // no more intervals to be executed for now
      return;
    }

    auto delta = now - currentInterval.lastExecution;
    currentInterval.lastExecution = now;
    auto nanoSeconds = std::chrono::nanoseconds(delta).count();
    float deltaSeconds = static_cast<float>(nanoSeconds) / std::nano::den;

    // increase nextExecution until it's in the past
    while (currentInterval.nextExecution <= now) {
      currentInterval.nextExecution += std::chrono::nanoseconds(
          std::nano::den / currentInterval.ratePerSecond
      );
    }

    mImpl->mIntervalQueue.pop();
    mImpl->mIntervalQueue.emplace(currentInterval);
    currentInterval.callback(deltaSeconds);
  }
}
