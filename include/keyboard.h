#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

enum KeyCode {
    KEY_NONE = 0,
    KEY_ESC = 27,
    KEY_ENTER = 13,
    KEY_BACKSPACE = 8,
    KEY_TAB = 9,
    KEY_SPACE = 32,
    
    KEY_UP = 0x80,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LCTRL,
    KEY_RCTRL,
    KEY_LALT,
    KEY_RALT,
    
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    
    KEY_A = 'a', KEY_B = 'b', KEY_C = 'c', KEY_D = 'd', KEY_E = 'e',
    KEY_F = 'f', KEY_G = 'g', KEY_H = 'h', KEY_I = 'i', KEY_J = 'j',
    KEY_K = 'k', KEY_L = 'l', KEY_M = 'm', KEY_N = 'n', KEY_O = 'o',
    KEY_P = 'p', KEY_Q = 'q', KEY_R = 'r', KEY_S = 's', KEY_T = 't',
    KEY_U = 'u', KEY_V = 'v', KEY_W = 'w', KEY_X = 'x', KEY_Y = 'y',
    KEY_Z = 'z',
    KEY_0 = '0', KEY_1 = '1', KEY_2 = '2', KEY_3 = '3', KEY_4 = '4',
    KEY_5 = '5', KEY_6 = '6', KEY_7 = '7', KEY_8 = '8', KEY_9 = '9'
};

class Keyboard {
public:
    static void init();
    static void on_irq();
    static bool is_key_down(uint8_t scancode);
    static bool is_key_pressed(uint8_t scancode); // One-shot trigger
    static char get_last_char();
    static bool has_char();
    static char read_char();
    static bool shift_pressed();
    static bool ctrl_pressed();
    static bool alt_pressed();
};

#endif // KEYBOARD_H
