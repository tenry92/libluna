#pragma once

#include <vector>

namespace Luna {
  /**
   * @brief A static buffer of audio samples.
   *
   * This class is used to store audio samples in memory.
   * The buffer usually contains a single channel of audio samples.
   * If it contains multiple channels, the channels are interleaved.
   */
  class SoundBuffer {
    public:
    SoundBuffer() = default;
    SoundBuffer(const std::vector<float>& samples) : mSamples(samples) {}
    SoundBuffer(std::vector<float>&& samples) : mSamples(std::move(samples)) {}

    const std::vector<float>& getSamples() const { return mSamples; }

    private:
    std::vector<float> mSamples;
  };
}
