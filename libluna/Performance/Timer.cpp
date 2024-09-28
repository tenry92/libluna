#include <libluna/Performance/Timer.hpp>
#include <libluna/Clock.hpp>

using namespace Luna::Performance;

class Timer::impl {
  public:
  Clock::TimePoint lastUpdate;
};

Timer::Timer() : mImpl{std::make_unique<impl>()} {}

Timer::~Timer() = default;

void Timer::start() { mImpl->lastUpdate = Clock::now(); }

double Timer::elapse() {
  auto now = Clock::now();
  auto delta = Clock::timeSpan(mImpl->lastUpdate, now);
  mImpl->lastUpdate = Clock::now();

  return delta;
}
