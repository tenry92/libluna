#include <libluna/SoundBufferSource.hpp>

namespace Luna {
  SoundBufferSource::SoundBufferSource(SoundBuffer* buffer, bool loop) : mBuffer(buffer), mLoop(loop) {}

  int SoundBufferSource::write(float* buffer, int frames) {
    int framesWritten = 0;
    auto samples = mBuffer->getSamples();
    auto sampleCount = samples.size();

    for (int i = 0; i < frames; i++) {
      float sample = samples[mSamplePosition];

      buffer[i * 2] = sample;
      buffer[i * 2 + 1] = sample;
      ++framesWritten;

      ++mSamplePosition;

      if (mSamplePosition >= sampleCount) {
        if (mLoop) {
          mSamplePosition = 0;
        } else {
          break;
        }
      }
    }

    return framesWritten;
  }
}
