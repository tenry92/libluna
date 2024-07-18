#pragma once

#include <list>

#include <libluna/Audio/AudioNode.hpp>

namespace Luna::Audio {
  class AudioManager;

  class AudioNodeImpl {
    public:
    AudioManager *mManager;
    std::list<AudioNodePtr> mInputs;
    AudioNodePtr mConnectedNode;
  };
} // namespace Luna::Audio
