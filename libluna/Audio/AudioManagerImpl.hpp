#pragma once

#include <libluna/config.h>

#include <memory>
#include <queue>

#ifdef LUNA_AUDIO_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/Audio/AudioNode.hpp>
#include <libluna/Command.hpp>
#include <libluna/Internal/AudioMetrics.hpp>

namespace Luna::Audio {
  class AudioManagerImpl {
    public:
    inline void pushCommand(std::shared_ptr<Command> command) {
      mCommandQueue.push(command);
    }

    AudioNodePtr mDestinationNode;
    std::queue<std::shared_ptr<Command>> mCommandQueue;
    double mTime;
    float mFrameRate;
    int mChannelCount;
    Internal::AudioMetrics mMetrics;
#ifdef LUNA_AUDIO_SDL2
    SDL_AudioDeviceID mSdlAudioDeviceId{0};
#endif
  };
} // namespace Luna::Audio
