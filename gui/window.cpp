#include "../include/window.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"

Window::Window(int id, const char* title, int x, int y, int w, int h)
    : id(id), x(x), y(y), width(w), height(h), is_minimized(false), is_active(false), is_closing(false), visible(true) {
    kstrncpy(this->title, title, sizeof(this->title) - 1);
}

void Window::draw_frame() {
    if (is_minimized || !visible) return;

    // Window shadow
    VesaDriver::fill_round_rect(x + 4, y + 4, width, height, 6, 0x0011161F);

    // Window main body background
    VesaDriver::fill_round_rect(x, y, width, height, 6, 0x001E222B);

    // Titlebar
    uint32_t title_top = is_active ? 0x000F4C81 : 0x002B303C;
    uint32_t title_bot = is_active ? 0x000A365C : 0x00212530;
    VesaDriver::fill_gradient_v(x, y, width, TITLE_BAR_HEIGHT, title_top, title_bot);

    // Window border
    uint32_t border_col = is_active ? 0x003A7BD5 : 0x003F4756;
    VesaDriver::draw_round_rect(x, y, width, height, 6, border_col);

    // Titlebar title text
    FontRenderer::draw_string(x + 12, y + 7, title, COLOR_WHITE, 1);

    // Close button [X]
    int btn_w = 18;
    int btn_h = 18;
    int close_x = x + width - 26;
    int close_y = y + 4;
    VesaDriver::fill_round_rect(close_x, close_y, btn_w, btn_h, 3, 0x00D9383A);
    FontRenderer::draw_string(close_x + 5, close_y + 4, "X", COLOR_WHITE, 1);

    // Minimize button [-]
    int min_x = close_x - 24;
    int min_y = close_y;
    VesaDriver::fill_round_rect(min_x, min_y, btn_w, btn_h, 3, 0x003A4454);
    FontRenderer::draw_string(min_x + 6, min_y + 3, "-", COLOR_WHITE, 1);

    // Client area content
    int cx = x + 2;
    int cy = y + TITLE_BAR_HEIGHT;
    int cw = width - 4;
    int ch = height - TITLE_BAR_HEIGHT - 2;

    on_render(cx, cy, cw, ch);
}

// WindowManager Static Members
Window* WindowManager::windows[MAX_WINDOWS];
int WindowManager::window_count = 0;
int WindowManager::active_window_id = -1;
int WindowManager::dragging_window_id = -1;
int WindowManager::drag_offset_x = 0;
int WindowManager::drag_offset_y = 0;

void WindowManager::init() {
    window_count = 0;
    active_window_id = -1;
    dragging_window_id = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i] = NULL;
    }
}

bool WindowManager::add_window(Window* win) {
    if (!win || window_count >= MAX_WINDOWS) return false;
    windows[window_count++] = win;
    bring_to_front(win->id);
    return true;
}

void WindowManager::remove_window(int id) {
    int idx = -1;
    for (int i = 0; i < window_count; i++) {
        if (windows[i] && windows[i]->id == id) {
            idx = i;
            break;
        }
    }
    if (idx != -1) {
        Window* w = windows[idx];
        w->on_close();
        // Shift left
        for (int i = idx; i < window_count - 1; i++) {
            windows[i] = windows[i + 1];
        }
        window_count--;
        windows[window_count] = NULL;

        if (active_window_id == id) {
            active_window_id = (window_count > 0) ? windows[window_count - 1]->id : -1;
        }
        delete w;
    }
}

Window* WindowManager::get_window(int id) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i] && windows[i]->id == id) {
            return windows[i];
        }
    }
    return NULL;
}

Window* WindowManager::get_active_window() {
    return get_window(active_window_id);
}

void WindowManager::set_active_window(int id) {
    active_window_id = id;
    for (int i = 0; i < window_count; i++) {
        if (windows[i]) {
            windows[i]->is_active = (windows[i]->id == id);
        }
    }
}

void WindowManager::bring_to_front(int id) {
    int idx = -1;
    for (int i = 0; i < window_count; i++) {
        if (windows[i] && windows[i]->id == id) {
            idx = i;
            break;
        }
    }
    if (idx != -1 && idx != window_count - 1) {
        Window* w = windows[idx];
        for (int i = idx; i < window_count - 1; i++) {
            windows[i] = windows[i + 1];
        }
        windows[window_count - 1] = w;
    }
    set_active_window(id);
}

int WindowManager::get_window_count() { return window_count; }
Window* WindowManager::get_window_at_index(int index) {
    if (index >= 0 && index < window_count) return windows[index];
    return NULL;
}

void WindowManager::handle_mouse_down(int x, int y, int btn) {
    // Top-to-bottom hit test
    for (int i = window_count - 1; i >= 0; i--) {
        Window* w = windows[i];
        if (!w || w->is_minimized || !w->visible) continue;

        if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height) {
            bring_to_front(w->id);

            // Title bar click?
            if (y < w->y + TITLE_BAR_HEIGHT) {
                // Close button test
                int close_x = w->x + w->width - 26;
                int close_y = w->y + 4;
                if (x >= close_x && x < close_x + 18 && y >= close_y && y < close_y + 18) {
                    Speaker::play_sfx(SFX_CLICK);
                    remove_window(w->id);
                    return;
                }

                // Minimize button test
                int min_x = close_x - 24;
                if (x >= min_x && x < min_x + 18 && y >= close_y && y < close_y + 18) {
                    Speaker::play_sfx(SFX_CLICK);
                    w->is_minimized = true;
                    return;
                }

                // Dragging window
                dragging_window_id = w->id;
                drag_offset_x = x - w->x;
                drag_offset_y = y - w->y;
            } else {
                // Client area click
                int cx = w->x + 2;
                int cy = w->y + TITLE_BAR_HEIGHT;
                w->on_mouse_click(x - cx, y - cy, btn);
            }
            return;
        }
    }
}

void WindowManager::handle_mouse_up(int, int, int) {
    dragging_window_id = -1;
}

void WindowManager::handle_mouse_move(int x, int y) {
    if (dragging_window_id != -1) {
        Window* w = get_window(dragging_window_id);
        if (w) {
            w->x = x - drag_offset_x;
            w->y = y - drag_offset_y;

            // Clamping to screen boundaries
            if (w->y < 0) w->y = 0;
            if (w->y > VesaDriver::get_height() - 50) w->y = VesaDriver::get_height() - 50;
        }
    } else {
        Window* top = get_active_window();
        if (top && !top->is_minimized && top->visible) {
            int cx = top->x + 2;
            int cy = top->y + TITLE_BAR_HEIGHT;
            if (x >= cx && x < cx + top->width && y >= cy && y < cy + top->height) {
                top->on_mouse_move(x - cx, y - cy);
            }
        }
    }
}

void WindowManager::handle_key(uint8_t scancode, char c) {
    Window* top = get_active_window();
    if (top && !top->is_minimized && top->visible) {
        top->on_key(scancode, c);
    }
}

void WindowManager::render_all() {
    for (int i = 0; i < window_count; i++) {
        if (windows[i] && !windows[i]->is_minimized && windows[i]->visible) {
            windows[i]->draw_frame();
        }
    }
}
