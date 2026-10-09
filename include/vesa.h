#ifndef VESA_H
#define VESA_H

#include "types.h"

#define COLOR_BLACK       0x00000000
#define COLOR_WHITE       0x00FFFFFF
#define COLOR_GRAY        0x00808080
#define COLOR_DARK_GRAY   0x002D3139
#define COLOR_LIGHT_GRAY  0x00D0D4DC
#define COLOR_RED         0x00E04040
#define COLOR_GREEN       0x002ECC71
#define COLOR_BLUE        0x003498DB
#define COLOR_YELLOW      0x00F1C40F
#define COLOR_PURPLE      0x009B59B6
#define COLOR_CYAN        0x001ABC9C
#define COLOR_ORANGE      0x00E67E22
#define COLOR_NAVY        0x001A2530
#define COLOR_ACCENT      0x0000A8FF

static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static inline uint32_t blend_color(uint32_t bg, uint32_t fg, uint8_t alpha) {
    if (alpha == 255) return fg;
    if (alpha == 0) return bg;
    uint32_t rb = bg & 0x00FF00FF;
    uint32_t g  = bg & 0x0000FF00;
    uint32_t rf = fg & 0x00FF00FF;
    uint32_t gf = fg & 0x0000FF00;
    rb += ((rf - rb) * alpha) >> 8;
    g  += ((gf - g) * alpha) >> 8;
    return (rb & 0x00FF00FF) | (g & 0x0000FF00);
}

class VesaDriver {
public:
    static void init(uint32_t* lfb_addr, int width, int height, int bpp);
    static int get_width();
    static int get_height();
    static int get_bpp();
    static uint32_t* get_backbuffer();
    static uint32_t* get_framebuffer();

    static void clear(uint32_t color);
    static void put_pixel(int x, int y, uint32_t color);
    static uint32_t get_pixel(int x, int y);
    static void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    static void draw_rect(int x, int y, int w, int h, uint32_t color);
    static void fill_rect(int x, int y, int w, int h, uint32_t color);
    static void fill_gradient_v(int x, int y, int w, int h, uint32_t c1, uint32_t c2);
    static void fill_gradient_h(int x, int y, int w, int h, uint32_t c1, uint32_t c2);
    static void draw_circle(int xc, int yc, int r, uint32_t color);
    static void fill_circle(int xc, int yc, int r, uint32_t color);
    static void fill_round_rect(int x, int y, int w, int h, int r, uint32_t color);
    static void draw_round_rect(int x, int y, int w, int h, int r, uint32_t color);
    
    static void flip();
    static void flip_rect(int x, int y, int w, int h);
};

#endif // VESA_H
