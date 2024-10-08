#include <libluna/Clock.hpp>
#include <libluna/Performance/Timer.hpp>

using namespace Luna::Performance;

Timer::Timer() = default;

Timer::~Timer() = default;

void Timer::start() { lastUpdate = Clock::now(); }

double Timer::elapse() {
  auto now = Clock::now();
  auto delta = Clock::timeSpan(lastUpdate, now);
  lastUpdate = Clock::now();

  return delta;
}
