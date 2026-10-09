#include "../include/gamepad.h"
#include "../include/keyboard.h"
#include "../include/types.h"

#define GAMEPORT_PORT 0x201

static GamepadState current_state;
static GamepadState prev_state;

void Gamepad::init() {
    kmemset(&current_state, 0, sizeof(GamepadState));
    kmemset(&prev_state, 0, sizeof(GamepadState));
}

void Gamepad::update() {
    prev_state = current_state;

    // Check hardware gameport 0x201
    // Bit 4: Button 1, Bit 5: Button 2, Bit 6: Button 3, Bit 7: Button 4
    uint8_t gp = inb(GAMEPORT_PORT);
    bool gp_btn1 = !(gp & 0x10);
    bool gp_btn2 = !(gp & 0x20);

    // Map keyboard inputs as universal gamepad:
    // Arrows / WASD -> D-Pad
    // Z / Space / Enter -> Button A
    // X / Shift -> Button B
    // C -> Button X
    // V -> Button Y
    // Return / Esc -> Start / Select

    bool up = Keyboard::is_key_down(0x48) || Keyboard::is_key_down(0x11);     // Up arrow or 'W'
    bool down = Keyboard::is_key_down(0x50) || Keyboard::is_key_down(0x1F);   // Down arrow or 'S'
    bool left = Keyboard::is_key_down(0x4B) || Keyboard::is_key_down(0x1E);   // Left arrow or 'A'
    bool right = Keyboard::is_key_down(0x4D) || Keyboard::is_key_down(0x20);  // Right arrow or 'D'
    bool btn_a = Keyboard::is_key_down(0x2C) || Keyboard::is_key_down(0x39) || Keyboard::is_key_down(0x1C) || gp_btn1; // 'Z', Space, Enter
    bool btn_b = Keyboard::is_key_down(0x2D) || Keyboard::is_key_down(0x2A) || gp_btn2; // 'X', LShift
    bool btn_x = Keyboard::is_key_down(0x2E); // 'C'
    bool btn_y = Keyboard::is_key_down(0x2F); // 'V'
    bool start = Keyboard::is_key_down(0x01); // Esc
    bool select = Keyboard::is_key_down(0x0F); // Tab

    current_state.up = up;
    current_state.down = down;
    current_state.left = left;
    current_state.right = right;
    current_state.btn_a = btn_a;
    current_state.btn_b = btn_b;
    current_state.btn_x = btn_x;
    current_state.btn_y = btn_y;
    current_state.btn_start = start;
    current_state.btn_select = select;
}

GamepadState Gamepad::get_state() {
    return current_state;
}

bool Gamepad::is_button_down(int btn) {
    switch (btn) {
        case 0: return current_state.btn_a;
        case 1: return current_state.btn_b;
        case 2: return current_state.btn_x;
        case 3: return current_state.btn_y;
        case 4: return current_state.up;
        case 5: return current_state.down;
        case 6: return current_state.left;
        case 7: return current_state.right;
        default: return false;
    }
}

bool Gamepad::is_button_pressed(int btn) {
    switch (btn) {
        case 0: return current_state.btn_a && !prev_state.btn_a;
        case 1: return current_state.btn_b && !prev_state.btn_b;
        case 2: return current_state.btn_x && !prev_state.btn_x;
        case 3: return current_state.btn_y && !prev_state.btn_y;
        case 4: return current_state.up && !prev_state.up;
        case 5: return current_state.down && !prev_state.down;
        case 6: return current_state.left && !prev_state.left;
        case 7: return current_state.right && !prev_state.right;
        default: return false;
    }
}
