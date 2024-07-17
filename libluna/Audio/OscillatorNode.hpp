#pragma once

#include <libluna/Audio/AudioNode.hpp>

namespace Luna::Audio {
  class OscillatorNode : public AudioNode {
    public:
    enum Type {
      kSine,
      kSquare,
      kTriangle,
      kSawtooth
    };
    OscillatorNode(AudioManager *manager, float frequency, Type type = kSine);
    ~OscillatorNode();
    void render(float *buffer, int frameCount) override;

    void setFrequency(float frequency);

    void setType(Type type);

    void setDuty(float duty);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
}
