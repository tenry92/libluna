#pragma once

#include <memory>

#include <libluna/Audio/DelayNode.hpp>
#include <libluna/Audio/GainNode.hpp>
#include <libluna/Audio/OscillatorNode.hpp>

namespace Luna::Audio {
  class AudioManagerImpl;
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

    static AudioManager *getInstance();

    double getTime() const;

    int getChannelCount() const;

    float getFrameRate() const;

    std::shared_ptr<DelayNode> createDelay(float delay);

    std::shared_ptr<GainNode> createGain(float volume);

    std::shared_ptr<OscillatorNode> createOscillator(
        float frequency, OscillatorNode::Type type = OscillatorNode::kSine
    );

    AudioManagerImpl *getImpl() const;

    private:
    std::unique_ptr<AudioManagerImpl> mImpl;
  };
} // namespace Luna::Audio
