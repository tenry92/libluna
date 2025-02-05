#include <libluna/Sound.hpp>

#ifdef N64
#include <libdragon.h>
#endif

namespace {
#ifdef N64
  int findFreeChannel() {
    for (int i = 0; i < 16; i++) {
      if (!mixer_ch_playing(i)) {
        return i;
      }
    }

    return -1;
  }
#endif
} // namespace

namespace Luna {
  void Sound::setSource(const char* source) { mSource = source; }

  void Sound::setPitch(float pitch) { mPitch = pitch; }

  void Sound::setPan(float pan) { mPan = pan; }

  void Sound::setVolume(float volume) { mVolume = volume; }

  void Sound::play() {
#ifdef N64
    if (!mSource) {
      return;
    }

    mChannel = findFreeChannel();

    if (mChannel == -1) {
      return;
    }

    wav64_open(&mSound, mSource);
    wav64_play(&mSound, 0);
    float normalizedPan = (mPan + 1.0f) / 2.0f;
    mixer_ch_set_vol_pan(0, mVolume, normalizedPan);
#endif
  }

  void Sound::stop() { mixer_ch_stop(mChannel); }

  bool Sound::isPlaying() {
    return mChannel != -1 && mixer_ch_playing(mChannel);
  }
} // namespace Luna
