#pragma once

#include <memory>

#include <libluna/String.hpp>

namespace Luna::Audio {
  class AudioNode;
  using AudioNodePtr = std::shared_ptr<AudioNode>;
  class AudioManager;

  /**
   * @brief Base node for rendering and transforming audio data.
   */
  class AudioNode : public std::enable_shared_from_this<AudioNode> {
    public:
    AudioNode(AudioManager* manager);
    virtual ~AudioNode();

    /**
     * @brief Render @p frameCount frames into @p buffer.
     *
     * In stereo mode (2 channels), a frame represents 2 samples (2 floats).
     */
    virtual void render(float* buffer, int frameCount) = 0;

    // void addInput(std::shared_ptr<AudioNode> source);

    void connect(AudioNodePtr destination);

    void disconnect();

    int getChannelCount() const;

    float getFrameRate() const;

    protected:
    friend class AudioManager;
    AudioNode();
    std::list<AudioNodePtr> mInputs;
    AudioManager* mManager;
    AudioNodePtr mConnectedNode;

    private:
    void addInput(AudioNodePtr input);
    void removeInput(AudioNodePtr input);
  };
} // namespace Luna::Audio
