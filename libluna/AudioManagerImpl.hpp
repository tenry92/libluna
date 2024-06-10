#pragma once

#include <libluna/config.h>

#include <memory>
#include <queue>

#ifdef LUNA_USE_SDL
#include <SDL2/SDL.h>
#endif

#include <libluna/AudioNode.hpp>
#include <libluna/Command.hpp>
#include <libluna/Internal/AudioMetrics.hpp>

namespace Luna {
  class AudioManagerImpl {
    public:
    inline void pushCommand(std::shared_ptr<Command> command) {
      mCommandQueue.push(command);
    }

    std::shared_ptr<AudioNode> mDestinationNode;
    std::queue<std::shared_ptr<Command>> mCommandQueue;
    Internal::AudioMetrics mMetrics;
#ifdef LUNA_USE_SDL
    SDL_AudioDeviceID mSdlAudioDeviceId{0};
#endif
  };
} // namespace Luna
