#include <libluna/Performance/Timer.hpp>

#include <chrono>

using namespace Luna::Performance;

class Timer::impl {
  public:
  std::chrono::steady_clock::time_point lastUpdate;
};

Timer::Timer() : mImpl{std::make_unique<impl>()} {}

Timer::~Timer() = default;

void Timer::start() { mImpl->lastUpdate = std::chrono::steady_clock::now(); }

double Timer::elapse() {
  auto now = std::chrono::steady_clock::now();
  auto delta = std::chrono::duration<double>(now - mImpl->lastUpdate).count();
  mImpl->lastUpdate = std::chrono::steady_clock::now();

  return delta;
}
