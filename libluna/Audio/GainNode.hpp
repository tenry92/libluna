#pragma once

#include <libluna/Audio/AudioNode.hpp>

namespace Luna::Audio {
  class GainNode : public AudioNode {
    public:
    GainNode(AudioManager *manager, float volume);
    ~GainNode();
    void render(float *buffer, int frameCount) override;
    void setVolume(float volume);

    private:
    float mVolume;
  };
}
