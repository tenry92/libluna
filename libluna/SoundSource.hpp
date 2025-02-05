#pragma once

namespace Luna {
  class SoundSource {
    public:
    virtual ~SoundSource() = default;
    virtual void generate(float* buffer, unsigned int samples) = 0;
  };
} // namespace Luna
