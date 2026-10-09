#include "../include/speaker.h"
#include "../include/pit.h"

static bool sound_enabled = true;

void Speaker::init() {
    sound_enabled = true;
    stop();
}

void Speaker::play_tone(uint32_t freq) {
    if (!sound_enabled || freq == 0) {
        stop();
        return;
    }

    uint32_t div = 1193180 / freq;
    
    // Set PIT channel 2 mode 3
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(div & 0xFF));
    outb(0x42, (uint8_t)((div >> 8) & 0xFF));

    // Enable speaker gate via port 0x61
    uint8_t tmp = inb(0x61);
    if ((tmp & 3) != 3) {
        outb(0x61, tmp | 3);
    }
}

void Speaker::stop() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void Speaker::beep(uint32_t freq, uint32_t duration_ms) {
    if (!sound_enabled) return;
    play_tone(freq);
    sleep_ms(duration_ms);
    stop();
}

void Speaker::set_enabled(bool enabled) {
    sound_enabled = enabled;
    if (!sound_enabled) stop();
}

bool Speaker::is_enabled() {
    return sound_enabled;
}

void Speaker::toggle_sound() {
    set_enabled(!sound_enabled);
}

void Speaker::play_sfx(SoundEffect sfx) {
    if (!sound_enabled) return;

    switch (sfx) {
        case SFX_CLICK:
            play_tone(1200);
            sleep_ms(15);
            stop();
            break;

        case SFX_JUMP:
            // Rising pitch sweep
            for (uint32_t f = 300; f < 800; f += 60) {
                play_tone(f);
                sleep_ms(8);
            }
            stop();
            break;

        case SFX_LASER:
            // Descending fast sweep
            for (uint32_t f = 1500; f > 400; f -= 120) {
                play_tone(f);
                sleep_ms(6);
            }
            stop();
            break;

        case SFX_EXPLOSION:
            // Low rumbling noise imitation
            for (int i = 0; i < 5; i++) {
                play_tone(120 + (i % 2) * 50);
                sleep_ms(25);
            }
            stop();
            break;

        case SFX_COIN:
            // Classic two-tone chime
            play_tone(988); // B5
            sleep_ms(50);
            play_tone(1319); // E6
            sleep_ms(120);
            stop();
            break;

        case SFX_ENGINE:
            // Quick engine hum pitch
            play_tone(150);
            sleep_ms(10);
            stop();
            break;

        case SFX_CRASH:
            play_tone(80);
            sleep_ms(80);
            play_tone(50);
            sleep_ms(120);
            stop();
            break;

        case SFX_VICTORY:
            // Fanfare
            beep(523, 70);  // C5
            beep(659, 70);  // E5
            beep(784, 70);  // G5
            beep(1046, 200); // C6
            break;

        case SFX_GAMEOVER:
            beep(440, 100);
            beep(370, 100);
            beep(311, 100);
            beep(261, 250);
            break;

        case SFX_STARTUP:
            // Yazan Game OS startup signature jingle
            beep(440, 60);  // A4
            beep(554, 60);  // C#5
            beep(659, 60);  // E5
            beep(880, 180); // A5
            break;
    }
}
