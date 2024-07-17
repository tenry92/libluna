#include <cstring>

#include <libluna/Application.hpp>
#include <libluna/Audio/AudioManagerImpl.hpp>
#include <libluna/Audio/AudioNode.hpp>
#include <libluna/Audio/AudioNodeImpl.hpp>
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

AudioNode::AudioNode(AudioManager *manager)
    : mImpl{std::make_unique<AudioNodeImpl>()} {
  mImpl->mManager = manager;
}

AudioNode::~AudioNode() = default;

void AudioNode::connect(AudioNodePtr destination) {
  disconnect();
  mImpl->mConnectedNode = destination;
  destination->addInput(shared_from_this());
}

void AudioNode::disconnect() {
  if (mImpl->mConnectedNode) {
    mImpl->mConnectedNode->removeInput(shared_from_this());
    mImpl->mConnectedNode = nullptr;
  }
}

void AudioNode::addInput(AudioNodePtr input) {
  mImpl->mInputs.emplace_back(input);
}

void AudioNode::removeInput(AudioNodePtr input) {
  mImpl->mInputs.remove(input);
}

int AudioNode::getChannelCount() const {
  return mImpl->mManager->getChannelCount();
}

float AudioNode::getFrameRate() const {
  return mImpl->mManager->getFrameRate();
}

AudioNodeImpl *AudioNode::getImpl() const { return mImpl.get(); }
