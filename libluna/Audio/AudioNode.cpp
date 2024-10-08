#include <cstring>

#include <libluna/Application.hpp>
#include <libluna/Audio/AudioNode.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>

using namespace Luna;
using namespace Luna::Audio;

class AudioCommand : public Command {
  public:
  AudioCommand(std::function<void()> callback) : mCallback(callback) {}

  void execute() const override { mCallback(); }

  private:
  std::function<void()> mCallback;
};

AudioNode::AudioNode(AudioManager *manager) {
  mManager = manager;
}

AudioNode::~AudioNode() = default;

void AudioNode::connect(AudioNodePtr destination) {
  disconnect();
  mConnectedNode = destination;
  destination->addInput(shared_from_this());
}

void AudioNode::disconnect() {
  if (mConnectedNode) {
    mConnectedNode->removeInput(shared_from_this());
    mConnectedNode = nullptr;
  }
}

void AudioNode::addInput(AudioNodePtr input) {
  mInputs.emplace_back(input);
}

void AudioNode::removeInput(AudioNodePtr input) {
  mInputs.remove(input);
}

int AudioNode::getChannelCount() const {
  return mManager->getChannelCount();
}

float AudioNode::getFrameRate() const {
  return mManager->getFrameRate();
}
