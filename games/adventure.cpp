#include "../include/games.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"
#include "../include/keyboard.h"

AdventureGameWindow::AdventureGameWindow(int id)
    : Window(id, "مغامرات يزن | Yazan's Quest", 80, 50, 560, 430),
      player_x(2), player_y(2), player_hp(3), player_keys(0), player_gems(0),
      level(1), game_won(false), game_over(false) {
    init_level();
}

void AdventureGameWindow::init_level() {
    game_won = false;
    game_over = false;
    player_x = 2;
    player_y = 2;
    player_hp = 3;
    player_keys = 0;

    // Tile Legend: 0=Floor, 1=Wall, 2=Door, 3=Chest, 4=Key, 5=Gem, 6=Heart, 7=Exit Portal
    static const uint8_t level_layout[12][16] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,1,0,0,0,0,1,5,0,0,0,3,1},
        {1,0,0,0,1,0,5,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,2,0,0,0,0,1,1,1,2,1,1,1},
        {1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,1},
        {1,4,0,0,0,0,1,0,0,0,0,0,0,0,5,1},
        {1,0,0,5,0,0,1,0,1,1,1,1,0,0,0,1},
        {1,1,2,1,1,0,1,0,1,4,0,1,0,0,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,1,0,0,0,1},
        {1,0,6,0,1,0,0,0,2,0,0,2,0,0,7,1},
        {1,0,0,0,1,5,0,0,1,0,0,1,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int r = 0; r < 12; r++) {
        for (int c = 0; c < 16; c++) {
            map[r][c] = level_layout[r][c];
        }
    }

    // Initialize patrolling skeletons
    enemies[0] = { 6, 2, 1, true };
    enemies[1] = { 11, 4, -1, true };
    enemies[2] = { 7, 8, 1, true };
    enemies[3] = { 13, 8, -1, true };
}

void AdventureGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x0014161C);

    // Top HUD
    VesaDriver::fill_rect(cx, cy, cw, 32, 0x001F2430);
    VesaDriver::draw_line(cx, cy + 32, cx + cw, cy + 32, 0x00333E50);

    // HP Hearts
    FontRenderer::draw_bilingual_text(cx + 8, cy + 10, "الصحة:", "HP:", 0x00E74C3C, 1);
    for (int h = 0; h < 3; h++) {
        uint32_t heart_col = (h < player_hp) ? 0x00E74C3C : 0x004A2024;
        VesaDriver::fill_round_rect(cx + 42 + h * 16, cy + 8, 12, 12, 3, heart_col);
    }

    // Keys
    FontRenderer::draw_bilingual_text(cx + 120, cy + 10, "المفاتيح:", "Keys:", 0x00F1C40F, 1);
    char key_str[8] = { (char)('0' + player_keys), '\0' };
    FontRenderer::draw_string(cx + 175, cy + 10, key_str, COLOR_WHITE, 1);

    // Gems / Score
    FontRenderer::draw_bilingual_text(cx + 210, cy + 10, "الجواهر:", "Gems:", 0x0000D2D3, 1);
    char gem_str[8] = { (char)('0' + player_gems), '\0' };
    FontRenderer::draw_string(cx + 265, cy + 10, gem_str, COLOR_WHITE, 1);

    // Restart button in HUD
    VesaDriver::fill_round_rect(cx + cw - 80, cy + 5, 72, 22, 3, 0x002980B9);
    FontRenderer::draw_string(cx + cw - 70, cy + 9, "Reset", COLOR_WHITE, 1);

    // Grid rendering (12 rows x 16 cols, each tile 32x28 px)
    int tile_w = 33;
    int tile_h = 28;
    int map_start_x = cx + (cw - 16 * tile_w) / 2;
    int map_start_y = cy + 40;

    for (int r = 0; r < 12; r++) {
        for (int c = 0; c < 16; c++) {
            int tx = map_start_x + c * tile_w;
            int ty = map_start_y + r * tile_h;
            uint8_t t = map[r][c];

            switch (t) {
                case 1: // Stone Wall
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x003A4354);
                    VesaDriver::draw_rect(tx, ty, tile_w - 1, tile_h - 1, 0x002C3542);
                    break;
                case 0: // Floor
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x001B202A);
                    break;
                case 2: // Locked Door
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x00784212);
                    VesaDriver::fill_circle(tx + tile_w / 2, ty + tile_h / 2, 4, 0x00F1C40F);
                    break;
                case 3: // Chest
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x001B202A);
                    VesaDriver::fill_round_rect(tx + 6, ty + 6, tile_w - 13, tile_h - 13, 3, 0x00E67E22);
                    break;
                case 4: // Key
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x001B202A);
                    VesaDriver::fill_circle(tx + tile_w / 2, ty + tile_h / 2, 5, 0x00F1C40F);
                    break;
                case 5: // Gem
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x001B202A);
                    VesaDriver::fill_circle(tx + tile_w / 2, ty + tile_h / 2, 4, 0x0000D2D3);
                    break;
                case 6: // Potion
                    VesaDriver::fill_rect(tx, ty, tile_w - 1, tile_h - 1, 0x001B202A);
                    VesaDriver::fill_circle(tx + tile_w / 2, ty + tile_h / 2, 5, 0x00E74C3C);
                    break;
                case 7: // Exit Portal
                    VesaDriver::fill_round_rect(tx + 4, ty + 4, tile_w - 9, tile_h - 9, 6, 0x009B59B6);
                    VesaDriver::draw_round_rect(tx + 4, ty + 4, tile_w - 9, tile_h - 9, 6, COLOR_WHITE);
                    break;
            }
        }
    }

    // Render Enemies (Skeletons)
    for (int i = 0; i < 4; i++) {
        if (!enemies[i].alive) continue;
        int ex = map_start_x + enemies[i].x * tile_w;
        int ey = map_start_y + enemies[i].y * tile_h;
        VesaDriver::fill_round_rect(ex + 6, ey + 4, tile_w - 13, tile_h - 9, 4, 0x00E0E0E0);
        VesaDriver::fill_circle(ex + 12, ey + 10, 2, 0x00C0392B); // red eyes
        VesaDriver::fill_circle(ex + 20, ey + 10, 2, 0x00C0392B);
    }

    // Render Hero Player (Yazan the Hero)
    int px = map_start_x + player_x * tile_w;
    int py = map_start_y + player_y * tile_h;
    VesaDriver::fill_round_rect(px + 4, py + 3, tile_w - 9, tile_h - 7, 5, 0x003498DB); // Knight blue armor
    VesaDriver::fill_circle(px + tile_w / 2, py + 8, 4, 0x00F5CD79); // Head
    VesaDriver::fill_rect(px + tile_w - 7, py + 8, 3, 12, 0x00BDC3C7); // Sword

    // Bottom Status / Controls Hint
    int hint_y = cy + ch - 22;
    FontRenderer::draw_bilingual_text(cx + 12, hint_y, "الأسهم/WASD للحركة | المسافة للضرب بالسيف", "Arrows/WASD to move | Space to Attack", 0x007F8C8D, 1);

    // Victory or Game Over overlays
    if (game_won) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 70, 8, 0x001B4D3E);
        VesaDriver::draw_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 70, 8, 0x002ECC71);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 80, cy + ch / 2 - 24, "مبروك! انتصرت!", "VICTORY! YOU ESCAPED!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Replay", 0x00F1C40F, 1);
    } else if (game_over) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 70, 8, 0x004D1B1B);
        VesaDriver::draw_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 70, 8, 0x00E74C3C);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 75, cy + ch / 2 - 24, "انتهت اللعبة! خسرت!", "GAME OVER!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Retry", 0x00F1C40F, 1);
    }
}

void AdventureGameWindow::on_key(uint8_t scancode, char c) {
    if (c == 'r' || c == 'R') {
        init_level();
        return;
    }

    if (game_won || game_over) return;

    int dx = 0, dy = 0;
    if (scancode == 0x48 || c == 'w' || c == 'W') dy = -1; // Up
    else if (scancode == 0x50 || c == 's' || c == 'S') dy = 1;  // Down
    else if (scancode == 0x4B || c == 'a' || c == 'A') dx = -1; // Left
    else if (scancode == 0x4D || c == 'd' || c == 'D') dx = 1;  // Right
    else if (scancode == 0x39 || c == ' ' || c == 'z' || c == 'Z') {
        // Attack nearby enemies with sword!
        Speaker::play_sfx(SFX_LASER);
        for (int i = 0; i < 4; i++) {
            if (enemies[i].alive) {
                int dist = (enemies[i].x - player_x) * (enemies[i].x - player_x) +
                           (enemies[i].y - player_y) * (enemies[i].y - player_y);
                if (dist <= 2) {
                    enemies[i].alive = false;
                    player_gems += 2;
                    Speaker::play_sfx(SFX_COIN);
                }
            }
        }
        return;
    }

    if (dx == 0 && dy == 0) return;

    int nx = player_x + dx;
    int ny = player_y + dy;

    if (nx < 0 || nx >= 16 || ny < 0 || ny >= 12) return;

    uint8_t target = map[ny][nx];

    // Check Wall
    if (target == 1) {
        Speaker::play_sfx(SFX_CLICK);
        return;
    }

    // Check Locked Door
    if (target == 2) {
        if (player_keys > 0) {
            player_keys--;
            map[ny][nx] = 0;
            Speaker::play_sfx(SFX_COIN);
        } else {
            Speaker::play_sfx(SFX_CLICK);
            return;
        }
    }

    // Check Chest
    if (target == 3) {
        player_gems += 5;
        map[ny][nx] = 0;
        Speaker::play_sfx(SFX_COIN);
    }

    // Check Key
    if (target == 4) {
        player_keys++;
        map[ny][nx] = 0;
        Speaker::play_sfx(SFX_COIN);
    }

    // Check Gem
    if (target == 5) {
        player_gems++;
        map[ny][nx] = 0;
        Speaker::play_sfx(SFX_COIN);
    }

    // Check Heart Potion
    if (target == 6) {
        if (player_hp < 3) player_hp++;
        map[ny][nx] = 0;
        Speaker::play_sfx(SFX_COIN);
    }

    // Check Exit Portal
    if (target == 7) {
        game_won = true;
        Speaker::play_sfx(SFX_VICTORY);
        return;
    }

    // Move player
    player_x = nx;
    player_y = ny;

    // Move Patrol Enemies
    for (int i = 0; i < 4; i++) {
        if (!enemies[i].alive) continue;
        int next_ex = enemies[i].x + enemies[i].dir;
        if (next_ex < 1 || next_ex >= 15 || map[enemies[i].y][next_ex] == 1) {
            enemies[i].dir = -enemies[i].dir;
        } else {
            enemies[i].x = next_ex;
        }

        // Enemy collision with player
        if (enemies[i].x == player_x && enemies[i].y == player_y) {
            player_hp--;
            Speaker::play_sfx(SFX_EXPLOSION);
            if (player_hp <= 0) {
                game_over = true;
                Speaker::play_sfx(SFX_GAMEOVER);
            }
        }
    }
}

void AdventureGameWindow::on_mouse_click(int mx, int my, int) {
    // Reset button in HUD
    if (mx >= width - 84 && mx < width - 12 && my >= 5 && my < 27) {
        init_level();
        Speaker::play_sfx(SFX_CLICK);
    }
}
