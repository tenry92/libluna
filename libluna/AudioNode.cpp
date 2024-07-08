#include <cstring>

#ifndef N64
#include <opusfile.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/AudioManagerImpl.hpp>
#include <libluna/AudioNode.hpp>
#include <libluna/AudioNodeImpl.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>

using namespace Luna;

class AudioCommand : public Command {
  public:
  AudioCommand(std::function<void()> callback) : mCallback(callback) {}

  void execute() const override { mCallback(); }

  private:
  std::function<void()> mCallback;
};

AudioNode::AudioNode(AudioManager *audioManager)
    : mImpl{std::make_unique<AudioNodeImpl>()} {
  mImpl->mManager = audioManager;
}

AudioNode::~AudioNode() = default;

void AudioNode::addInput(std::shared_ptr<AudioNode> input) {
  mImpl->mManager->getImpl()->pushCommand(std::make_shared<AudioCommand>([=]() {
    mImpl->mInputs.emplace_back(input);
  }));
}

AudioNodeImpl *AudioNode::getImpl() const { return mImpl.get(); }

class StreamAudioNode::impl {
  public:
  impl(const String &name) : mName{name} {}

  void init() {
#ifndef N64
    mOpusReader = ResourceReader::make(mName.c_str());

    OpusFileCallbacks callbacks;
    callbacks.read = [](void *stream, unsigned char *buffer, int byteCount) {
      auto reader = reinterpret_cast<ResourceReader *>(stream);

      return static_cast<int>(reader->read(buffer, 1, byteCount));
    };
    callbacks.seek = [](void *stream, std::int64_t offset, int whence) {
      auto reader = reinterpret_cast<ResourceReader *>(stream);

      switch (whence) {
      case SEEK_SET:
        reader->seek(offset);
        break;
      case SEEK_CUR:
        reader->seekRelative(static_cast<int>(offset));
        break;
      case SEEK_END:
        reader->seek(reader->getSize() + offset);
        break;
      }

      return 0;
    };
    callbacks.tell = [](void *stream) {
      auto reader = reinterpret_cast<ResourceReader *>(stream);

      return static_cast<std::int64_t>(reader->tell());
    };

    int result;
    mOggOpusFile =
        op_open_callbacks(mOpusReader.get(), &callbacks, nullptr, 0, &result);

    if (result != 0) {
      logError("failed opening opus file");
    }
#endif
  }

  bool isInitialized() {
#ifndef N64
    return mOggOpusFile != nullptr;
#else
    return true;
#endif
  }

  void free() {
#ifndef N64
    if (mOggOpusFile) {
      op_free(mOggOpusFile);
      mOggOpusFile = nullptr;
    }
#endif
  }

  String mName;

  ResourceReaderPtr mOpusReader;
#ifndef N64
  OggOpusFile *mOggOpusFile{nullptr};
#endif
};

StreamAudioNode::StreamAudioNode(const String &name)
    : AudioNode(AudioManager::getInstance()), mImpl{std::make_unique<impl>(name
                                              )} {}

StreamAudioNode::~StreamAudioNode() = default;

void StreamAudioNode::render([[maybe_unused]] float *buffer, [[maybe_unused]] int frameCount) {
#ifndef N64
  if (!mImpl->isInitialized()) {
    mImpl->init();
  }

  int remainingSampleCount = frameCount * 2;
  float *targetBuffer = buffer;

  do {
    int framesRead = op_read_float_stereo(
        mImpl->mOggOpusFile, targetBuffer, remainingSampleCount
    );

    if (framesRead <= 0) {
      // failure
      return;
    }

    remainingSampleCount -= framesRead * 2;
    targetBuffer += framesRead * 2;

    if (op_pcm_tell(mImpl->mOggOpusFile) ==
        op_pcm_total(mImpl->mOggOpusFile, -1)) {
      op_pcm_seek(mImpl->mOggOpusFile, 0);
    }
  } while (remainingSampleCount > 0);

  for (int sampleIndex = 0; sampleIndex < frameCount * 2; ++sampleIndex) {
    buffer[sampleIndex] *= 0.1f;
  }
#endif
}
