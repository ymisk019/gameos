#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "types.h"

struct GamepadState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool btn_a;       // Primary action / Jump / Shoot
    bool btn_b;       // Secondary action / Nitro / Back
    bool btn_x;       // Action 3 / Special
    bool btn_y;       // Action 4 / Inventory
    bool btn_start;   // Pause / Menu
    bool btn_select;  // Map / Reset
};

class Gamepad {
public:
    static void init();
    static void update();
    static GamepadState get_state();
    static bool is_button_down(int btn);
    static bool is_button_pressed(int btn);
};

#endif // GAMEPAD_H
