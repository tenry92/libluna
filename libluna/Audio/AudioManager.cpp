#include <libluna/config.h>

#include <algorithm>
#include <future>
#include <list>
#include <queue>

#ifdef LUNA_AUDIO_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/Audio/AudioManager.hpp>
#include <libluna/Audio/AudioManagerImpl.hpp>
#include <libluna/Audio/AudioNodeImpl.hpp>
#include <libluna/Command.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>

using namespace Luna;
using namespace Luna::Audio;

class DestinationAudioNode : public AudioNode {
  public:
  DestinationAudioNode(AudioManager *manager) : AudioNode(manager) {}
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
    auto audioManager = destinationNode->getImpl()->mManager;

    auto sampleCount = byteCount / sizeof(float);
    auto frameCount = sampleCount / audioManager->getChannelCount();
    auto frameRate = audioManager->getFrameRate();

    metrics->bufferSize =
        static_cast<float>(frameCount) / frameRate;

    metrics->renderTicker.tick();

    destinationNode->render(
        reinterpret_cast<float *>(stream), static_cast<int>(frameCount)
    );

    audioManager->getImpl()->mTime += static_cast<double>(frameCount) / frameRate;

    metrics->renderTicker.measure();
  }
} // namespace

AudioManager::AudioManager() : mImpl{std::make_unique<AudioManagerImpl>()} {
  gInstance = this;
  mImpl->mDestinationNode = std::make_shared<DestinationAudioNode>(this);
  mImpl->mDestinationNode->getImpl()->mManager = this;
  mImpl->mTime = 0.0;
  mImpl->mChannelCount = 2;
  mImpl->mFrameRate = 48000.0f;
}

AudioManager::~AudioManager() = default;

std::shared_ptr<AudioNode> AudioManager::getDestinationNode() const {
  return mImpl->mDestinationNode;
}

void AudioManager::init() {
#ifdef LUNA_AUDIO_SDL2
  SDL_AudioSpec desired, obtained;

  desired.freq = static_cast<int>(getFrameRate());
  desired.format = AUDIO_F32;
  desired.channels = static_cast<uint8_t>(getChannelCount());
  desired.samples = 4096; /* sample FRAMES (channels combined) */
  desired.callback = audioCallback;
  desired.userdata = mImpl->mDestinationNode.get();

  mImpl->mSdlAudioDeviceId = SDL_OpenAudioDevice(
      nullptr, /* use suitable device */
      false,   /* no capture */
      &desired, &obtained, 0
  );

  mImpl->mFrameRate = static_cast<float>(obtained.freq);
  mImpl->mChannelCount = obtained.channels;

  logDebug("obtained format: {}Hz, {} channels, {} ({})", obtained.freq, obtained.channels, obtained.format, AUDIO_F32);

  if (mImpl->mSdlAudioDeviceId == 0) {
    logError(SDL_GetError());
  } else {
    SDL_PauseAudioDevice(mImpl->mSdlAudioDeviceId, false);
    logInfo("audio opened");
  }
#endif
#ifdef N64
  int internalBufferCount = 4;
  audio_init(static_cast<int>(getFrameRate()), internalBufferCount);

  // int numMixChannels = 16;
  // mixer_init(numMixChannels);
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

double AudioManager::getTime() const {
  return mImpl->mTime;
}

int AudioManager::getChannelCount() const {
  return mImpl->mChannelCount;
}

float AudioManager::getFrameRate() const {
  return mImpl->mFrameRate;
}

std::shared_ptr<DelayNode> AudioManager::createDelay(float delay) {
  return std::make_shared<DelayNode>(this, delay);
}

std::shared_ptr<GainNode> AudioManager::createGain(float volume) {
  return std::make_shared<GainNode>(this, volume);
}

std::shared_ptr<OscillatorNode> AudioManager::createOscillator(float frequency, OscillatorNode::Type type) {
  return std::make_shared<OscillatorNode>(this, frequency, type);
}

AudioManagerImpl *AudioManager::getImpl() const { return mImpl.get(); }
