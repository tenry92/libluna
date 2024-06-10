#include <libluna/Performance/Ticker.hpp>

#include <chrono>
#include <deque>
#include <map>

using namespace Luna::Performance;
using Luna::String;

static constexpr std::size_t kTicksPerQueue = 10;

static std::map<String, Ticker &> gTickers;

class Ticker::impl {
  public:
  unsigned long mTickCount = 0;
  std::deque<std::chrono::steady_clock::time_point> mTicks;
  std::deque<std::chrono::steady_clock::duration> mTickTimes;
  String mName;
};

Ticker::Ticker(const String &pName) : mImpl{std::make_unique<impl>()} {
  mImpl->mName = pName;

  if (pName != "") {
    gTickers.emplace(pName, *this);
  }
}

Ticker::~Ticker() {
  auto it = gTickers.find(mImpl->mName);

  if (it != gTickers.end() && &it->second == this) {
    gTickers.erase(it);
  }
}

void Ticker::tick() {
  mImpl->mTicks.emplace_back(std::chrono::steady_clock::now());
  ++mImpl->mTickCount;

  if (mImpl->mTicks.size() > kTicksPerQueue) {
    mImpl->mTicks.pop_front();
  }
}

void Ticker::measure() {
  if (mImpl->mTicks.empty()) {
    return;
  }

  auto now = std::chrono::steady_clock::now();
  auto last = mImpl->mTicks.back();
  mImpl->mTickTimes.push_back(now - last);

  if (mImpl->mTickTimes.size() > kTicksPerQueue) {
    mImpl->mTickTimes.pop_front();
  }
}

float Ticker::getTickDuration() const {
  if (mImpl->mTickTimes.size() == 0) {
    return 0.f;
  }

  auto unit = std::nano().den;
  long sum = 0;

  for (auto time : mImpl->mTickTimes) {
    sum += static_cast<long>(
        time.count() * unit * std::chrono::steady_clock::period().num /
        std::chrono::steady_clock::period().den
    );
  }

  return static_cast<float>(sum) / static_cast<float>(unit) /
         static_cast<float>(mImpl->mTickTimes.size());
}

float Ticker::getTicksPerSecond() const {
  if (mImpl->mTicks.size() < 2) {
    // we can't calculate anything meaningful with < 2 entries
    return 0.f;
  }

  auto unit = std::nano().den;
  long sum = 0;

  // aggregate the deltas from a tick and following tick
  for (std::size_t i = 0; i < mImpl->mTicks.size() - 1; ++i) {
    auto diff = mImpl->mTicks[i + 1] - mImpl->mTicks[i];
    sum += static_cast<long>(
        diff.count() * unit * std::chrono::steady_clock::period().num /
        std::chrono::steady_clock::period().den
    );
  }

  auto averageDuration = static_cast<float>(sum) / static_cast<float>(unit) /
                         static_cast<float>(mImpl->mTicks.size() - 1);

  return 1.f / averageDuration;
}

Ticker *Ticker::getTickerByName(const String &name) {
  auto it = gTickers.find(name);

  if (it != gTickers.end()) {
    return &it->second;
  }

  return nullptr;
}
