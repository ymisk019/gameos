#include "../include/compositor.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/mouse.h"
#include "../include/speaker.h"
#include "../include/rtc.h"
#include "../include/window.h"
#include "../include/power.h"
#include "../include/apps.h"
#include "../include/games.h"

WallpaperTheme DesktopCompositor::current_theme = WP_CYBER_NEON;
bool DesktopCompositor::start_menu_open = false;

static int window_id_counter = 100;

void DesktopCompositor::init() {
    current_theme = WP_CYBER_NEON;
    start_menu_open = false;
}

void DesktopCompositor::set_wallpaper(WallpaperTheme theme) {
    current_theme = theme;
}

WallpaperTheme DesktopCompositor::get_wallpaper() {
    return current_theme;
}

void DesktopCompositor::toggle_start_menu() {
    start_menu_open = !start_menu_open;
}

bool DesktopCompositor::is_start_menu_open() {
    return start_menu_open;
}

void DesktopCompositor::close_start_menu() {
    start_menu_open = false;
}

void DesktopCompositor::render_desktop() {
    int w = VesaDriver::get_width();
    int h = VesaDriver::get_height();

    switch (current_theme) {
        case WP_CYBER_NEON: {
            // High-tech dark gradient
            VesaDriver::fill_gradient_v(0, 0, w, h, 0x000F172A, 0x00020617);

            // Cyber perspective grid lines on lower half
            int grid_y = h / 2;
            for (int y = grid_y; y < h - TASKBAR_HEIGHT; y += 24) {
                VesaDriver::draw_line(0, y, w, y, 0x001E293B);
            }
            for (int x = 0; x < w; x += 60) {
                VesaDriver::draw_line(x, grid_y, x, h - TASKBAR_HEIGHT, 0x00131E2E);
            }

            // Big elegant background watermark
            FontRenderer::draw_string(w / 2 - 160, h / 2 - 80, "YAZAN GAME OS", 0x001A2536, 3);
            break;
        }
        case WP_DEEP_SPACE: {
            VesaDriver::fill_gradient_v(0, 0, w, h, 0x001A0B2E, 0x000B0014);
            // Nebula glow
            VesaDriver::fill_circle(w / 2, h / 3, 120, 0x002D124D);
            VesaDriver::fill_circle(w / 2 + 80, h / 3 + 20, 90, 0x003D1A68);
            // Stars
            for (int i = 0; i < 40; i++) {
                int sx = (i * 137) % w;
                int sy = (i * 89) % (h - TASKBAR_HEIGHT);
                VesaDriver::put_pixel(sx, sy, COLOR_WHITE);
            }
            break;
        }
        case WP_SUNSET_RETRO: {
            VesaDriver::fill_gradient_v(0, 0, w, h / 2, 0x00240046, 0x007B2CBF);
            VesaDriver::fill_gradient_v(0, h / 2, w, h / 2, 0x00FF007F, 0x0010002B);
            // Retro Sun
            VesaDriver::fill_circle(w / 2, h / 2 - 20, 60, 0x00FFBE0B);
            break;
        }
        case WP_MINIMAL_DARK: {
            VesaDriver::clear(0x00121418);
            VesaDriver::draw_rect(20, 20, w - 40, h - TASKBAR_HEIGHT - 40, 0x001F242C);
            break;
        }
    }
}

struct DesktopIcon {
    int x, y;
    const char* label_ar;
    const char* label_en;
    uint32_t color;
    int action; // 1..10
};

static const DesktopIcon desktop_icons[] = {
    { 24,  24, "مدير الملفات",  "File Manager",  0x003498DB, 1 },
    { 24, 110, "الإعدادات",     "Settings",      0x0095A5A6, 2 },
    { 24, 196, "متجر الألعاب",  "Game Store",    0x009B59B6, 3 },
    { 24, 282, "المغامرات",     "Adventure",     0x00E67E22, 4 },
    { 24, 368, "سباق سيارات",   "Desert Speed",  0x00E74C3C, 5 },
    { 120, 24, "منصات وقفز",    "Cyber Jumper",  0x002ECC71, 6 },
    { 120, 110, "حامي المجرة",  "Space Shooter", 0x0000D2D3, 7 },
    { 120, 196, "مكعبات الذكاء","Mind Blocks",   0x00F39C12, 8 },
};
static const int icon_count = 8;

void DesktopCompositor::render_desktop_icons() {
    for (int i = 0; i < icon_count; i++) {
        const DesktopIcon& icon = desktop_icons[i];

        // Icon rounded square box
        VesaDriver::fill_round_rect(icon.x, icon.y, 52, 52, 10, icon.color);
        VesaDriver::draw_round_rect(icon.x, icon.y, 52, 52, 10, COLOR_WHITE);

        // Icon emblem letter
        char emblem[2] = { "FMSAGCSB"[i], '\0' };
        FontRenderer::draw_string(icon.x + 20, icon.y + 18, emblem, COLOR_WHITE, 2);

        // Label below icon
        const char* label = (FontRenderer::get_language() == LANG_ARABIC) ? icon.label_ar : icon.label_en;
        int lbl_w = FontRenderer::get_utf8_width(label, 1);
        int lbl_x = icon.x + 26 - (lbl_w / 2);
        if (lbl_x < 4) lbl_x = 4;
        
        // Label shadow pill
        VesaDriver::fill_round_rect(lbl_x - 4, icon.y + 56, lbl_w + 8, 14, 4, 0x000A0E14);
        FontRenderer::draw_bilingual_text(lbl_x, icon.y + 59, icon.label_ar, icon.label_en, COLOR_WHITE, 1);
    }
}

void DesktopCompositor::render_taskbar() {
    int w = VesaDriver::get_width();
    int h = VesaDriver::get_height();
    int tb_y = h - TASKBAR_HEIGHT;

    // Taskbar background
    VesaDriver::fill_gradient_v(0, tb_y, w, TASKBAR_HEIGHT, 0x001B222C, 0x000F141C);
    VesaDriver::draw_line(0, tb_y, w, tb_y, 0x002C3E50);

    // Start Button
    uint32_t start_col = start_menu_open ? 0x000080FF : 0x000066CC;
    VesaDriver::fill_round_rect(6, tb_y + 4, 90, TASKBAR_HEIGHT - 8, 6, start_col);
    VesaDriver::draw_round_rect(6, tb_y + 4, 90, TASKBAR_HEIGHT - 8, 6, 0x003399FF);

    // Start Button text
    const char* start_txt = (FontRenderer::get_language() == LANG_ARABIC) ? "ابدأ" : "Start";
    FontRenderer::draw_bilingual_text(26, tb_y + 11, "ابدأ", "Start", COLOR_WHITE, 1);
    FontRenderer::draw_string(14, tb_y + 10, "Y", COLOR_WHITE, 1);

    // Running window buttons
    int tab_x = 104;
    int tab_w = 140;
    int win_count = WindowManager::get_window_count();
    for (int i = 0; i < win_count; i++) {
        Window* win = WindowManager::get_window_at_index(i);
        if (!win) continue;

        uint32_t tab_bg = win->is_active ? 0x002A3649 : 0x00161C24;
        uint32_t tab_border = win->is_active ? 0x0000A8FF : 0x0027313E;

        VesaDriver::fill_round_rect(tab_x, tb_y + 4, tab_w, TASKBAR_HEIGHT - 8, 5, tab_bg);
        VesaDriver::draw_round_rect(tab_x, tb_y + 4, tab_w, TASKBAR_HEIGHT - 8, 5, tab_border);

        // Truncate title if long
        char short_title[16];
        kstrncpy(short_title, win->title, 14);
        short_title[14] = '\0';
        FontRenderer::draw_string(tab_x + 8, tb_y + 11, short_title, COLOR_WHITE, 1);

        tab_x += tab_w + 6;
        if (tab_x + tab_w > w - 160) break; // Keep room for system tray
    }

    // System Tray (Right side)
    // 1. Language switcher toggle button
    int lang_x = w - 150;
    VesaDriver::fill_round_rect(lang_x, tb_y + 5, 42, TASKBAR_HEIGHT - 10, 4, 0x00222D3D);
    const char* lang_label = (FontRenderer::get_language() == LANG_ARABIC) ? "AR" : "EN";
    FontRenderer::draw_string(lang_x + 12, tb_y + 10, lang_label, 0x0000D2D3, 1);

    // 2. Sound indicator button
    int sound_x = w - 100;
    VesaDriver::fill_round_rect(sound_x, tb_y + 5, 36, TASKBAR_HEIGHT - 10, 4, 0x00222D3D);
    const char* sound_lbl = Speaker::is_enabled() ? "SND" : "MUT";
    uint32_t sound_col = Speaker::is_enabled() ? 0x002ECC71 : 0x00E74C3C;
    FontRenderer::draw_string(sound_x + 6, tb_y + 10, sound_lbl, sound_col, 1);

    // 3. Real-time Clock
    int clock_x = w - 56;
    char time_str[8];
    RTC::format_time_string(time_str);
    FontRenderer::draw_string(clock_x, tb_y + 10, time_str, COLOR_WHITE, 1);
}

void DesktopCompositor::render_start_menu() {
    if (!start_menu_open) return;

    int sm_w = 260;
    int sm_h = 340;
    int sm_x = 6;
    int sm_y = VesaDriver::get_height() - TASKBAR_HEIGHT - sm_h - 4;

    // Start Menu Body
    VesaDriver::fill_round_rect(sm_x, sm_y, sm_w, sm_h, 8, 0x00141922);
    VesaDriver::draw_round_rect(sm_x, sm_y, sm_w, sm_h, 8, 0x002F3A4C);

    // Header banner
    VesaDriver::fill_gradient_h(sm_x + 1, sm_y + 1, sm_w - 2, 46, 0x000052D4, 0x004364F7);
    FontRenderer::draw_string(sm_x + 14, sm_y + 12, "YAZAN GAME OS", COLOR_WHITE, 1);
    FontRenderer::draw_bilingual_text(sm_x + 14, sm_y + 26, "أقوى نظام للألعاب", "Ultimate Gaming OS", 0x00A0D2EB, 1);

    // Menu Items
    struct MenuItem {
        const char* label_ar;
        const char* label_en;
        uint32_t color;
        int action;
    };

    static const MenuItem items[] = {
        { "مغامرات يزن",       "Yazan Quest",       0x00E67E22, 4 },
        { "سباق الصحراء",      "Desert Speed",      0x00E74C3C, 5 },
        { "قفزة السايبر",      "Cyber Jumper",      0x002ECC71, 6 },
        { "حامي المجرة",       "Galactic Defender", 0x0000D2D3, 7 },
        { "مكعبات الذكاء",     "Mind Blocks",       0x00F39C12, 8 },
        { "متجر التطبيقات",    "Game Store",        0x009B59B6, 3 },
        { "مدير الملفات",      "File Manager",      0x003498DB, 1 },
        { "الإعدادات",         "Settings",          0x0095A5A6, 2 },
    };

    int item_y = sm_y + 54;
    for (int i = 0; i < 8; i++) {
        // Item button
        VesaDriver::fill_round_rect(sm_x + 10, item_y, sm_w - 20, 24, 4, 0x001D2430);
        VesaDriver::fill_circle(sm_x + 22, item_y + 12, 5, items[i].color);
        FontRenderer::draw_bilingual_text(sm_x + 36, item_y + 6, items[i].label_ar, items[i].label_en, COLOR_WHITE, 1);
        item_y += 28;
    }

    // Bottom Power Actions: Shutdown & Restart
    int pwr_y = sm_y + sm_h - 32;
    VesaDriver::fill_round_rect(sm_x + 10, pwr_y, 115, 24, 4, 0x00C0392B);
    FontRenderer::draw_bilingual_text(sm_x + 22, pwr_y + 6, "إطفاء", "Shut Down", COLOR_WHITE, 1);

    VesaDriver::fill_round_rect(sm_x + 135, pwr_y, 115, 24, 4, 0x002980B9);
    FontRenderer::draw_bilingual_text(sm_x + 147, pwr_y + 6, "إعادة تشغيل", "Restart", COLOR_WHITE, 1);
}

void DesktopCompositor::render_cursor() {
    Mouse::draw_cursor(Mouse::get_x(), Mouse::get_y());
}

static void launch_app(int action) {
    Speaker::play_sfx(SFX_CLICK);
    int new_id = window_id_counter++;

    switch (action) {
        case 1: // File Manager
            WindowManager::add_window(new FileManagerWindow(new_id));
            break;
        case 2: // Settings
            WindowManager::add_window(new SettingsWindow(new_id));
            break;
        case 3: // Game Store
            WindowManager::add_window(new GameManagerWindow(new_id));
            break;
        case 4: // Adventure
            WindowManager::add_window(new AdventureGameWindow(new_id));
            break;
        case 5: // Racing
            WindowManager::add_window(new RacingGameWindow(new_id));
            break;
        case 6: // Platformer
            WindowManager::add_window(new PlatformerGameWindow(new_id));
            break;
        case 7: // Shooter
            WindowManager::add_window(new ShooterGameWindow(new_id));
            break;
        case 8: // Puzzle
            WindowManager::add_window(new PuzzleGameWindow(new_id));
            break;
    }
}

void DesktopCompositor::handle_click(int x, int y, int btn) {
    int w = VesaDriver::get_width();
    int h = VesaDriver::get_height();
    int tb_y = h - TASKBAR_HEIGHT;

    // Check Start Menu items if open
    if (start_menu_open) {
        int sm_w = 260;
        int sm_h = 340;
        int sm_x = 6;
        int sm_y = h - TASKBAR_HEIGHT - sm_h - 4;

        if (x >= sm_x && x < sm_x + sm_w && y >= sm_y && y < sm_y + sm_h) {
            // Power buttons
            int pwr_y = sm_y + sm_h - 32;
            if (y >= pwr_y && y < pwr_y + 24) {
                if (x >= sm_x + 10 && x < sm_x + 125) {
                    // Shutdown
                    Power::shutdown();
                    return;
                } else if (x >= sm_x + 135 && x < sm_x + 250) {
                    // Restart
                    Power::reboot();
                    return;
                }
            }

            // Menu items
            int item_y = sm_y + 54;
            int actions[] = { 4, 5, 6, 7, 8, 3, 1, 2 };
            for (int i = 0; i < 8; i++) {
                if (y >= item_y && y < item_y + 24) {
                    start_menu_open = false;
                    launch_app(actions[i]);
                    return;
                }
                item_y += 28;
            }
            return;
        } else {
            // Clicked outside start menu
            start_menu_open = false;
        }
    }

    // Check Taskbar
    if (y >= tb_y) {
        // Start button
        if (x >= 6 && x < 96) {
            Speaker::play_sfx(SFX_CLICK);
            toggle_start_menu();
            return;
        }

        // Language toggle
        int lang_x = w - 150;
        if (x >= lang_x && x < lang_x + 42) {
            Speaker::play_sfx(SFX_CLICK);
            FontRenderer::toggle_language();
            return;
        }

        // Sound toggle
        int sound_x = w - 100;
        if (x >= sound_x && x < sound_x + 36) {
            Speaker::toggle_sound();
            Speaker::play_sfx(SFX_CLICK);
            return;
        }

        // Taskbar running tabs
        int tab_x = 104;
        int tab_w = 140;
        int win_count = WindowManager::get_window_count();
        for (int i = 0; i < win_count; i++) {
            Window* win = WindowManager::get_window_at_index(i);
            if (win && x >= tab_x && x < tab_x + tab_w) {
                if (win->is_minimized) {
                    win->is_minimized = false;
                    WindowManager::bring_to_front(win->id);
                } else if (win->is_active) {
                    win->is_minimized = true;
                } else {
                    WindowManager::bring_to_front(win->id);
                }
                Speaker::play_sfx(SFX_CLICK);
                return;
            }
            tab_x += tab_w + 6;
        }
        return;
    }

    // Check Desktop Icons
    for (int i = 0; i < icon_count; i++) {
        const DesktopIcon& icon = desktop_icons[i];
        if (x >= icon.x && x < icon.x + 52 && y >= icon.y && y < icon.y + 70) {
            launch_app(icon.action);
            return;
        }
    }

    // Pass through to Window Manager
    WindowManager::handle_mouse_down(x, y, btn);
}

void DesktopCompositor::update_and_render() {
    render_desktop();
    render_desktop_icons();
    WindowManager::render_all();
    render_taskbar();
    render_start_menu();
    render_cursor();

    VesaDriver::flip();
}

void DesktopCompositor::show_splash_screen() {
    int w = VesaDriver::get_width();
    int h = VesaDriver::get_height();

    // Dark high-tech boot background
    VesaDriver::fill_gradient_v(0, 0, w, h, 0x000A0E17, 0x00020408);

    // Glowing logo badge
    int cx = w / 2;
    int cy = h / 2 - 40;

    VesaDriver::fill_circle(cx, cy, 54, 0x000F3E6D);
    VesaDriver::fill_circle(cx, cy, 48, 0x00007ACC);
    VesaDriver::draw_circle(cx, cy, 54, 0x0000D2D3);

    // Emblem 'Y'
    FontRenderer::draw_string(cx - 14, cy - 20, "Y", COLOR_WHITE, 5);

    // Title & Subtitle
    FontRenderer::draw_string(cx - 130, cy + 70, "YAZAN GAME OS", COLOR_WHITE, 3);
    FontRenderer::draw_string_utf8(cx - 90, cy + 105, "نظام التشغيل للألعاب", 0x0000D2D3, 2, true);

    // Animated loading bar
    int bar_w = 320;
    int bar_h = 10;
    int bar_x = cx - bar_w / 2;
    int bar_y = cy + 140;

    VesaDriver::fill_round_rect(bar_x, bar_y, bar_w, bar_h, 5, 0x001B2433);
    VesaDriver::draw_round_rect(bar_x, bar_y, bar_w, bar_h, 5, 0x00334E68);

    VesaDriver::flip();

    // Startup sound jingle
    Speaker::play_sfx(SFX_STARTUP);

    // Progress bar fill animation
    for (int p = 0; p <= 100; p += 10) {
        int fill = (bar_w * p) / 100;
        VesaDriver::fill_round_rect(bar_x, bar_y, fill, bar_h, 5, 0x0000D2D3);
        VesaDriver::flip();
        sleep_ms(30);
    }
}
