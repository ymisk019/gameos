#include "../include/games.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"

ShooterGameWindow::ShooterGameWindow(int id)
    : Window(id, "حامي المجرة | Galactic Defender", 160, 60, 500, 430),
      ship_x(230), ship_y(340), hp(100), score(0), game_over(false) {
    reset_game();
}

void ShooterGameWindow::reset_game() {
    ship_x = 230;
    ship_y = 340;
    hp = 100;
    score = 0;
    game_over = false;

    for (int i = 0; i < 20; i++) bullets[i].active = false;
    for (int i = 0; i < 30; i++) particles[i].life = 0;

    spawn_aliens();
}

void ShooterGameWindow::spawn_aliens() {
    int idx = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            aliens[idx].x = 60 + col * 90;
            aliens[idx].y = 40 + row * 45;
            aliens[idx].hp = (row == 0) ? 2 : 1;
            aliens[idx].type = row;
            aliens[idx].active = true;
            idx++;
        }
    }
}

void ShooterGameWindow::add_explosion(int x, int y) {
    Speaker::play_sfx(SFX_EXPLOSION);
    for (int i = 0; i < 6; i++) {
        for (int p = 0; p < 30; p++) {
            if (particles[p].life <= 0) {
                particles[p].x = x;
                particles[p].y = y;
                particles[p].vx = ((p % 7) - 3) * 2;
                particles[p].vy = (((p * 3) % 7) - 3) * 2;
                particles[p].life = 10;
                particles[p].color = (p % 2 == 0) ? 0x00F39C12 : 0x00E74C3C;
                break;
            }
        }
    }
}

void ShooterGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x00050811);

    // Starfield in background
    for (int i = 0; i < 25; i++) {
        int sx = cx + (i * 97) % cw;
        int sy = cy + (i * 61 + (score * 2)) % ch;
        VesaDriver::put_pixel(sx, sy, COLOR_WHITE);
    }

    if (!game_over) {
        // Update bullets
        for (int i = 0; i < 20; i++) {
            if (bullets[i].active) {
                bullets[i].y -= 10;
                if (bullets[i].y < cy + 30) {
                    bullets[i].active = false;
                }

                // Check collision with aliens
                for (int a = 0; a < 12; a++) {
                    if (aliens[a].active) {
                        int ax = cx + aliens[a].x;
                        int ay = cy + aliens[a].y;
                        if (bullets[i].x >= ax && bullets[i].x <= ax + 36 &&
                            bullets[i].y >= ay && bullets[i].y <= ay + 26) {
                            bullets[i].active = false;
                            aliens[a].hp--;
                            if (aliens[a].hp <= 0) {
                                aliens[a].active = false;
                                score += 50;
                                add_explosion(ax + 18, ay + 13);
                            } else {
                                Speaker::play_sfx(SFX_CLICK);
                            }
                            break;
                        }
                    }
                }
            }
        }

        // Update aliens (oscillate horizontally and slowly descend)
        bool any_alive = false;
        for (int a = 0; a < 12; a++) {
            if (aliens[a].active) {
                any_alive = true;
                aliens[a].y += 1;
                aliens[a].x += ((score / 10 + a) % 2 == 0) ? 2 : -2;
                if (aliens[a].x < 10) aliens[a].x = 10;
                if (aliens[a].x > cw - 50) aliens[a].x = cw - 50;

                // Alien hits player ship or reaches bottom
                int px = cx + ship_x;
                int py = cy + ship_y;
                int ax = cx + aliens[a].x;
                int ay = cy + aliens[a].y;
                if (ax + 30 >= px && ax <= px + 30 &&
                    ay + 24 >= py && ay <= py + 30) {
                    aliens[a].active = false;
                    hp -= 25;
                    add_explosion(px + 15, py + 15);
                    if (hp <= 0) {
                        game_over = true;
                        Speaker::play_sfx(SFX_GAMEOVER);
                    }
                } else if (ay > cy + ch - 40) {
                    aliens[a].active = false;
                    hp -= 15;
                    if (hp <= 0) {
                        game_over = true;
                        Speaker::play_sfx(SFX_GAMEOVER);
                    }
                }
            }
        }

        // If wave cleared, respawn with bonus!
        if (!any_alive) {
            score += 200;
            Speaker::play_sfx(SFX_VICTORY);
            spawn_aliens();
        }

        // Update explosion particles
        for (int p = 0; p < 30; p++) {
            if (particles[p].life > 0) {
                particles[p].x += particles[p].vx;
                particles[p].y += particles[p].vy;
                particles[p].life--;
            }
        }
    }

    // Render Bullets
    for (int i = 0; i < 20; i++) {
        if (bullets[i].active) {
            VesaDriver::fill_round_rect(bullets[i].x, bullets[i].y, 4, 12, 2, 0x0000D2D3);
        }
    }

    // Render Aliens
    for (int a = 0; a < 12; a++) {
        if (aliens[a].active) {
            int ax = cx + aliens[a].x;
            int ay = cy + aliens[a].y;
            uint32_t a_col = (aliens[a].type == 0) ? 0x00E74C3C : ((aliens[a].type == 1) ? 0x009B59B6 : 0x002ECC71);
            VesaDriver::fill_round_rect(ax, ay, 32, 22, 5, a_col);
            VesaDriver::fill_circle(ax + 9, ay + 10, 3, COLOR_WHITE);
            VesaDriver::fill_circle(ax + 23, ay + 10, 3, COLOR_WHITE);
            VesaDriver::fill_circle(ax + 9, ay + 10, 1, COLOR_BLACK);
            VesaDriver::fill_circle(ax + 23, ay + 10, 1, COLOR_BLACK);
        }
    }

    // Render Explosion Particles
    for (int p = 0; p < 30; p++) {
        if (particles[p].life > 0) {
            VesaDriver::fill_circle(particles[p].x, particles[p].y, 3, particles[p].color);
        }
    }

    // Render Player Spaceship
    int px = cx + ship_x;
    int py = cy + ship_y;
    // Wing thrusters
    VesaDriver::fill_rect(px - 10, py + 14, 8, 16, 0x0034495E);
    VesaDriver::fill_rect(px + 32, py + 14, 8, 16, 0x0034495E);
    // Ship hull
    VesaDriver::fill_round_rect(px, py, 30, 32, 6, 0x003498DB);
    // Cockpit glass
    VesaDriver::fill_circle(px + 15, py + 12, 6, 0x0000D2D3);
    // Engine plasma exhaust
    VesaDriver::fill_rect(px + 10, py + 32, 10, 6, 0x00F39C12);

    // Top HUD Bar
    VesaDriver::fill_rect(cx, cy, cw, 28, 0x00141820);
    FontRenderer::draw_bilingual_text(cx + 12, cy + 8, "الدرع:", "Shield:", 0x0000D2D3, 1);
    
    // Shield HP bar
    int hp_fill = (80 * hp) / 100;
    if (hp_fill < 0) hp_fill = 0;
    uint32_t hp_col = (hp > 30) ? 0x002ECC71 : 0x00E74C3C;
    VesaDriver::fill_rect(cx + 65, cy + 9, 80, 10, 0x002C3E50);
    VesaDriver::fill_rect(cx + 65, cy + 9, hp_fill, 10, hp_col);

    FontRenderer::draw_bilingual_text(cx + 170, cy + 8, "النقاط:", "Score:", 0x00F1C40F, 1);
    char sc_str[16];
    sc_str[0] = '0' + ((score / 1000) % 10);
    sc_str[1] = '0' + ((score / 100) % 10);
    sc_str[2] = '0' + ((score / 10) % 10);
    sc_str[3] = '0' + (score % 10);
    sc_str[4] = '\0';
    FontRenderer::draw_string(cx + 225, cy + 8, sc_str, COLOR_WHITE, 1);

    // Controls text
    FontRenderer::draw_bilingual_text(cx + 12, cy + ch - 18, "الأسهم/WASD للحركة | المسافة/Z لإطلاق الليزر", "Arrows/WASD to fly | Space/Z to Shoot", 0x007F8C8D, 1);

    // Game Over Overlay
    if (game_over) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 80, 8, 0x004D1B1B);
        VesaDriver::draw_round_rect(cx + cw / 2 - 130, cy + ch / 2 - 40, 260, 80, 8, 0x00E74C3C);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 80, cy + ch / 2 - 24, "سقطت السفينة!", "FLEET DESTROYED!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Retry", 0x00F1C40F, 1);
    }
}

void ShooterGameWindow::on_key(uint8_t scancode, char c) {
    if (c == 'r' || c == 'R') {
        reset_game();
        return;
    }

    if (game_over) return;

    if (scancode == 0x4B || c == 'a' || c == 'A') { // Left
        ship_x -= 14;
        if (ship_x < 20) ship_x = 20;
    } else if (scancode == 0x4D || c == 'd' || c == 'D') { // Right
        ship_x += 14;
        if (ship_x > width - 50) ship_x = width - 50;
    } else if (scancode == 0x48 || c == 'w' || c == 'W') { // Up
        ship_y -= 12;
        if (ship_y < 100) ship_y = 100;
    } else if (scancode == 0x50 || c == 's' || c == 'S') { // Down
        ship_y += 12;
        if (ship_y > height - 60) ship_y = height - 60;
    } else if (scancode == 0x39 || c == ' ' || c == 'z' || c == 'Z') { // Shoot!
        for (int i = 0; i < 20; i++) {
            if (!bullets[i].active) {
                bullets[i].x = x + 2 + ship_x + 13;
                bullets[i].y = y + TITLE_BAR_HEIGHT + ship_y;
                bullets[i].active = true;
                Speaker::play_sfx(SFX_LASER);
                break;
            }
        }
    }
}

void ShooterGameWindow::on_mouse_click(int mx, int my, int) {
    if (game_over) {
        reset_game();
        Speaker::play_sfx(SFX_CLICK);
        return;
    }

    // Click in window shoots
    for (int i = 0; i < 20; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x + 2 + ship_x + 13;
            bullets[i].y = y + TITLE_BAR_HEIGHT + ship_y;
            bullets[i].active = true;
            Speaker::play_sfx(SFX_LASER);
            break;
        }
    }
}
