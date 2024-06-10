#pragma once

#include <memory>

namespace Luna {
  class AudioManagerImpl;
  class AudioNode;

  class AudioManager {
    public:
    AudioManager();
    ~AudioManager();
    std::shared_ptr<AudioNode> getDestinationNode() const;
    void init();
    void update();
    void free();
    static AudioManager *getInstance();

    AudioManagerImpl *getImpl() const;

    private:
    std::unique_ptr<AudioManagerImpl> mImpl;
  };
} // namespace Luna
