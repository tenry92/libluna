#pragma once

#include <memory>

#include <libluna/AudioManager.hpp>
#include <libluna/String.hpp>

namespace Luna {
  class AudioNodeImpl;

  class AudioNode {
    public:
    virtual ~AudioNode();
    virtual void render(float *buffer, int frameCount) = 0;
    void addInput(std::shared_ptr<AudioNode> source);

    AudioNodeImpl *getImpl() const;

    protected:
    AudioNode(AudioManager *audioManager);

    private:
    std::unique_ptr<AudioNodeImpl> mImpl;
  };

  class StreamAudioNode : public AudioNode {
    public:
    StreamAudioNode(const String &name);
    ~StreamAudioNode();
    void render(float *buffer, int frameCount) override;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
