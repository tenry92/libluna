#pragma once

#include <libluna/Audio/GainNode.hpp>

namespace Luna::Audio {
  class DelayNode : public AudioNode {
    public:
    DelayNode(AudioManager *manager, float delay);
    ~DelayNode();
    void render(float *buffer, int frameCount) override;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna::Audio
