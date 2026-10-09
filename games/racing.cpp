#include "../include/games.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"

RacingGameWindow::RacingGameWindow(int id)
    : Window(id, "سباق الصحراء | Desert Speed", 120, 60, 520, 420),
      car_x(260), speed(6), distance(0), score(0), fuel(100),
      crashed(false), fuel_x(260), fuel_y(80), road_offset(0) {
    reset_game();
}

void RacingGameWindow::reset_game() {
    car_x = 260;
    speed = 6;
    distance = 0;
    score = 0;
    fuel = 100;
    crashed = false;
    road_offset = 0;

    uint32_t colors[] = { 0x003498DB, 0x002ECC71, 0x009B59B6, 0x00F1C40F, 0x00E67E22 };
    for (int i = 0; i < 5; i++) {
        traffic[i].x = 180 + (i % 3) * 60;
        traffic[i].y = 50 + i * 80;
        traffic[i].speed = 2 + (i % 3);
        traffic[i].color = colors[i];
        traffic[i].active = true;
    }
    fuel_x = 220;
    fuel_y = -60;
}

void RacingGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x001B2028);

    // Desert background on sides
    VesaDriver::fill_rect(cx, cy, 120, ch, 0x00C29548);
    VesaDriver::fill_rect(cx + cw - 120, cy, 120, ch, 0x00C29548);

    // Highway Asphalt
    int road_x = cx + 120;
    int road_w = cw - 240;
    VesaDriver::fill_rect(road_x, cy, road_w, ch, 0x00242B35);

    // Road borders (Curbs red & white)
    for (int y = cy; y < cy + ch; y += 20) {
        bool red = ((y + road_offset) / 20) % 2 == 0;
        uint32_t curb_col = red ? 0x00E74C3C : COLOR_WHITE;
        VesaDriver::fill_rect(road_x - 8, y, 8, 20, curb_col);
        VesaDriver::fill_rect(road_x + road_w, y, 8, 20, curb_col);
    }

    // Moving lane divider dashed lines
    for (int lane = 1; lane <= 2; lane++) {
        int lx = road_x + (road_w * lane) / 3;
        for (int y = cy; y < cy + ch; y += 32) {
            int ny = y + (road_offset % 32);
            if (ny < cy + ch) {
                VesaDriver::fill_rect(lx - 2, ny, 4, 16, 0x00F1C40F);
            }
        }
    }

    // Update game motion if not crashed
    if (!crashed) {
        road_offset = (road_offset + speed) % 1000;
        distance += speed / 2;
        score += speed;
        fuel -= 1; // fuel consumption
        if (fuel <= 0) {
            crashed = true;
            Speaker::play_sfx(SFX_GAMEOVER);
        }

        // Move traffic cars downwards relative to player speed
        for (int i = 0; i < 5; i++) {
            traffic[i].y += (speed - traffic[i].speed);
            if (traffic[i].y > cy + ch + 40) {
                traffic[i].y = cy - 60;
                traffic[i].x = road_x + 20 + (i * 53) % (road_w - 50);
            }

            // Collision check with player car
            int px = cx + car_x;
            int py = cy + ch - 80;
            if (traffic[i].y + 40 >= py && traffic[i].y <= py + 48 &&
                traffic[i].x + 28 >= px && traffic[i].x <= px + 28) {
                crashed = true;
                Speaker::play_sfx(SFX_CRASH);
            }
        }

        // Move fuel canister
        fuel_y += speed;
        if (fuel_y > cy + ch + 30) {
            fuel_y = cy - 80;
            fuel_x = road_x + 30 + (distance % (road_w - 60));
        }

        // Fuel collection check
        int px = cx + car_x;
        int py = cy + ch - 80;
        if (fuel_y + 16 >= py && fuel_y <= py + 48 &&
            fuel_x + 16 >= px && fuel_x <= px + 28) {
            fuel = 100;
            fuel_y = cy - 200;
            score += 250;
            Speaker::play_sfx(SFX_COIN);
        }
    }

    // Render Traffic Cars
    for (int i = 0; i < 5; i++) {
        if (traffic[i].y >= cy - 50 && traffic[i].y <= cy + ch) {
            VesaDriver::fill_round_rect(traffic[i].x, traffic[i].y, 28, 46, 5, traffic[i].color);
            // Windshield
            VesaDriver::fill_rect(traffic[i].x + 4, traffic[i].y + 12, 20, 8, 0x001B2028);
            // Wheels
            VesaDriver::fill_rect(traffic[i].x - 3, traffic[i].y + 6, 3, 10, COLOR_BLACK);
            VesaDriver::fill_rect(traffic[i].x + 28, traffic[i].y + 6, 3, 10, COLOR_BLACK);
            VesaDriver::fill_rect(traffic[i].x - 3, traffic[i].y + 30, 3, 10, COLOR_BLACK);
            VesaDriver::fill_rect(traffic[i].x + 28, traffic[i].y + 30, 3, 10, COLOR_BLACK);
        }
    }

    // Render Fuel Canister
    if (fuel_y >= cy && fuel_y <= cy + ch) {
        VesaDriver::fill_round_rect(fuel_x, fuel_y, 18, 20, 3, 0x00E67E22);
        FontRenderer::draw_string(fuel_x + 5, fuel_y + 4, "F", COLOR_WHITE, 1);
    }

    // Render Player Supercar (Red Sports Car)
    int px = cx + car_x;
    int py = cy + ch - 80;
    VesaDriver::fill_round_rect(px, py, 30, 50, 6, 0x00E74C3C); // Ferrari Red
    // Racing stripes
    VesaDriver::fill_rect(px + 12, py, 6, 50, COLOR_WHITE);
    // Windshield & roof
    VesaDriver::fill_round_rect(px + 5, py + 14, 20, 16, 3, 0x001B2028);
    // Wheels
    VesaDriver::fill_rect(px - 3, py + 6, 3, 12, COLOR_BLACK);
    VesaDriver::fill_rect(px + 30, py + 6, 3, 12, COLOR_BLACK);
    VesaDriver::fill_rect(px - 3, py + 32, 3, 12, COLOR_BLACK);
    VesaDriver::fill_rect(px + 30, py + 32, 3, 12, COLOR_BLACK);
    // Headlights
    VesaDriver::fill_rect(px + 3, py + 2, 6, 3, 0x00F1C40F);
    VesaDriver::fill_rect(px + 21, py + 2, 6, 3, 0x00F1C40F);

    // Left HUD panel (Speedometer & Distance)
    VesaDriver::fill_round_rect(cx + 8, cy + 12, 104, 110, 6, 0x00141820);
    FontRenderer::draw_bilingual_text(cx + 14, cy + 18, "السرعة", "SPEED", 0x0000D2D3, 1);
    char spd_str[16];
    int spd_kmh = speed * 25;
    spd_str[0] = '0' + (spd_kmh / 100);
    spd_str[1] = '0' + ((spd_kmh / 10) % 10);
    spd_str[2] = '0' + (spd_kmh % 10);
    spd_str[3] = ' ';
    spd_str[4] = 'k'; spd_str[5] = 'm'; spd_str[6] = '/'; spd_str[7] = 'h'; spd_str[8] = '\0';
    FontRenderer::draw_string(cx + 14, cy + 32, spd_str, COLOR_WHITE, 1);

    FontRenderer::draw_bilingual_text(cx + 14, cy + 54, "المسافة", "DIST", 0x00F1C40F, 1);
    char dist_str[16];
    int dist_km = distance / 10;
    dist_str[0] = '0' + ((dist_km / 100) % 10);
    dist_str[1] = '0' + ((dist_km / 10) % 10);
    dist_str[2] = '0' + (dist_km % 10);
    dist_str[3] = ' ';
    dist_str[4] = 'k'; dist_str[5] = 'm'; dist_str[6] = '\0';
    FontRenderer::draw_string(cx + 14, cy + 68, dist_str, COLOR_WHITE, 1);

    // Fuel Gauge Bar
    FontRenderer::draw_bilingual_text(cx + 14, cy + 88, "الوقود", "FUEL", 0x00E67E22, 1);
    int fuel_fill = (80 * fuel) / 100;
    if (fuel_fill < 0) fuel_fill = 0;
    uint32_t f_col = (fuel > 30) ? 0x002ECC71 : 0x00E74C3C;
    VesaDriver::fill_rect(cx + 14, cy + 102, 80, 8, 0x002C3E50);
    VesaDriver::fill_rect(cx + 14, cy + 102, fuel_fill, 8, f_col);

    // Right HUD panel (Score & Restart)
    VesaDriver::fill_round_rect(cx + cw - 112, cy + 12, 104, 70, 6, 0x00141820);
    FontRenderer::draw_bilingual_text(cx + cw - 104, cy + 18, "النقاط", "SCORE", 0x002ECC71, 1);
    char score_str[16];
    score_str[0] = '0' + ((score / 1000) % 10);
    score_str[1] = '0' + ((score / 100) % 10);
    score_str[2] = '0' + ((score / 10) % 10);
    score_str[3] = '0' + (score % 10);
    score_str[4] = '\0';
    FontRenderer::draw_string(cx + cw - 104, cy + 34, score_str, COLOR_WHITE, 1);

    // Controls text
    FontRenderer::draw_bilingual_text(cx + 12, cy + ch - 20, "A/D أو الأسهم للانعطاف | W للنيترو", "Arrows/A,D to steer | W for Nitro", 0x007F8C8D, 1);

    // Crash / Game Over Overlay
    if (crashed) {
        VesaDriver::fill_round_rect(cx + cw / 2 - 120, cy + ch / 2 - 40, 240, 80, 8, 0x004A151B);
        VesaDriver::draw_round_rect(cx + cw / 2 - 120, cy + ch / 2 - 40, 240, 80, 8, 0x00E74C3C);
        FontRenderer::draw_bilingual_text(cx + cw / 2 - 60, cy + ch / 2 - 24, "حادث تحطم!", "CRASHED!", COLOR_WHITE, 1);
        FontRenderer::draw_string(cx + cw / 2 - 70, cy + ch / 2 + 6, "Press R to Retry", 0x00F1C40F, 1);
    }
}

void RacingGameWindow::on_key(uint8_t scancode, char c) {
    if (c == 'r' || c == 'R') {
        reset_game();
        return;
    }

    if (crashed) return;

    // Steer Left
    if (scancode == 0x4B || c == 'a' || c == 'A') {
        car_x -= 16;
        if (car_x < 135) car_x = 135;
    }
    // Steer Right
    else if (scancode == 0x4D || c == 'd' || c == 'D') {
        car_x += 16;
        if (car_x > width - 165) car_x = width - 165;
    }
    // Nitro Boost Up
    else if (scancode == 0x48 || c == 'w' || c == 'W' || scancode == 0x39) {
        speed = 10;
        Speaker::play_sfx(SFX_ENGINE);
    }
    // Brake
    else if (scancode == 0x50 || c == 's' || c == 'S') {
        speed = 3;
    }
}

void RacingGameWindow::on_mouse_click(int mx, int my, int) {
    if (crashed) {
        reset_game();
        Speaker::play_sfx(SFX_CLICK);
    }
}
