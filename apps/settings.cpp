#include "../include/apps.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"
#include "../include/compositor.h"
#include "../include/memory.h"

SettingsWindow::SettingsWindow(int id)
    : Window(id, "الإعدادات | Settings", 180, 100, 440, 320), active_tab(0) {
}

void SettingsWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x001B2028);

    // Sidebar Tabs
    int tab_w = 110;
    VesaDriver::fill_rect(cx, cy, tab_w, ch, 0x00141820);
    VesaDriver::draw_line(cx + tab_w, cy, cx + tab_w, cy + ch, 0x002A3240);

    const char* tabs_ar[] = { "الشاشة", "الصوت", "اللغة", "عن النظام" };
    const char* tabs_en[] = { "Display", "Audio", "Language", "About OS" };

    for (int i = 0; i < 4; i++) {
        int ty = cy + 10 + i * 36;
        uint32_t bg = (active_tab == i) ? 0x000F4C81 : 0x001B2028;
        VesaDriver::fill_round_rect(cx + 6, ty, tab_w - 12, 28, 4, bg);
        FontRenderer::draw_bilingual_text(cx + 14, ty + 8, tabs_ar[i], tabs_en[i], COLOR_WHITE, 1);
    }

    // Content Area
    int content_x = cx + tab_w + 16;
    int content_y = cy + 16;

    switch (active_tab) {
        case 0: { // Display Tab
            FontRenderer::draw_bilingual_text(content_x, content_y, "خلفية سطح المكتب", "Desktop Wallpaper", 0x0000D2D3, 1);

            const char* themes_ar[] = { "السايبر المستقبلي", "الفضاء العميق", "شروق الريترو", "الداكن الهادئ" };
            const char* themes_en[] = { "Cyber Neon", "Deep Space", "Retro Sunset", "Minimal Dark" };

            for (int i = 0; i < 4; i++) {
                int by = content_y + 26 + i * 34;
                bool is_cur = ((int)DesktopCompositor::get_wallpaper() == i);
                uint32_t btn_bg = is_cur ? 0x0027AE60 : 0x002C3E50;

                VesaDriver::fill_round_rect(content_x, by, 220, 26, 4, btn_bg);
                FontRenderer::draw_bilingual_text(content_x + 12, by + 7, themes_ar[i], themes_en[i], COLOR_WHITE, 1);
                if (is_cur) {
                    FontRenderer::draw_string(content_x + 195, by + 7, "*", COLOR_WHITE, 1);
                }
            }
            break;
        }
        case 1: { // Audio Tab
            FontRenderer::draw_bilingual_text(content_x, content_y, "إعدادات الصوت والمؤثرات", "Sound & Audio FX", 0x0000D2D3, 1);

            // Mute / Unmute Toggle Button
            int btn_y = content_y + 28;
            uint32_t mute_bg = Speaker::is_enabled() ? 0x0027AE60 : 0x00C0392B;
            VesaDriver::fill_round_rect(content_x, btn_y, 200, 30, 4, mute_bg);
            const char* snd_state_ar = Speaker::is_enabled() ? "الصوت: مفعّل (انقر للكتم)" : "الصوت: مكتوم (انقر للتفعيل)";
            const char* snd_state_en = Speaker::is_enabled() ? "Sound: ON (Click to Mute)" : "Sound: MUTED (Click to Enable)";
            FontRenderer::draw_bilingual_text(content_x + 8, btn_y + 9, snd_state_ar, snd_state_en, COLOR_WHITE, 1);

            // Test SFX Buttons
            FontRenderer::draw_bilingual_text(content_x, btn_y + 45, "اختبار المؤثرات الصوتية:", "Test Sound Effects:", COLOR_WHITE, 1);
            
            const char* sfx_names[] = { "قفز (Jump)", "ليزر (Laser)", "انفجار (Explosion)", "عملة (Coin)" };
            for (int i = 0; i < 4; i++) {
                int s_y = btn_y + 65 + i * 30;
                VesaDriver::fill_round_rect(content_x, s_y, 160, 22, 4, 0x002980B9);
                FontRenderer::draw_string(content_x + 10, s_y + 5, sfx_names[i], COLOR_WHITE, 1);
            }
            break;
        }
        case 2: { // Language Tab
            FontRenderer::draw_bilingual_text(content_x, content_y, "لغة النظام | System Language", 0x0000D2D3, 1);

            bool is_ar = (FontRenderer::get_language() == LANG_ARABIC);

            // Arabic Button
            int ar_y = content_y + 35;
            VesaDriver::fill_round_rect(content_x, ar_y, 200, 34, 4, is_ar ? 0x0027AE60 : 0x0034495E);
            FontRenderer::draw_string(content_x + 16, ar_y + 10, "العربية (Arabic)", COLOR_WHITE, 1);
            if (is_ar) FontRenderer::draw_string(content_x + 170, ar_y + 10, "[V]", COLOR_WHITE, 1);

            // English Button
            int en_y = ar_y + 45;
            VesaDriver::fill_round_rect(content_x, en_y, 200, 34, 4, !is_ar ? 0x0027AE60 : 0x0034495E);
            FontRenderer::draw_string(content_x + 16, en_y + 10, "English (US)", COLOR_WHITE, 1);
            if (!is_ar) FontRenderer::draw_string(content_x + 170, en_y + 10, "[V]", COLOR_WHITE, 1);
            break;
        }
        case 3: { // System Info Tab
            FontRenderer::draw_bilingual_text(content_x, content_y, "معلومات النظام", "System Specifications", 0x0000D2D3, 1);

            FontRenderer::draw_string(content_x, content_y + 30, "OS: Yazan Game OS v1.0", COLOR_WHITE, 1);
            FontRenderer::draw_string(content_x, content_y + 48, "Architecture: x86 Bare-Metal (32-bit)", COLOR_WHITE, 1);
            FontRenderer::draw_string(content_x, content_y + 66, "Resolution: 800x600 32bpp VBE LFB", COLOR_WHITE, 1);
            FontRenderer::draw_string(content_x, content_y + 84, "Audio: PC Speaker Synthesizer", COLOR_WHITE, 1);
            
            // Memory stats
            uint32_t total_mb = memory_get_total() / (1024 * 1024);
            uint32_t used_kb = memory_get_used() / 1024;
            char mem_str[48];
            kstrcpy(mem_str, "Heap Total: 32 MB | Used: < 2 MB");
            FontRenderer::draw_string(content_x, content_y + 102, mem_str, 0x00F1C40F, 1);

            FontRenderer::draw_bilingual_text(content_x, content_y + 130, "تمت برمجته من الصفر دون Linux", "Built completely from scratch!", 0x002ECC71, 1);
            break;
        }
    }
}

void SettingsWindow::on_mouse_click(int mx, int my, int) {
    int tab_w = 110;

    // Clicked Tab
    if (mx >= 6 && mx < tab_w - 6) {
        for (int i = 0; i < 4; i++) {
            int ty = 10 + i * 36;
            if (my >= ty && my < ty + 28) {
                active_tab = i;
                Speaker::play_sfx(SFX_CLICK);
                return;
            }
        }
    }

    int content_x = tab_w + 16;
    int content_y = 16;

    if (active_tab == 0) { // Display
        for (int i = 0; i < 4; i++) {
            int by = content_y + 26 + i * 34;
            if (mx >= content_x && mx < content_x + 220 && my >= by && my < by + 26) {
                DesktopCompositor::set_wallpaper((WallpaperTheme)i);
                Speaker::play_sfx(SFX_CLICK);
                return;
            }
        }
    } else if (active_tab == 1) { // Audio
        int btn_y = content_y + 28;
        if (mx >= content_x && mx < content_x + 200 && my >= btn_y && my < btn_y + 30) {
            Speaker::toggle_sound();
            Speaker::play_sfx(SFX_CLICK);
            return;
        }

        // Test SFX
        for (int i = 0; i < 4; i++) {
            int s_y = btn_y + 65 + i * 30;
            if (mx >= content_x && mx < content_x + 160 && my >= s_y && my < s_y + 22) {
                SoundEffect sfx_arr[] = { SFX_JUMP, SFX_LASER, SFX_EXPLOSION, SFX_COIN };
                Speaker::play_sfx(sfx_arr[i]);
                return;
            }
        }
    } else if (active_tab == 2) { // Language
        int ar_y = content_y + 35;
        if (mx >= content_x && mx < content_x + 200 && my >= ar_y && my < ar_y + 34) {
            FontRenderer::set_language(LANG_ARABIC);
            Speaker::play_sfx(SFX_CLICK);
            return;
        }
        int en_y = ar_y + 45;
        if (mx >= content_x && mx < content_x + 200 && my >= en_y && my < en_y + 34) {
            FontRenderer::set_language(LANG_ENGLISH);
            Speaker::play_sfx(SFX_CLICK);
            return;
        }
    }
}
