#include <libluna/config.h>

#include <algorithm>
#include <future>
#include <list>
#include <queue>

#ifdef LUNA_AUDIO_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/AudioManager.hpp>
#include <libluna/AudioManagerImpl.hpp>
#include <libluna/AudioNodeImpl.hpp>
#include <libluna/Command.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>

using namespace Luna;

class DestinationAudioNode : public AudioNode {
  public:
  DestinationAudioNode(AudioManager *audioManager) : AudioNode(audioManager) {}

  void render(float *buffer, int frameCount) override {
    int sampleCount = frameCount * 2;
    std::fill(buffer, buffer + sampleCount, 0.0f);
    std::vector<float> mixBuffer(sampleCount);

    for (auto &&input : getImpl()->mInputs) {
      input->render(mixBuffer.data(), frameCount);

      std::transform(
          buffer, buffer + sampleCount, mixBuffer.data(), buffer,
          [](float a, float b) { return a + b; }
      );
    }
  }
};

namespace {
  AudioManager *gInstance{nullptr};

  [[maybe_unused]] void audioCallback(void *userData, std::uint8_t *stream, int byteCount) {
    auto metrics = &AudioManager::getInstance()->getImpl()->mMetrics;

    Logger::getInstance().setThreadIdentifier("audio");
    auto destinationNode = reinterpret_cast<DestinationAudioNode *>(userData);

    auto sampleCount = byteCount / sizeof(float);
    auto frameCount = sampleCount / 2;
    auto frameRate = 48000;

    metrics->bufferSize =
        static_cast<float>(frameCount) / static_cast<float>(frameRate);

    metrics->renderTicker.tick();

    destinationNode->render(
        reinterpret_cast<float *>(stream), static_cast<int>(frameCount)
    );

    metrics->renderTicker.measure();
  }
} // namespace

AudioManager::AudioManager() : mImpl{std::make_unique<AudioManagerImpl>()} {
  gInstance = this;
  mImpl->mDestinationNode = std::make_shared<DestinationAudioNode>(this);
}

AudioManager::~AudioManager() = default;

std::shared_ptr<AudioNode> AudioManager::getDestinationNode() const {
  return mImpl->mDestinationNode;
}

void AudioManager::init() {
#ifdef LUNA_AUDIO_SDL2
  SDL_AudioSpec desired, obtained;

  desired.freq = 48000;
  desired.format = AUDIO_F32;
  desired.channels = 2;
  desired.samples = 4096; /* sample FRAMES (channels combined) */
  desired.callback = audioCallback;
  desired.userdata = mImpl->mDestinationNode.get();

  mImpl->mSdlAudioDeviceId = SDL_OpenAudioDevice(
      nullptr, /* use suitable device */
      false,   /* no capture */
      &desired, &obtained, 0
  );

  if (mImpl->mSdlAudioDeviceId == 0) {
    logError(SDL_GetError());
  } else {
    SDL_PauseAudioDevice(mImpl->mSdlAudioDeviceId, false);
    logInfo("audio opened");
  }
#endif
}

void AudioManager::update() {
#ifdef LUNA_AUDIO_SDL2
  SDL_LockAudioDevice(mImpl->mSdlAudioDeviceId);
  while (!mImpl->mCommandQueue.empty()) {
    auto command = mImpl->mCommandQueue.front();
    mImpl->mCommandQueue.pop();
    command->execute();
  }
  SDL_UnlockAudioDevice(mImpl->mSdlAudioDeviceId);
#endif
}

void AudioManager::free() {
#ifdef LUNA_AUDIO_SDL2
  if (mImpl->mSdlAudioDeviceId > 0) {
    SDL_CloseAudioDevice(mImpl->mSdlAudioDeviceId);
  }
#endif
}

AudioManager *AudioManager::getInstance() { return gInstance; }

AudioManagerImpl *AudioManager::getImpl() const { return mImpl.get(); }
