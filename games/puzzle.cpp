#include "../include/games.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"

PuzzleGameWindow::PuzzleGameWindow(int id)
    : Window(id, "مكعبات الذكاء | Mind Blocks (2048)", 180, 80, 480, 430),
      score(0), best_score(2048), won(false), lost(false) {
    reset_game();
}

void PuzzleGameWindow::reset_game() {
    score = 0;
    won = false;
    lost = false;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            grid[r][c] = 0;
        }
    }

    add_random_tile();
    add_random_tile();
}

void PuzzleGameWindow::add_random_tile() {
    int empty_r[16], empty_c[16];
    int empty_count = 0;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[r][c] == 0) {
                empty_r[empty_count] = r;
                empty_c[empty_count] = c;
                empty_count++;
            }
        }
    }

    if (empty_count == 0) return;

    // Pseudo-random selection based on score and count
    int idx = (score * 7 + empty_count * 13) % empty_count;
    grid[empty_r[idx]][empty_c[idx]] = ((score % 5 == 0) ? 4 : 2);
}

bool PuzzleGameWindow::move_left() {
    bool moved = false;
    for (int r = 0; r < 4; r++) {
        int target = 0;
        for (int c = 1; c < 4; c++) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col > target && grid[r][col - 1] == 0) {
                    grid[r][col - 1] = grid[r][col];
                    grid[r][col] = 0;
                    col--;
                    moved = true;
                }
                if (col > target && grid[r][col - 1] == grid[r][col]) {
                    grid[r][col - 1] *= 2;
                    score += grid[r][col - 1];
                    grid[r][col] = 0;
                    target = col;
                    moved = true;
                    if (grid[r][col - 1] == 2048) won = true;
                }
            }
        }
    }
    return moved;
}

bool PuzzleGameWindow::move_right() {
    bool moved = false;
    for (int r = 0; r < 4; r++) {
        int target = 3;
        for (int c = 2; c >= 0; c--) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col < target && grid[r][col + 1] == 0) {
                    grid[r][col + 1] = grid[r][col];
                    grid[r][col] = 0;
                    col++;
                    moved = true;
                }
                if (col < target && grid[r][col + 1] == grid[r][col]) {
                    grid[r][col + 1] *= 2;
                    score += grid[r][col + 1];
                    grid[r][col] = 0;
                    target = col;
                    moved = true;
                    if (grid[r][col + 1] == 2048) won = true;
                }
            }
        }
    }
    return moved;
}

bool PuzzleGameWindow::move_up() {
    bool moved = false;
    for (int c = 0; c < 4; c++) {
        int target = 0;
        for (int r = 1; r < 4; r++) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row > target && grid[row - 1][c] == 0) {
                    grid[row - 1][c] = grid[row][c];
                    grid[row][c] = 0;
                    row--;
                    moved = true;
                }
                if (row > target && grid[row - 1][c] == grid[row][c]) {
                    grid[row - 1][c] *= 2;
                    score += grid[row - 1][c];
                    grid[row][c] = 0;
                    target = row;
                    moved = true;
                    if (grid[row - 1][c] == 2048) won = true;
                }
            }
        }
    }
    return moved;
}

bool PuzzleGameWindow::move_down() {
    bool moved = false;
    for (int c = 0; c < 4; c++) {
        int target = 3;
        for (int r = 2; r >= 0; r--) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row < target && grid[row + 1][c] == 0) {
                    grid[row + 1][c] = grid[row][c];
                    grid[row][c] = 0;
                    row++;
                    moved = true;
                }
                if (row < target && grid[row + 1][c] == grid[row][c]) {
                    grid[row + 1][c] *= 2;
                    score += grid[row + 1][c];
                    grid[row][c] = 0;
                    target = row;
                    moved = true;
                    if (grid[row + 1][c] == 2048) won = true;
                }
            }
        }
    }
    return moved;
}

bool PuzzleGameWindow::can_move() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[r][c] == 0) return true;
            if (r < 3 && grid[r][c] == grid[r + 1][c]) return true;
            if (c < 3 && grid[r][c] == grid[r][c + 1]) return true;
        }
    }
    return false;
}

static uint32_t get_tile_color(int val) {
    switch (val) {
        case 2:    return 0x00EEE4DA;
        case 4:    return 0x00EDE0C8;
        case 8:    return 0x00F2B179;
        case 16:   return 0x00F59563;
        case 32:   return 0x00F67C5F;
        case 64:   return 0x00F65E3B;
        case 128:  return 0x00EDCF72;
        case 256:  return 0x00EDCC61;
        case 512:  return 0x00EDC850;
        case 1024: return 0x00EDC53F;
        case 2048: return 0x00EDC22E;
        default:   return 0x003C3A32;
    }
}

void PuzzleGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x001B2028);

    // Top Header
    VesaDriver::fill_rect(cx, cy, cw, 34, 0x00141820);
    FontRenderer::draw_bilingual_text(cx + 12, cy + 10, "النقاط:", "Score:", 0x00F1C40F, 1);
    
    char sc_str[16];
    sc_str[0] = '0' + ((score / 1000) % 10);
    sc_str[1] = '0' + ((score / 100) % 10);
    sc_str[2] = '0' + ((score / 10) % 10);
    sc_str[3] = '0' + (score % 10);
    sc_str[4] = '\0';
    FontRenderer::draw_string(cx + 65, cy + 10, sc_str, COLOR_WHITE, 1);

    // New Game button
    VesaDriver::fill_round_rect(cx + cw - 90, cy + 6, 80, 22, 3, 0x00E67E22);
    FontRenderer::draw_string(cx + cw - 78, cy + 10, "Restart", COLOR_WHITE, 1);

    // 4x4 Grid Container
    int grid_size = 280;
    int grid_x = cx + (cw - grid_size) / 2;
    int grid_y = cy + 45;

    VesaDriver::fill_round_rect(grid_x, grid_y, grid_size, grid_size, 8, 0x00282E39);

    int cell_size = 60;
    int cell_gap = 8;
    int start_offset = 8;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int tx = grid_x + start_offset + c * (cell_size + cell_gap);
            int ty = grid_y + start_offset + r * (cell_size + cell_gap);
            int val = grid[r][c];

            if (val == 0) {
                VesaDriver::fill_round_rect(tx, ty, cell_size, cell_size, 6, 0x00323A47);
            } else {
                uint32_t t_col = get_tile_color(val);
                VesaDriver::fill_round_rect(tx, ty, cell_size, cell_size, 6, t_col);

                // Format number string
                char val_str[8];
                int temp = val, len = 0;
                while (temp > 0) { len++; temp /= 10; }
                val_str[len] = '\0';
                temp = val;
                for (int k = len - 1; k >= 0; k--) {
                    val_str[k] = '0' + (temp % 10);
                    temp /= 10;
                }

                uint32_t txt_col = (val <= 4) ? 0x00776E65 : COLOR_WHITE;
                int text_w = len * 8 * 2;
                int text_x = tx + (cell_size - text_w) / 2;
                int text_y = ty + (cell_size - 16) / 2;
                FontRenderer::draw_string(text_x, text_y, val_str, txt_col, 2);
            }
        }
    }

    // On-screen directional buttons for mouse/touch
    int dpad_y = grid_y + grid_size + 10;
    int dpad_cx = cx + cw / 2;

    VesaDriver::fill_round_rect(dpad_cx - 16, dpad_y, 32, 22, 3, 0x003A4758);
    FontRenderer::draw_string(dpad_cx - 4, dpad_y + 4, "^", COLOR_WHITE, 1);

    VesaDriver::fill_round_rect(dpad_cx - 52, dpad_y + 12, 32, 22, 3, 0x003A4758);
    FontRenderer::draw_string(dpad_cx - 42, dpad_y + 16, "<", COLOR_WHITE, 1);

    VesaDriver::fill_round_rect(dpad_cx + 20, dpad_y + 12, 32, 22, 3, 0x003A4758);
    FontRenderer::draw_string(dpad_cx + 30, dpad_y + 16, ">", COLOR_WHITE, 1);

    VesaDriver::fill_round_rect(dpad_cx - 16, dpad_y + 24, 32, 22, 3, 0x003A4758);
    FontRenderer::draw_string(dpad_cx - 4, dpad_y + 28, "v", COLOR_WHITE, 1);

    // Controls description
    FontRenderer::draw_bilingual_text(cx + 12, cy + ch - 16, "الأسهم أو WASD لتحريك المكعبات ودمج الأرقام", "Arrows or WASD to slide & merge tiles", 0x007F8C8D, 1);

    // Win / Lose Overlay
    if (won) {
        VesaDriver::fill_round_rect(grid_x + 10, grid_y + 70, grid_size - 20, 100, 8, 0x001B4D3E);
        FontRenderer::draw_bilingual_text(grid_x + 40, grid_y + 90, "مبروك! وصلت إلى 2048!", "VICTORY! REACHED 2048!", COLOR_WHITE, 1);
        FontRenderer::draw_string(grid_x + 60, grid_y + 120, "Press R to Restart", 0x00F1C40F, 1);
    } else if (lost) {
        VesaDriver::fill_round_rect(grid_x + 10, grid_y + 70, grid_size - 20, 100, 8, 0x004D1B1B);
        FontRenderer::draw_bilingual_text(grid_x + 45, grid_y + 90, "لا توجد حركات متاحة!", "GAME OVER! NO MOVES!", COLOR_WHITE, 1);
        FontRenderer::draw_string(grid_x + 60, grid_y + 120, "Press R to Restart", 0x00F1C40F, 1);
    }
}

void PuzzleGameWindow::on_key(uint8_t scancode, char c) {
    if (c == 'r' || c == 'R') {
        reset_game();
        return;
    }

    if (won || lost) return;

    bool moved = false;
    if (scancode == 0x4B || c == 'a' || c == 'A') {
        moved = move_left();
    } else if (scancode == 0x4D || c == 'd' || c == 'D') {
        moved = move_right();
    } else if (scancode == 0x48 || c == 'w' || c == 'W') {
        moved = move_up();
    } else if (scancode == 0x50 || c == 's' || c == 'S') {
        moved = move_down();
    }

    if (moved) {
        Speaker::play_sfx(SFX_COIN);
        add_random_tile();
        if (!can_move()) {
            lost = true;
            Speaker::play_sfx(SFX_GAMEOVER);
        }
    }
}

void PuzzleGameWindow::on_mouse_click(int mx, int my, int) {
    // Restart button in header
    if (mx >= width - 94 && mx < width - 14 && my >= 6 && my < 28) {
        reset_game();
        Speaker::play_sfx(SFX_CLICK);
        return;
    }

    if (won || lost) {
        reset_game();
        Speaker::play_sfx(SFX_CLICK);
        return;
    }

    // Directional buttons click
    int grid_size = 280;
    int grid_y = 45;
    int dpad_y = grid_y + grid_size + 10;
    int dpad_cx = width / 2;

    bool moved = false;
    if (mx >= dpad_cx - 16 && mx < dpad_cx + 16 && my >= dpad_y && my < dpad_y + 22) {
        moved = move_up();
    } else if (mx >= dpad_cx - 52 && mx < dpad_cx - 20 && my >= dpad_y + 12 && my < dpad_y + 34) {
        moved = move_left();
    } else if (mx >= dpad_cx + 20 && mx < dpad_cx + 52 && my >= dpad_y + 12 && my < dpad_y + 34) {
        moved = move_right();
    } else if (mx >= dpad_cx - 16 && mx < dpad_cx + 16 && my >= dpad_y + 24 && my < dpad_y + 46) {
        moved = move_down();
    }

    if (moved) {
        Speaker::play_sfx(SFX_COIN);
        add_random_tile();
        if (!can_move()) {
            lost = true;
            Speaker::play_sfx(SFX_GAMEOVER);
        }
    }
}
