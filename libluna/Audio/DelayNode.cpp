#include <cstring>

#include <libluna/Audio/AudioNodeImpl.hpp>
#include <libluna/Audio/DelayNode.hpp>
#include <libluna/Logger.hpp>

using namespace Luna;
using namespace Luna::Audio;

class DelayNode::impl {
  public:
  float mDelay;
  std::vector<float> mBuffer;
  int mBufferOffset;
};

DelayNode::DelayNode(AudioManager *manager, float delay) : AudioNode(manager), mImpl{std::make_unique<impl>()} {
  mImpl->mDelay = delay;
  mImpl->mBuffer.resize(static_cast<int>(getFrameRate() * delay) * getChannelCount());
  mImpl->mBufferOffset = 0;
}

DelayNode::~DelayNode() = default;

void DelayNode::render(float *buffer, int frameCount) {
  int sampleCount = frameCount * getChannelCount();
  int renderedSampleCount = 0;

  while (renderedSampleCount < sampleCount) {
    size_t chunkSampleCount = sampleCount - renderedSampleCount;

    if (mImpl->mBufferOffset + chunkSampleCount > mImpl->mBuffer.size()) {
      chunkSampleCount = mImpl->mBuffer.size() - mImpl->mBufferOffset;
    }

    // copy previous data
    memcpy(buffer, mImpl->mBuffer.data() + mImpl->mBufferOffset, sizeof(float) * chunkSampleCount);
    buffer += chunkSampleCount;

    // render new input
    if (getImpl()->mInputs.size() > 0) {
      auto input = getImpl()->mInputs.front();
      input->render(mImpl->mBuffer.data() + mImpl->mBufferOffset, static_cast<int>(chunkSampleCount / getChannelCount()));
    }

    mImpl->mBufferOffset = static_cast<int>((mImpl->mBufferOffset + chunkSampleCount) % mImpl->mBuffer.size());
    renderedSampleCount += static_cast<int>(chunkSampleCount);
  }
}
