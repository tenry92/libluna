#pragma once

#include <list>

#include <libluna/AudioNode.hpp>

namespace Luna {
  class AudioManager;

  class AudioNodeImpl {
    public:
    AudioManager *mManager;
    std::list<std::shared_ptr<AudioNode>> mInputs;
  };
} // namespace Luna
