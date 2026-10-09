#ifndef WINDOW_H
#define WINDOW_H

#include "types.h"

#define MAX_WINDOWS 16
#define TITLE_BAR_HEIGHT 26

class Window {
public:
    int id;
    char title[64];
    int x, y, width, height;
    bool is_minimized;
    bool is_active;
    bool is_closing;
    bool visible;

    Window(int id, const char* title, int x, int y, int w, int h);
    virtual ~Window() {}

    virtual void on_render(int cx, int cy, int cw, int ch) = 0;
    virtual void on_key(uint8_t scancode, char c) {}
    virtual void on_mouse_click(int mx, int my, int btn) {}
    virtual void on_mouse_move(int mx, int my) {}
    virtual void on_close() {}

    void draw_frame();
};

class WindowManager {
private:
    static Window* windows[MAX_WINDOWS];
    static int window_count;
    static int active_window_id;
    static int dragging_window_id;
    static int drag_offset_x;
    static int drag_offset_y;

public:
    static void init();
    static bool add_window(Window* win);
    static void remove_window(int id);
    static Window* get_window(int id);
    static Window* get_active_window();
    static void set_active_window(int id);
    static void bring_to_front(int id);
    static int get_window_count();
    static Window* get_window_at_index(int index);

    static void handle_mouse_down(int x, int y, int btn);
    static void handle_mouse_up(int x, int y, int btn);
    static void handle_mouse_move(int x, int y);
    static void handle_key(uint8_t scancode, char c);

    static void render_all();
};

#endif // WINDOW_H
