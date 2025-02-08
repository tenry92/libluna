#pragma once

#include <libluna/SoundBuffer.hpp>
#include <libluna/SoundSource.hpp>

namespace Luna {
  class SoundBufferSource final : public SoundSource {
    public:
    SoundBufferSource(SoundBuffer* buffer, bool loop = false);

    int write(float* buffer, int frames) override;

    private:
    SoundBuffer* mBuffer;
    int mSamplePosition{0};
    bool mLoop{false};
  };
};
