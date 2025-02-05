#pragma once

#include <libluna/SoundSource.hpp>

#ifdef N64
#include <libdragon.h>
#endif

namespace Luna {
  class Sound {
    public:
#ifdef N64
    void setSource(const char* source);
#else
    void setSource(SoundSource* source);
#endif

    void setPitch(float pitch);

    /**
     * @brief Set the pan of the sound.
     *
     * @param pan The pan value. -1 is left, 1 is right, 0 is center.
     */
    void setPan(float pan);

    void setVolume(float volume);

    void play();

    void stop();

    bool isPlaying();

    private:
#ifdef N64
    const char* mSource;
    wav64_t mSound;
#else
    SoundSource* mSource;
#endif
    float mPitch{1.0f};
    float mPan{0.0f};
    float mVolume{1.0f};
    int mChannel{-1};
  };
} // namespace Luna
