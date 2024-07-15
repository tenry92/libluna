#pragma once

#include <memory>

namespace Luna {
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

    AudioManagerImpl *getImpl() const;

    private:
    std::unique_ptr<AudioManagerImpl> mImpl;
  };
} // namespace Luna
