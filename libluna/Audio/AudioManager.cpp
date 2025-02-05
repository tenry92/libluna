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
#include <libluna/Command.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>

using namespace Luna;
using namespace Luna::Audio;

#ifdef N64
constexpr int kInternalBufferCount = 4;
#endif

class DestinationAudioNode : public AudioNode {
  public:
  DestinationAudioNode(AudioManager* manager) : AudioNode(manager) {}
  void render(float* buffer, int frameCount) override {
    int sampleCount = frameCount * 2;
    std::fill(buffer, buffer + sampleCount, 0.0f);
    std::vector<float> mixBuffer(sampleCount);

    for (auto&& input : mInputs) {
      input->render(mixBuffer.data(), frameCount);

      std::transform(
        buffer, buffer + sampleCount, mixBuffer.data(), buffer,
        [](float a, float b) { return a + b; }
      );
    }
  }
};

namespace {
  AudioManager* gInstance{nullptr};

  [[maybe_unused]] void
  audioCallback(void* userData, std::uint8_t* stream, int byteCount) {
    auto metrics = &AudioManager::getInstance()->getMetrics();

    Logger::getInstance().setThreadIdentifier("audio");
    auto destinationNode = reinterpret_cast<DestinationAudioNode*>(userData);
    auto audioManager = AudioManager::getInstance();

    auto sampleCount = byteCount / sizeof(float);
    auto frameCount = sampleCount / audioManager->getChannelCount();
    auto frameRate = audioManager->getFrameRate();

    metrics->bufferSize = static_cast<float>(frameCount) / frameRate;

    metrics->renderTicker.tick();

    destinationNode->render(
      reinterpret_cast<float*>(stream), static_cast<int>(frameCount)
    );

    audioManager->advanceTime(static_cast<double>(frameCount) / frameRate);

    metrics->renderTicker.measure();
  }
} // namespace

AudioManager::AudioManager() {
  gInstance = this;
  mDestinationNode = std::make_shared<DestinationAudioNode>(this);
  mDestinationNode->mManager = this;
  mTime = 0.0;
  mChannelCount = 2;
  mFrameRate = 48000.0f;
}

AudioManager::~AudioManager() = default;

std::shared_ptr<AudioNode> AudioManager::getDestinationNode() const {
  return mDestinationNode;
}

void AudioManager::init() {
#ifdef LUNA_AUDIO_SDL2
  SDL_AudioSpec desired, obtained;

  desired.freq = static_cast<int>(getFrameRate());
  desired.format = AUDIO_F32;
  desired.channels = static_cast<uint8_t>(getChannelCount());
  desired.samples = 4096; /* sample FRAMES (channels combined) */
  desired.callback = audioCallback;
  desired.userdata = mDestinationNode.get();

  mSdlAudioDeviceId = SDL_OpenAudioDevice(
    nullptr, /* use suitable device */
    false,   /* no capture */
    &desired, &obtained, 0
  );

  mFrameRate = static_cast<float>(obtained.freq);
  mChannelCount = obtained.channels;

  logDebug(
    "obtained format: {}Hz, {} channels, {} ({})", obtained.freq,
    obtained.channels, obtained.format, AUDIO_F32
  );

  if (mSdlAudioDeviceId == 0) {
    logError(SDL_GetError());
  } else {
    SDL_PauseAudioDevice(mSdlAudioDeviceId, false);
    logInfo("audio opened");
  }
#endif
#ifdef N64
  audio_init(static_cast<int>(getFrameRate()), kInternalBufferCount);

  const int numMixChannels = 16;
  mixer_init(numMixChannels);

  wav64_init_compression(3);
#endif
}

void AudioManager::update() {
#ifdef LUNA_AUDIO_SDL2
  SDL_LockAudioDevice(mSdlAudioDeviceId);
#endif
  while (!mCommandQueue.empty()) {
    auto command = mCommandQueue.front();
    mCommandQueue.pop();
    command->execute();
  }
#ifdef LUNA_AUDIO_SDL2
  SDL_UnlockAudioDevice(mSdlAudioDeviceId);
#endif
#ifdef N64
  for (auto it = mPlayingSounds.begin(); it != mPlayingSounds.end();) {
    auto sound = *it;
    if (!sound->isPlaying()) {
      it = mPlayingSounds.erase(it);
      destroySound(sound);
    } else {
      ++it;
    }
  }

  for (int i = 0; i < kInternalBufferCount; i++) {
    mixer_try_play();
  }
#endif
}

void AudioManager::free() {
#ifdef LUNA_AUDIO_SDL2
  if (mSdlAudioDeviceId > 0) {
    SDL_CloseAudioDevice(mSdlAudioDeviceId);
  }
#endif
#ifdef N64
  mixer_close();
  audio_close();
#endif
}

AudioManager* AudioManager::getInstance() { return gInstance; }

double AudioManager::getTime() const { return mTime; }

int AudioManager::getChannelCount() const { return mChannelCount; }

float AudioManager::getFrameRate() const { return mFrameRate; }

Sound* AudioManager::createSound() { return mSounds.acquire(); }

void AudioManager::destroySound(Sound* sound) { mSounds.release(sound); }

void AudioManager::playSound(const char* source) {
  auto sound = createSound();
  mPlayingSounds.push_front(sound);

#ifdef N64
  sound->setSource(source);
  sound->play();
#endif
}

std::shared_ptr<DelayNode> AudioManager::createDelay(float delay) {
  return std::make_shared<DelayNode>(this, delay);
}

std::shared_ptr<GainNode> AudioManager::createGain(float volume) {
  return std::make_shared<GainNode>(this, volume);
}

std::shared_ptr<OscillatorNode>
AudioManager::createOscillator(float frequency, OscillatorNode::Type type) {
  return std::make_shared<OscillatorNode>(this, frequency, type);
}

Internal::AudioMetrics& AudioManager::getMetrics() { return mMetrics; }

void AudioManager::advanceTime(double time) { mTime += time; }
