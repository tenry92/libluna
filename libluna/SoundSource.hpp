#pragma once

namespace Luna {
  class SoundSource {
    public:
    virtual ~SoundSource() = default;

    /**
     * @brief Write interleaved stereo audio samples to the buffer.
     *
     * @return Number of frames actually written.
     * If 0, the source is assumed to be finished.
     */
    virtual int write(float* buffer, int frames) = 0;
  };
} // namespace Luna
