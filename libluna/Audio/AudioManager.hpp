#pragma once

#include <libluna/config.h>

#include <list>

#ifdef LUNA_AUDIO_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/Audio/DelayNode.hpp>
#include <libluna/Audio/GainNode.hpp>
#include <libluna/Audio/OscillatorNode.hpp>
#include <libluna/Command.hpp>
#include <libluna/Internal/AudioMetrics.hpp>
#include <libluna/Pool.hpp>
#include <libluna/Sound.hpp>
#include <libluna/SoundBufferSource.hpp>

namespace Luna::Audio {
  class AudioNode;

  /**
   * @brief Main class for managing audio output.
   */
  class AudioManager {
    public:
    AudioManager();
    ~AudioManager();
    std::shared_ptr<AudioNode> getDestinationNode() const;

    /**
     * @brief Initialize the audio system.
     */
    void init();

    /**
     * @brief Update the audio.
     *
     * Call this once per frame to apply updates such as playing sound effects.
     *
     * Note that any updates done prior this call are executed simultaneously
     * (sample perfect). For example, as long as two audio nodes are started
     * before calling this function, these audio nodes are guaranteed to be
     * synced with each other.
     */
    void update();

    /**
     * @brief Shut down the audio system.
     */
    void free();

    static AudioManager* getInstance();

    double getTime() const;

    int getChannelCount() const;

    float getFrameRate() const;

    Sound* createSound();
    void destroySound(Sound* sound);

#ifdef N64
    void playSound(const char* source);
#else
    void playSound(SoundBufferSource* source);
#endif

    std::shared_ptr<DelayNode> createDelay(float delay);

    std::shared_ptr<GainNode> createGain(float volume);

    std::shared_ptr<OscillatorNode> createOscillator(
      float frequency, OscillatorNode::Type type = OscillatorNode::kSine
    );

    Internal::AudioMetrics& getMetrics();

    void advanceTime(double time);

    private:
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
    Pool<Sound, 64> mSounds;
    std::list<Sound*> mPlayingSounds;
  };
} // namespace Luna::Audio
