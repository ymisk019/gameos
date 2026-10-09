#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"

class Mouse {
public:
    static void init(int screen_w, int screen_h);
    static void on_irq();
    static int get_x();
    static int get_y();
    static bool is_left_down();
    static bool is_right_down();
    static bool is_middle_down();
    static bool is_left_clicked();
    static bool is_right_clicked();
    static void update_bounds(int w, int h);
    static void set_position(int x, int y);
    static void draw_cursor(int x, int y);
};

#endif // MOUSE_H
