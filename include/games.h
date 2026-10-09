#ifndef GAMES_H
#define GAMES_H

#include "window.h"

// Game 1: Adventure ("مغامرات يزن" / "Yazan's Quest")
class AdventureGameWindow : public Window {
private:
    int player_x, player_y;
    int player_hp;
    int player_keys;
    int player_gems;
    int level;
    bool game_won;
    bool game_over;
    uint8_t map[12][16];
    struct Enemy {
        int x, y;
        int dir;
        bool alive;
    } enemies[4];

    void init_level();

public:
    AdventureGameWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int sh) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

// Game 2: Racing ("سباق الصحراء" / "Desert Speed")
class RacingGameWindow : public Window {
private:
    int car_x;
    int speed;
    int distance;
    int score;
    int fuel;
    bool crashed;
    struct TrafficCar {
        int x, y;
        int speed;
        uint32_t color;
        bool active;
    } traffic[5];
    int fuel_x, fuel_y;
    int road_offset;

    void reset_game();

public:
    RacingGameWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int sh) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

// Game 3: Platformer ("قفزة السايبر" / "Cyber Jumper")
class PlatformerGameWindow : public Window {
private:
    float px, py;
    float vx, vy;
    bool on_ground;
    int jump_count;
    int score;
    int coins_left;
    int lives;
    bool level_cleared;
    bool game_over;
    
    struct Platform {
        int x, y, w, h;
    } platforms[10];

    struct Coin {
        int x, y;
        bool collected;
    } coins[10];

    struct Hazard {
        int x, y, w;
    } hazards[4];

    void reset_level();

public:
    PlatformerGameWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int sh) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

// Game 4: Shooter ("حامي المجرة" / "Galactic Defender")
class ShooterGameWindow : public Window {
private:
    int ship_x, ship_y;
    int hp;
    int score;
    bool game_over;
    
    struct Bullet {
        int x, y;
        bool active;
    } bullets[20];

    struct Alien {
        int x, y;
        int hp;
        int type;
        bool active;
    } aliens[12];

    struct Particle {
        int x, y;
        int vx, vy;
        int life;
        uint32_t color;
    } particles[30];

    void spawn_aliens();
    void reset_game();
    void add_explosion(int x, int y);

public:
    ShooterGameWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int sh) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

// Game 5: Puzzle ("مكعبات الذكاء" / "Mind Blocks")
class PuzzleGameWindow : public Window {
private:
    int grid[4][4];
    int score;
    int best_score;
    bool won;
    bool lost;

    void add_random_tile();
    bool move_left();
    bool move_right();
    bool move_up();
    bool move_down();
    bool can_move();
    void reset_game();

public:
    PuzzleGameWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int sh) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

#endif // GAMES_H
