#include <cstring>

#include <libluna/Audio/DelayNode.hpp>
#include <libluna/Logger.hpp>

using namespace Luna;
using namespace Luna::Audio;

DelayNode::DelayNode(AudioManager *manager, float delay) : AudioNode(manager) {
  mDelay = delay;
  mBuffer.resize(static_cast<int>(getFrameRate() * delay) * getChannelCount());
  mBufferOffset = 0;
}

DelayNode::~DelayNode() = default;

void DelayNode::render(float *buffer, int frameCount) {
  int sampleCount = frameCount * getChannelCount();
  int renderedSampleCount = 0;

  while (renderedSampleCount < sampleCount) {
    size_t chunkSampleCount = sampleCount - renderedSampleCount;

    if (mBufferOffset + chunkSampleCount > mBuffer.size()) {
      chunkSampleCount = mBuffer.size() - mBufferOffset;
    }

    // copy previous data
    memcpy(
      buffer, mBuffer.data() + mBufferOffset, sizeof(float) * chunkSampleCount
    );
    buffer += chunkSampleCount;

    // render new input
    if (mInputs.size() > 0) {
      auto input = mInputs.front();
      input->render(
        mBuffer.data() + mBufferOffset,
        static_cast<int>(chunkSampleCount / getChannelCount())
      );
    }

    mBufferOffset =
      static_cast<int>((mBufferOffset + chunkSampleCount) % mBuffer.size());
    renderedSampleCount += static_cast<int>(chunkSampleCount);
  }
}
