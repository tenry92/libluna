#include <cmath>

#include <libluna/Audio/OscillatorNode.hpp>

using namespace Luna;
using namespace Luna::Audio;

constexpr double pi = 3.14159265358979323846;

OscillatorNode::OscillatorNode(
    AudioManager *manager, float frequency, Type type
)
    : AudioNode(manager) {
  mFrequency = frequency;
  mType = type;
  mPhase = 0.0f;
  mDuty = 0.5f;
}

OscillatorNode::~OscillatorNode() = default;

void OscillatorNode::render(float *buffer, int frameCount) {
  // assuming a frame rate of 48000
  float phaseIncrement =
      static_cast<float>(pi * 2.0f * mFrequency / getFrameRate());

  for (int i = 0; i < frameCount; ++i) {
    float sample;

    switch (mType) {
    default:
      sample = sinf(mPhase);
      break;
    case kSquare: {
      sample = sinf(mPhase);
      float threshold = 1.0f - mDuty * 2.0f;
      sample = (sample < threshold) ? -0.33f : 0.33f;
      break;
    }
    case kTriangle:
      sample = -1.0f + mPhase / static_cast<float>(pi) * 2.0f;

      if (sample > 1.0f) {
        sample = 1.0f - (sample - 1.0f);
      }
      break;
    case kSawtooth:
      sample = (-1.0f + mPhase / static_cast<float>(pi)) * 0.5f;
      break;
    }

    buffer[i * 2] = buffer[i * 2 + 1] = sample;

    mPhase = fmodf(
        static_cast<float>(mPhase + phaseIncrement),
        static_cast<float>(pi * 2.0f)
    );
  }
}

void OscillatorNode::setFrequency(float frequency) {
  mFrequency = frequency;
}

void OscillatorNode::setType(Type type) { mType = type; }

void OscillatorNode::setDuty(float duty) { mDuty = duty; }
