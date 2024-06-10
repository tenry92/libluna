#pragma once

#include <libluna/Performance/Ticker.hpp>

namespace Luna::Internal {
  struct AudioMetrics {
    float bufferSize;
    Performance::Ticker renderTicker;
  };
} // namespace Luna::Internal
