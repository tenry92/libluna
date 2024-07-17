#include <algorithm>

#include <libluna/Audio/AudioNodeImpl.hpp>
#include <libluna/Audio/GainNode.hpp>

using namespace Luna;
using namespace Luna::Audio;

GainNode::GainNode(AudioManager *manager, float volume) : AudioNode(manager), mVolume(volume) {}

GainNode::~GainNode() = default;

void GainNode::render(float *buffer, int frameCount) {
  if (getImpl()->mInputs.size() > 0) {
    auto input = getImpl()->mInputs.front();
    input->render(buffer, frameCount);

    std::transform(
      buffer, buffer + frameCount * getChannelCount(), buffer, [=](float a) {
        return a *= mVolume;
      }
    );
  }
}
