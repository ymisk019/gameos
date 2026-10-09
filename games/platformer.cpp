#include "../include/games.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"

PlatformerGameWindow::PlatformerGameWindow(int id)
    : Window(id, "قفزة السايبر | Cyber Jumper", 140, 70, 540, 420),
      px(40), py(280), vx(0), vy(0), on_ground(false), jump_count(0),
      score(0), coins_left(5), lives(3), level_cleared(false), game_over(false) {
    reset_level();
}

void PlatformerGameWindow::reset_level() {
    px = 40;
    py = 280;
    vx = 0;
    vy = 0;
    on_ground = true;
    jump_count = 0;
    score = 0;
    coins_left = 5;
    level_cleared = false;
    game_over = false;

    // Platform layout
    platforms[0] = { 20, 320, 100, 16 };  // Start ground
    platforms[1] = { 150, 270, 80, 16 };  // Platform 1
    platforms[2] = { 260, 220, 70, 16 };  // Platform 2
    platforms[3] = { 360, 180, 80, 16 };  // Platform 3
    platforms[4] = { 230, 130, 80, 16 };  // High Platform
    platforms[5] = { 100, 100, 70, 16 };  // Very high Platform
    platforms[6] = { 420, 100, 100, 16 }; // Goal Platform
    platforms[7] = { 0, 360, 540, 20 };   // Base bottom

    // Coins
    coins[0] = { 180, 240, false };
    coins[1] = { 290, 190, false };
    coins[2] = { 390, 150, false };
    coins[3] = { 260, 100, false };
    coins[4] = { 130, 70,  false };

    // Hazards (Red spikes)
    hazards[0] = { 120, 350, 40 };
    hazards[1] = { 330, 350, 60 };
}

void PlatformerGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x000F172A);

    // Physics Update
    if (!level_cleared && !game_over) {
        vy += 1.2f; // Gravity
        px += vx;
        py += vy;

        // Friction
        vx *= 0.82f;

        // Platform collision checks
        on_ground = false;
        for (int i = 0; i < 8; i++) {
            Platform& p = platforms[i];
            int pl_x = cx + p.x;
            int pl_y = cy + p.y;

            // Landing on top of platform
            if (px + 18 >= pl_x && px <= pl_x + p.w &&
                py + 24 >= pl_y && py + 24 <= pl_y + 12 && vy >= 0) {
                py = pl_y - 24;
                vy = 0;
                on_ground = true;
                jump_count = 0;
            }
        }

        // Screen boundary limits
        if (px < cx + 10) px = cx + 10;
        if (px > cx + cw - 30) px = cx + cw - 30;

        // Check hazard spikes
        for (int i = 0; i < 2; i++) {
            int hz_x = cx + hazards[i].x;
            int hz_y = cy + hazards[i].y;
            if (px + 16 >= hz_x && px <= hz_x + hazards[i].w &&
                py + 24 >= hz_y) {
                lives--;
                Speaker::play_sfx(SFX_EXPLOSION);
                if (lives <= 0) {
                    game_over = true;
                    Speaker::play_sfx(SFX_GAMEOVER);
                } else {
                    px = cx + 40;
                    py = cy + 280;
                    vy = 0;
                }
            }
        }

        // Check coin pickups
        for (int i = 0; i < 5; i++) {
            if (!coins[i].collected) {
                int coin_cx = cx + coins[i].x;
                int coin_cy = cy + coins[i].y;
                if (px + 20 >= coin_cx && px <= coin_cx + 16 &&
                    py + 24 >= coin_cy && py <= coin_cy + 16) {
                    coins[i].collected = true;
                    coins_left--;
                    score += 100;
                    Speaker::play_sfx(SFX_COIN);
                }
            }
        }

        // Check Goal Portal
        int goal_x = cx + platforms[6].x + 40;
        int goal_y = cy + platforms[6].y - 30;
        if (px + 18 >= goal_x && px <= goal_x + 30 &&
            py + 24 >= goal_y && py <= goal_y + 36) {
            level_cleared = true;
            Speaker::play_sfx(SFX_VICTORY);
        }
    }

    // Render Platforms
    for (int i = 0; i < 8; i++) {
        Platform& p = platforms[i];
        VesaDriver::fill_round_rect(cx + p.x, cy + p.y, p.w, p.h, 4, 0x001E293B);
        VesaDriver::fill_rect(cx + p.x, cy + p.y, p.w, 4, 0x0000D2D3); // Neon top border
    }

    // Render Spikes
    for (int i = 0; i < 2; i++) {
        int hz_x = cx + hazards[i].x;
        int hz_y = cy + hazards[i].y;
        for (int s = 0; s < hazards[i].w; s += 10) {
            VesaDriver::draw_line(hz_x + s, hz_y + 10, hz_x + s + 5, hz_y, 0x00E74C3C);
            VesaDriver::draw_line(hz_x + s + 5, hz_y, hz_x + s + 10, hz_y + 10, 0x00E74C3C);
        }
    }

    // Render Coins
    for (int i = 0; i < 5; i++) {
        if (!coins[i].collected) {
            int coin_cx = cx + coins[i].x;
            int coin_cy = cy + coins[i].y;
            VesaDriver::fill_circle(coin_cx + 8, coin_cy + 8, 7, 0x00F1C40F);
            VesaDriver::draw_circle(coin_cx + 8, coin_cy + 8, 7, COLOR_WHITE);
        }
    }

    // Render Goal Portal
    int gx = cx + platforms[6].x + 40;
    int gy = cy + platforms[6].y - 30;
    VesaDriver::fill_round_rect(gx, gy, 26, 32, 8, 0x009B59B6);
    VesaDriver::draw_round_rect(gx, gy, 26, 32, 8, 0x0000D2D3);
    FontRenderer::draw_string(gx + 9, gy + 8, "G", COLOR_WHITE, 1);

    // Render Cyber Jumper Character
    int cur_px = (int)px;
    int cur_py = (int)py;
    VesaDriver::fill_round_rect(cur_px, cur_py, 18, 24, 4, 0x002ECC71); // Cyber Green suit
    VesaDriver::fill_rect(cur_px + 4, cur_py + 4, 10, 6, 0x0000D2D3);    // Visor
    VesaDriver::fill_rect(cur_px + 3, cur_py + 20, 4, 6, 0x001B2028);    // Left leg
    VesaDriver::fill_rect(cur_px + 11, cur_py + 20, 4, 6, 0x001B2028);   // Right leg

    // Top HUD
    VesaDriver::fill_rect(cx, cy, cw, 28, 0x00141820);
    FontRenderer::draw_bilingual_text(cx + 12, cy + 8, "النقاط:", "Score:", 0x00F1C40F, 1);
    char sc_str[16];
    sc_str[0] = '0' + ((score / 100) % 10);
    sc_str[1] = '0' + ((score / 10) % 10);
    sc_str[2] = '0' + (score % 10);
    sc_str[3] = '\0';
    FontRenderer::draw_string(cx + 65, cy + 8, sc_str, COLOR_WHITE, 1);

    FontRenderer::draw_bilingual_text(cx + 130, cy + 8, "القطع المتبقية:", "Coins Left:", 0x0000D2D3, 1);
    char cn_str[8] = { (char)('0' + coins_left), '\0' };
    FontRenderer::draw_string(cx + 250, cy + 8, cn_str, COLOR_WHITE, 1);

    FontRenderer::draw_bilingual_text(cx + 290, cy + 8, "المحاولات:", "Lives:", 0x00E74C3C, 1);
    char lv_str[8] = { (char)('0' + lives), '\0' };
    FontRenderer::draw_string(cx + 375, cy + 8, lv_str, COLOR_WHITE, 1);

    // Controls text
    FontRenderer::draw_bilingual_text(cx + 12, cy + ch - 18, "A/D أو الأسهم للحركة | W أو المسافة للقفز المضاعف!", "A/D to move | W/Space to Double Jump!", 0x007F8C8D, 1);

    // Victory or Game Over overlays
    if (level_cleared) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 140, cy + ch / 2 - 40, 280, 80, 8, 0x001B4D3E);
        VesaDriver::draw_round_rect(cx + cw / 2 - 140, cy + ch / 2 - 40, 280, 80, 8, 0x002ECC71);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 80, cy + ch / 2 - 24, "أحسنت! أكملت المرحلة!", "LEVEL CLEARED!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Replay", 0x00F1C40F, 1);
    } else if (game_over) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 80, 8, 0x004D1B1B);
        VesaDriver::draw_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 80, 8, 0x00E74C3C);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 75, cy + ch / 2 - 24, "انتهت المحاولات!", "GAME OVER!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Retry", 0x00F1C40F, 1);
    }
}

void PlatformerGameWindow::on_key(uint8_t scancode, char c) {
    if (c == 'r' || c == 'R') {
        reset_level();
        return;
    }

    if (level_cleared || game_over) return;

    // Move Left
    if (scancode == 0x4B || c == 'a' || c == 'A') {
        vx = -5.0f;
    }
    // Move Right
    else if (scancode == 0x4D || c == 'd' || c == 'D') {
        vx = 5.0f;
    }
    // Jump / Double Jump
    else if (scancode == 0x48 || c == 'w' || c == 'W' || scancode == 0x39 || c == ' ') {
        if (on_ground) {
            vy = -12.0f;
            on_ground = false;
            jump_count = 1;
            Speaker::play_sfx(SFX_JUMP);
        } else if (jump_count < 2) { // Double Jump!
            vy = -10.0f;
            jump_count = 2;
            Speaker::play_sfx(SFX_JUMP);
        }
    }
}

void PlatformerGameWindow::on_mouse_click(int mx, int my, int) {
    if (level_cleared || game_over) {
        reset_level();
        Speaker::play_sfx(SFX_CLICK);
    }
}
