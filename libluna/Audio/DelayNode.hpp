#pragma once

#include <libluna/Audio/GainNode.hpp>

namespace Luna::Audio {
  class DelayNode : public AudioNode {
    public:
    DelayNode(AudioManager* manager, float delay);
    ~DelayNode();
    void render(float* buffer, int frameCount) override;

    private:
    float mDelay;
    std::vector<float> mBuffer;
    int mBufferOffset;
  };
} // namespace Luna::Audio
