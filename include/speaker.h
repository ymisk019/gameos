#ifndef SPEAKER_H
#define SPEAKER_H

#include "types.h"

enum SoundEffect {
    SFX_CLICK,
    SFX_JUMP,
    SFX_LASER,
    SFX_EXPLOSION,
    SFX_COIN,
    SFX_ENGINE,
    SFX_CRASH,
    SFX_VICTORY,
    SFX_GAMEOVER,
    SFX_STARTUP
};

class Speaker {
public:
    static void init();
    static void play_tone(uint32_t freq);
    static void stop();
    static void beep(uint32_t freq, uint32_t duration_ms);
    static void play_sfx(SoundEffect sfx);
    static void set_enabled(bool enabled);
    static bool is_enabled();
    static void toggle_sound();
};

#endif // SPEAKER_H
