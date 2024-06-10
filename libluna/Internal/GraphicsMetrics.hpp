#pragma once

#include <libluna/String.hpp>

namespace Luna::Internal {
  struct GraphicsMetrics {
    int spriteCount{0};
    int textureCount{0};
    int maxTextureSize{0};
    int glMajor{0};
    int glMinor{0};
    String vendor;
    String shadingLangVersion;
  };
} // namespace Luna::Internal
