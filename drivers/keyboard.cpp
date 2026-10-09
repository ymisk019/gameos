#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/types.h"

#define KBD_DATA_PORT 0x60
#define KBD_BUFFER_SIZE 64

static bool key_states[256];
static bool key_pressed_events[256];
static char char_buffer[KBD_BUFFER_SIZE];
static int buf_head = 0;
static int buf_tail = 0;
static bool shift_active = false;
static bool ctrl_active = false;
static bool alt_active = false;

static const char scancode_to_ascii_lower[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // Ctrl
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // Right shift
    '*', 0, ' '
};

static const char scancode_to_ascii_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, // Ctrl
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, // Left shift
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, // Right shift
    '*', 0, ' '
};

static void keyboard_isr(registers_t*) {
    Keyboard::on_irq();
}

void Keyboard::init() {
    kmemset(key_states, 0, sizeof(key_states));
    kmemset(key_pressed_events, 0, sizeof(key_pressed_events));
    buf_head = 0;
    buf_tail = 0;

    // Register IRQ1 (interrupt 33)
    register_interrupt_handler(33, keyboard_isr);
}

void Keyboard::on_irq() {
    uint8_t scancode = inb(KBD_DATA_PORT);

    if (scancode & 0x80) {
        // Key release
        uint8_t released = scancode & 0x7F;
        key_states[released] = false;
        if (released == 0x2A || released == 0x36) shift_active = false;
        if (released == 0x1D) ctrl_active = false;
        if (released == 0x38) alt_active = false;
    } else {
        // Key press
        key_states[scancode] = true;
        key_pressed_events[scancode] = true;

        if (scancode == 0x2A || scancode == 0x36) shift_active = true;
        if (scancode == 0x1D) ctrl_active = true;
        if (scancode == 0x38) alt_active = true;

        char c = 0;
        if (scancode < 128) {
            c = shift_active ? scancode_to_ascii_upper[scancode] : scancode_to_ascii_lower[scancode];
        }

        if (c != 0) {
            int next_head = (buf_head + 1) % KBD_BUFFER_SIZE;
            if (next_head != buf_tail) {
                char_buffer[buf_head] = c;
                buf_head = next_head;
            }
        }
    }
}

bool Keyboard::is_key_down(uint8_t scancode) {
    return key_states[scancode];
}

bool Keyboard::is_key_pressed(uint8_t scancode) {
    if (key_pressed_events[scancode]) {
        key_pressed_events[scancode] = false;
        return true;
    }
    return false;
}

bool Keyboard::has_char() {
    return buf_head != buf_tail;
}

char Keyboard::read_char() {
    if (buf_head == buf_tail) return 0;
    char c = char_buffer[buf_tail];
    buf_tail = (buf_tail + 1) % KBD_BUFFER_SIZE;
    return c;
}

char Keyboard::get_last_char() {
    return read_char();
}

bool Keyboard::shift_pressed() { return shift_active; }
bool Keyboard::ctrl_pressed()  { return ctrl_active; }
bool Keyboard::alt_pressed()   { return alt_active; }
