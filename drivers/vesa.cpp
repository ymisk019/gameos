#include "../include/vesa.h"
#include "../include/memory.h"

static uint32_t* lfb = NULL;
static uint32_t* backbuffer = NULL;
static int screen_width = 800;
static int screen_height = 600;
static int screen_bpp = 32;

void VesaDriver::init(uint32_t* lfb_addr, int width, int height, int bpp) {
    lfb = lfb_addr;
    screen_width = width;
    screen_height = height;
    screen_bpp = bpp;

    // Allocate backbuffer for double-buffering
    size_t buffer_size = width * height * sizeof(uint32_t);
    backbuffer = (uint32_t*)kmalloc_aligned(buffer_size, 16);
    if (!backbuffer) {
        // Fallback directly to LFB if heap memory is tight
        backbuffer = lfb_addr;
    } else {
        clear(COLOR_BLACK);
    }
}

int VesaDriver::get_width() { return screen_width; }
int VesaDriver::get_height() { return screen_height; }
int VesaDriver::get_bpp() { return screen_bpp; }
uint32_t* VesaDriver::get_backbuffer() { return backbuffer; }
uint32_t* VesaDriver::get_framebuffer() { return lfb; }

void VesaDriver::clear(uint32_t color) {
    if (!backbuffer) return;
    int total_pixels = screen_width * screen_height;
    for (int i = 0; i < total_pixels; i++) {
        backbuffer[i] = color;
    }
}

void VesaDriver::put_pixel(int x, int y, uint32_t color) {
    if (!backbuffer) return;
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;
    backbuffer[y * screen_width + x] = color;
}

uint32_t VesaDriver::get_pixel(int x, int y) {
    if (!backbuffer) return 0;
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return 0;
    return backbuffer[y * screen_width + x];
}

void VesaDriver::draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void VesaDriver::draw_rect(int x, int y, int w, int h, uint32_t color) {
    draw_line(x, y, x + w - 1, y, color);
    draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    draw_line(x, y, x, y + h - 1, color);
    draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void VesaDriver::fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (!backbuffer) return;
    int x1 = (x < 0) ? 0 : x;
    int y1 = (y < 0) ? 0 : y;
    int x2 = (x + w > screen_width) ? screen_width : (x + w);
    int y2 = (y + h > screen_height) ? screen_height : (y + h);

    if (x1 >= x2 || y1 >= y2) return;

    for (int cy = y1; cy < y2; cy++) {
        uint32_t* row = &backbuffer[cy * screen_width + x1];
        int count = x2 - x1;
        for (int i = 0; i < count; i++) {
            row[i] = color;
        }
    }
}

void VesaDriver::fill_gradient_v(int x, int y, int w, int h, uint32_t c1, uint32_t c2) {
    if (h <= 0) return;
    int r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    int r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;

    for (int i = 0; i < h; i++) {
        int r = r1 + ((r2 - r1) * i) / h;
        int g = g1 + ((g2 - g1) * i) / h;
        int b = b1 + ((b2 - b1) * i) / h;
        uint32_t row_color = rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
        fill_rect(x, y + i, w, 1, row_color);
    }
}

void VesaDriver::fill_gradient_h(int x, int y, int w, int h, uint32_t c1, uint32_t c2) {
    if (w <= 0) return;
    int r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    int r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;

    for (int i = 0; i < w; i++) {
        int r = r1 + ((r2 - r1) * i) / w;
        int g = g1 + ((g2 - g1) * i) / w;
        int b = b1 + ((b2 - b1) * i) / w;
        uint32_t col_color = rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
        fill_rect(x + i, y, 1, h, col_color);
    }
}

void VesaDriver::draw_circle(int xc, int yc, int r, uint32_t color) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (y >= x) {
        put_pixel(xc + x, yc + y, color);
        put_pixel(xc - x, yc + y, color);
        put_pixel(xc + x, yc - y, color);
        put_pixel(xc - x, yc - y, color);
        put_pixel(xc + y, yc + x, color);
        put_pixel(xc - y, yc + x, color);
        put_pixel(xc + y, yc - x, color);
        put_pixel(xc - y, yc - x, color);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void VesaDriver::fill_circle(int xc, int yc, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                put_pixel(xc + x, yc + y, color);
            }
        }
    }
}

void VesaDriver::fill_round_rect(int x, int y, int w, int h, int r, uint32_t color) {
    if (r <= 0) {
        fill_rect(x, y, w, h, color);
        return;
    }
    fill_rect(x + r, y, w - 2 * r, h, color);
    fill_rect(x, y + r, r, h - 2 * r, color);
    fill_rect(x + w - r, y + r, r, h - 2 * r, color);
    fill_circle(x + r, y + r, r, color);
    fill_circle(x + w - r, y + r, r, color);
    fill_circle(x + r, y + h - r, r, color);
    fill_circle(x + w - r, y + h - r, r, color);
}

void VesaDriver::draw_round_rect(int x, int y, int w, int h, int r, uint32_t color) {
    draw_line(x + r, y, x + w - r, y, color);
    draw_line(x + r, y + h - 1, x + w - r, y + h - 1, color);
    draw_line(x, y + r, x, y + h - r, color);
    draw_line(x + w - 1, y + r, x + w - 1, y + h - r, color);
    // Approximate corner curves
    draw_circle(x + r, y + r, r, color);
    draw_circle(x + w - r, y + r, r, color);
    draw_circle(x + r, y + h - r, r, color);
    draw_circle(x + w - r, y + h - r, r, color);
}

void VesaDriver::flip() {
    if (!lfb || !backbuffer || lfb == backbuffer) return;
    kmemcpy(lfb, backbuffer, screen_width * screen_height * sizeof(uint32_t));
}

void VesaDriver::flip_rect(int x, int y, int w, int h) {
    if (!lfb || !backbuffer || lfb == backbuffer) return;
    int x1 = (x < 0) ? 0 : x;
    int y1 = (y < 0) ? 0 : y;
    int x2 = (x + w > screen_width) ? screen_width : (x + w);
    int y2 = (y + h > screen_height) ? screen_height : (y + h);

    for (int cy = y1; cy < y2; cy++) {
        kmemcpy(&lfb[cy * screen_width + x1], &backbuffer[cy * screen_width + x1], (x2 - x1) * sizeof(uint32_t));
    }
}
