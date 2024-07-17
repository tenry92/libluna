#include <cmath>

#include <libluna/Audio/OscillatorNode.hpp>

using namespace Luna;
using namespace Luna::Audio;

constexpr double pi = 3.14159265358979323846;

class OscillatorNode::impl {
  public:
  float mFrequency;
  Type mType;
  float mPhase;
  float mDuty;
};

OscillatorNode::OscillatorNode(AudioManager *manager, float frequency, Type type) : AudioNode(manager), mImpl{std::make_unique<impl>()} {
  mImpl->mFrequency = frequency;
  mImpl->mType = type;
  mImpl->mPhase = 0.0f;
  mImpl->mDuty = 0.5f;
}

OscillatorNode::~OscillatorNode() = default;

void OscillatorNode::render(float *buffer, int frameCount) {
  // assuming a frame rate of 48000
  float phaseIncrement = static_cast<float>(pi * 2.0f * mImpl->mFrequency / getFrameRate());

  for (int i = 0; i < frameCount; ++i) {
    float sample;

    switch (mImpl->mType) {
      default:
        sample = sinf(mImpl->mPhase);
        break;
      case kSquare: {
        sample = sinf(mImpl->mPhase);
        float threshold = 1.0f - mImpl->mDuty * 2.0f;
        sample = (sample < threshold) ? -0.33f : 0.33f;
        break;
      }
      case kTriangle:
        sample = -1.0f + mImpl->mPhase / static_cast<float>(pi) * 2.0f;

        if (sample > 1.0f) {
          sample = 1.0f - (sample - 1.0f);
        }
        break;
      case kSawtooth:
        sample = (-1.0f + mImpl->mPhase / static_cast<float>(pi)) * 0.5f;
        break;
    }

    buffer[i * 2] = buffer[i * 2 + 1] = sample;

    mImpl->mPhase = fmodf(static_cast<float>(mImpl->mPhase + phaseIncrement), static_cast<float>(pi * 2.0f));
  }
}

void OscillatorNode::setFrequency(float frequency) {
  mImpl->mFrequency = frequency;
}

void OscillatorNode::setType(Type type) {
  mImpl->mType = type;
}

void OscillatorNode::setDuty(float duty) {
  mImpl->mDuty = duty;
}
