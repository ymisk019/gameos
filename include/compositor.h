#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include "types.h"

#define TASKBAR_HEIGHT 36

enum WallpaperTheme {
    WP_CYBER_NEON,
    WP_DEEP_SPACE,
    WP_SUNSET_RETRO,
    WP_MINIMAL_DARK
};

class DesktopCompositor {
private:
    static WallpaperTheme current_theme;
    static bool start_menu_open;

public:
    static void init();
    static void set_wallpaper(WallpaperTheme theme);
    static WallpaperTheme get_wallpaper();
    
    static void render_desktop();
    static void render_desktop_icons();
    static void render_taskbar();
    static void render_start_menu();
    static void render_cursor();
    
    static void toggle_start_menu();
    static bool is_start_menu_open();
    static void close_start_menu();

    static void handle_click(int x, int y, int btn);
    static void update_and_render();

    static void show_splash_screen();
};

#endif // COMPOSITOR_H
