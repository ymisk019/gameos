#include "../include/mouse.h"
#include "../include/vesa.h"
#include "../include/idt.h"
#include "../include/pic.h"

#define MOUSE_DATA_PORT    0x60
#define MOUSE_STATUS_PORT  0x64
#define MOUSE_CMD_PORT     0x64

static int mouse_x = 400;
static int mouse_y = 300;
static int max_x = 800;
static int max_y = 600;

static uint8_t mouse_cycle = 0;
static uint8_t mouse_bytes[3];
static bool left_btn = false;
static bool right_btn = false;
static bool middle_btn = false;
static bool left_clicked = false;
static bool right_clicked = false;

static inline void mouse_wait_write() {
    uint32_t timeout = 100000;
    while (timeout-- && (inb(MOUSE_STATUS_PORT) & 2));
}

static inline void mouse_wait_read() {
    uint32_t timeout = 100000;
    while (timeout-- && !(inb(MOUSE_STATUS_PORT) & 1));
}

static void mouse_write(uint8_t write_val) {
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, 0xD4);
    mouse_wait_write();
    outb(MOUSE_DATA_PORT, write_val);
}

static uint8_t mouse_read() {
    mouse_wait_read();
    return inb(MOUSE_DATA_PORT);
}

static void mouse_isr(registers_t*) {
    Mouse::on_irq();
}

void Mouse::init(int screen_w, int screen_h) {
    max_x = screen_w;
    max_y = screen_h;
    mouse_x = screen_w / 2;
    mouse_y = screen_h / 2;

    // Enable auxiliary device
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, 0xA8);

    // Enable mouse IRQ12
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, 0x20);
    mouse_wait_read();
    uint8_t status = inb(MOUSE_DATA_PORT) | 2;
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, 0x60);
    mouse_wait_write();
    outb(MOUSE_DATA_PORT, status);

    // Set defaults
    mouse_write(0xF6);
    mouse_read(); // ACK

    // Enable packet streaming
    mouse_write(0xF4);
    mouse_read(); // ACK

    // Clear IRQ mask for IRQ12
    pic_clear_mask(12);

    register_interrupt_handler(44, mouse_isr);
}

void Mouse::on_irq() {
    uint8_t status = inb(MOUSE_STATUS_PORT);
    if (!(status & 1)) return;

    uint8_t data = inb(MOUSE_DATA_PORT);

    switch (mouse_cycle) {
        case 0:
            if (data & 0x08) { // Bit 3 must be 1 in first byte
                mouse_bytes[0] = data;
                mouse_cycle = 1;
            }
            break;
        case 1:
            mouse_bytes[1] = data;
            mouse_cycle = 2;
            break;
        case 2:
            mouse_bytes[2] = data;
            mouse_cycle = 0;

            int dx = (int8_t)mouse_bytes[1];
            int dy = (int8_t)mouse_bytes[2];

            // Invert dy because PS/2 mouse Y is upwards
            mouse_x += dx;
            mouse_y -= dy;

            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x >= max_x) mouse_x = max_x - 1;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y >= max_y) mouse_y = max_y - 1;

            bool new_left = (mouse_bytes[0] & 0x01) != 0;
            bool new_right = (mouse_bytes[0] & 0x02) != 0;
            middle_btn = (mouse_bytes[0] & 0x04) != 0;

            if (new_left && !left_btn) left_clicked = true;
            if (new_right && !right_btn) right_clicked = true;

            left_btn = new_left;
            right_btn = new_right;
            break;
    }
}

int Mouse::get_x() { return mouse_x; }
int Mouse::get_y() { return mouse_y; }
bool Mouse::is_left_down() { return left_btn; }
bool Mouse::is_right_down() { return right_btn; }
bool Mouse::is_middle_down() { return middle_btn; }

bool Mouse::is_left_clicked() {
    if (left_clicked) {
        left_clicked = false;
        return true;
    }
    return false;
}

bool Mouse::is_right_clicked() {
    if (right_clicked) {
        right_clicked = false;
        return true;
    }
    return false;
}

void Mouse::update_bounds(int w, int h) {
    max_x = w;
    max_y = h;
}

void Mouse::set_position(int x, int y) {
    mouse_x = x;
    mouse_y = y;
}

// Draw a sharp, high-contrast OS cursor
void Mouse::draw_cursor(int x, int y) {
    static const char* cursor_bitmap[] = {
        "X               ",
        "XX              ",
        "X.X             ",
        "X..X            ",
        "X...X           ",
        "X....X          ",
        "X.....X         ",
        "X......X        ",
        "X.......X       ",
        "X........X      ",
        "X.....XXXX      ",
        "X..X..X         ",
        "X.X X..X        ",
        "XX   X..X       ",
        "X     X..X      ",
        "       XX       "
    };

    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 16; cx++) {
            char pixel = cursor_bitmap[cy][cx];
            if (pixel == 'X') {
                VesaDriver::put_pixel(x + cx, y + cy, COLOR_BLACK);
            } else if (pixel == '.') {
                VesaDriver::put_pixel(x + cx, y + cy, COLOR_WHITE);
            }
        }
    }
}
