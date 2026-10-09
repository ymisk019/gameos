#include "../include/apps.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"
#include "../include/games.h"
#include "../include/store.h"

static int gm_win_counter = 500;

GameManagerWindow::GameManagerWindow(int id)
    : Window(id, "مدير الألعاب ومتجر التطبيقات | Game Manager & Store", 100, 60, 580, 420),
      selected_game(0), active_tab(0) {
    kstrcpy(status_msg, "جاهز للتشغيل | Ready to Launch");
}

void GameManagerWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x001B2028);

    // Top Tab Bar
    VesaDriver::fill_rect(cx, cy, cw, 34, 0x00141820);
    VesaDriver::draw_line(cx, cy + 34, cx + cw, cy + 34, 0x002A3240);

    // Tab 1: Library
    uint32_t t1_bg = (active_tab == 0) ? 0x000F4C81 : 0x001E2430;
    VesaDriver::fill_round_rect(cx + 8, cy + 5, 150, 24, 4, t1_bg);
    FontRenderer::draw_bilingual_text(cx + 18, cy + 9, "مكتبة الألعاب (5)", "My Games (5)", COLOR_WHITE, 1);

    // Tab 2: Store / Packages
    uint32_t t2_bg = (active_tab == 1) ? 0x000F4C81 : 0x001E2430;
    VesaDriver::fill_round_rect(cx + 166, cy + 5, 170, 24, 4, t2_bg);
    FontRenderer::draw_bilingual_text(cx + 176, cy + 9, "المتجر وإضافة ألعاب", "Store & Add Games", COLOR_WHITE, 1);

    if (active_tab == 0) { // Library
        // Left Column: Game List
        int list_w = 200;
        VesaDriver::fill_rect(cx, cy + 35, list_w, ch - 35, 0x00171B22);
        VesaDriver::draw_line(cx + list_w, cy + 35, cx + list_w, cy + ch, 0x002A3240);

        struct GameEntry {
            const char* title_ar;
            const char* title_en;
            uint32_t color;
        };

        static const GameEntry lib_games[] = {
            { "مغامرات يزن",       "Yazan Quest",       0x00E67E22 },
            { "سباق الصحراء",      "Desert Speed",      0x00E74C3C },
            { "قفزة السايبر",      "Cyber Jumper",      0x002ECC71 },
            { "حامي المجرة",       "Galactic Defender", 0x0000D2D3 },
            { "مكعبات الذكاء",     "Mind Blocks",       0x00F39C12 }
        };

        for (int i = 0; i < 5; i++) {
            int gy = cy + 42 + i * 46;
            bool is_sel = (selected_game == i);
            uint32_t bg = is_sel ? 0x002A384A : 0x001C212A;

            VesaDriver::fill_round_rect(cx + 8, gy, list_w - 16, 40, 5, bg);
            if (is_sel) {
                VesaDriver::draw_round_rect(cx + 8, gy, list_w - 16, 40, 5, 0x0000A8FF);
            }

            VesaDriver::fill_round_rect(cx + 14, gy + 8, 24, 24, 4, lib_games[i].color);
            FontRenderer::draw_bilingual_text(cx + 46, gy + 14, lib_games[i].title_ar, lib_games[i].title_en, COLOR_WHITE, 1);
        }

        // Right Column: Details & Preview
        int det_x = cx + list_w + 16;
        int det_y = cy + 45;

        // Preview Box
        VesaDriver::fill_round_rect(det_x, det_y, 330, 140, 8, 0x000E1117);
        VesaDriver::draw_round_rect(det_x, det_y, 330, 140, 8, 0x002A3546);

        // Preview decoration based on selected game
        if (selected_game == 0) {
            // Adventure preview: Dungeon room with sword
            VesaDriver::fill_rect(det_x + 20, det_y + 20, 290, 100, 0x002A1E14);
            FontRenderer::draw_string(det_x + 90, det_y + 60, "[HERO] vs [SKELETON]", 0x00E67E22, 1);
        } else if (selected_game == 1) {
            // Racing preview: Road with stripes
            VesaDriver::fill_rect(det_x + 20, det_y + 20, 290, 100, 0x00222222);
            VesaDriver::draw_line(det_x + 20, det_y + 70, det_x + 310, det_y + 70, 0x00F1C40F);
            FontRenderer::draw_string(det_x + 110, det_y + 50, "HIGHWAY 200 KM/H", 0x00E74C3C, 1);
        } else if (selected_game == 2) {
            // Platformer preview
            VesaDriver::fill_rect(det_x + 20, det_y + 20, 290, 100, 0x000A192F);
            VesaDriver::fill_rect(det_x + 40, det_y + 90, 120, 16, 0x002ECC71);
            FontRenderer::draw_string(det_x + 80, det_y + 50, "GRAVITY & JUMPING", 0x002ECC71, 1);
        } else if (selected_game == 3) {
            // Shooter preview
            VesaDriver::fill_rect(det_x + 20, det_y + 20, 290, 100, 0x00050811);
            VesaDriver::fill_circle(det_x + 160, det_y + 60, 20, 0x00E74C3C);
            FontRenderer::draw_string(det_x + 90, det_y + 90, "PLASMA CANNONS ON", 0x0000D2D3, 1);
        } else if (selected_game == 4) {
            // Puzzle preview
            VesaDriver::fill_rect(det_x + 20, det_y + 20, 290, 100, 0x001B1E2B);
            VesaDriver::fill_round_rect(det_x + 130, det_y + 45, 50, 40, 4, 0x00F39C12);
            FontRenderer::draw_string(det_x + 140, det_y + 58, "2048", COLOR_WHITE, 1);
        }

        // Game Details Text
        const char* desc_ar[] = {
            "مغامرة زنزانة وبطل: اجمع المفاتيح وحارب الوحوش!",
            "سباق سيارات سريع: تجاوز السيارات واجمع الوقود!",
            "منصات وقفز بالفيزياء: اجمع القطع وتفادَ الأشواك!",
            "إطلاق نار فضائي: دمّر أسراب الغزاة بسفينتك الحربية!",
            "لغز المكعبات والأرقام: ادمج الأرقام ووحّد البلوكات!"
        };
        const char* desc_en[] = {
            "Dungeon RPG: Collect keys, open chests and defeat monsters!",
            "High-speed racer: Dodge traffic cars and collect nitro fuel!",
            "Physics platformer: Double jump, collect coins and reach portal!",
            "Arcade space shooter: Shoot alien fleets with plasma lasers!",
            "Tile sliding puzzle: Merge identical blocks to reach 2048!"
        };

        FontRenderer::draw_bilingual_text(det_x, det_y + 155, desc_ar[selected_game], desc_en[selected_game], COLOR_WHITE, 1);
        FontRenderer::draw_bilingual_text(det_x, det_y + 175, "التحكم: الأسهم أو WASD + المسافة / Z", "Controls: Arrows or WASD + Space / Z", 0x0095A5A6, 1);

        // Big Launch Game Button
        int btn_y = det_y + 205;
        VesaDriver::fill_round_rect(det_x, btn_y, 200, 36, 6, 0x0027AE60);
        VesaDriver::draw_round_rect(det_x, btn_y, 200, 36, 6, 0x002ECC71);
        FontRenderer::draw_bilingual_text(det_x + 36, btn_y + 11, "تشغيل اللعبة الآن", "Launch Game Now", COLOR_WHITE, 1);

    } else { // Store Tab (Dynamic Game Packages)
        int sx = cx + 16;
        int sy = cy + 48;

        FontRenderer::draw_bilingual_text(sx, sy, "متجر Yazan OS وإضافة ألعاب جديدة (.YGP)", "Yazan OS Store - Dynamic Game Packages (.YGP)", 0x0000D2D3, 1);
        FontRenderer::draw_bilingual_text(sx, sy + 20, "يمكنك إضافة أي لعبة بصيغة .ygp دون إعادة بناء الـ ISO!", "Add new games dynamically without rebuilding ISO!", 0x0095A5A6, 1);

        // Action Buttons
        int act_y = sy + 44;
        VesaDriver::fill_round_rect(sx, act_y, 220, 32, 5, 0x002980B9);
        FontRenderer::draw_bilingual_text(sx + 14, act_y + 9, "فحص مجلد /store للتحميل", "Scan /store for Packages", COLOR_WHITE, 1);

        // Package Catalog List
        int cat_y = act_y + 44;
        VesaDriver::fill_round_rect(sx, cat_y, cw - 32, 170, 6, 0x00141820);
        VesaDriver::draw_round_rect(sx, cat_y, cw - 32, 170, 6, 0x002A3240);

        int pkg_cnt = GameStore::get_package_count();
        if (pkg_cnt == 0) {
            FontRenderer::draw_bilingual_text(sx + 20, cat_y + 20, "لا توجد حزم إضافية، انقر فوق الفحص أعلاه", "No external packages yet. Click Scan above.", 0x007F8C8D, 1);
        } else {
            for (int i = 0; i < pkg_cnt; i++) {
                GamePackage* pkg = GameStore::get_package(i);
                if (!pkg) continue;
                int py = cat_y + 12 + i * 40;
                VesaDriver::fill_round_rect(sx + 10, py, cw - 52, 34, 4, 0x00202734);
                FontRenderer::draw_string(sx + 20, py + 10, pkg->header.title_en, COLOR_WHITE, 1);
                FontRenderer::draw_string(sx + 200, py + 10, pkg->header.version, 0x00F1C40F, 1);

                // Run package button
                int run_btn_x = sx + cw - 140;
                VesaDriver::fill_round_rect(run_btn_x, py + 5, 75, 24, 3, 0x0027AE60);
                FontRenderer::draw_string(run_btn_x + 14, py + 8, "Play", COLOR_WHITE, 1);
            }
        }

        // Status bar
        FontRenderer::draw_string(sx, cy + ch - 22, status_msg, 0x002ECC71, 1);
    }
}

void GameManagerWindow::on_mouse_click(int mx, int my, int) {
    // Top Tabs
    if (my >= 5 && my < 29) {
        if (mx >= 8 && mx < 158) {
            active_tab = 0;
            Speaker::play_sfx(SFX_CLICK);
            return;
        } else if (mx >= 166 && mx < 336) {
            active_tab = 1;
            Speaker::play_sfx(SFX_CLICK);
            return;
        }
    }

    if (active_tab == 0) {
        // Game list selection
        if (mx >= 8 && mx < 192) {
            for (int i = 0; i < 5; i++) {
                int gy = 42 + i * 46;
                if (my >= gy && my < gy + 40) {
                    selected_game = i;
                    Speaker::play_sfx(SFX_CLICK);
                    return;
                }
            }
        }

        // Launch Game button
        int det_x = 216;
        int btn_y = 45 + 205;
        if (mx >= det_x && mx < det_x + 200 && my >= btn_y && my < btn_y + 36) {
            Speaker::play_sfx(SFX_COIN);
            int wid = gm_win_counter++;
            switch (selected_game) {
                case 0: WindowManager::add_window(new AdventureGameWindow(wid)); break;
                case 1: WindowManager::add_window(new RacingGameWindow(wid)); break;
                case 2: WindowManager::add_window(new PlatformerGameWindow(wid)); break;
                case 3: WindowManager::add_window(new ShooterGameWindow(wid)); break;
                case 4: WindowManager::add_window(new PuzzleGameWindow(wid)); break;
            }
            return;
        }
    } else { // Store Tab
        int sx = 16;
        int sy = 48;
        int act_y = sy + 44;

        // Scan button
        if (mx >= sx && mx < sx + 220 && my >= act_y && my < act_y + 32) {
            Speaker::play_sfx(SFX_CLICK);
            GameStore::scan_vfs_for_packages();
            kstrcpy(status_msg, "تم فحص المتجر وتحميل الحزم بنجاح! | Packages scanned!");
            return;
        }

        // Run package button
        int cat_y = act_y + 44;
        int pkg_cnt = GameStore::get_package_count();
        for (int i = 0; i < pkg_cnt; i++) {
            int py = cat_y + 12 + i * 40;
            int run_btn_x = sx + (width - 4) - 140;
            if (mx >= run_btn_x && mx < run_btn_x + 75 && my >= py + 5 && my < py + 29) {
                Speaker::play_sfx(SFX_COIN);
                GameStore::launch_package(i);
                return;
            }
        }
    }
}
