#pragma once

#include <libluna/Performance/Ticker.hpp>

namespace Luna::Internal {
  struct DebugMetrics {
    unsigned int framesElapsed{0};
    Performance::Ticker frameTicker;
    Performance::Ticker renderTicker;
  };
} // namespace Luna::Internal
