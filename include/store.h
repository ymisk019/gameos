#ifndef STORE_H
#define STORE_H

#include "types.h"
#include "window.h"

#define YGP_MAGIC 0x31504759 // "YGP1"
#define MAX_STORE_PACKAGES 16

struct YGPHeader {
    uint32_t magic;
    char id[16];
    char title_ar[32];
    char title_en[32];
    char author[32];
    char version[16];
    char desc_ar[64];
    char desc_en[64];
    uint32_t icon_color;
    uint32_t code_size;
    uint32_t data_size;
} __attribute__((packed));

struct GamePackage {
    YGPHeader header;
    uint8_t* bytecode;
    uint8_t* assets;
    bool is_installed;
};

class DynamicGameWindow : public Window {
private:
    GamePackage package;
    int state_vars[16];
    int score;
    bool active;
public:
    DynamicGameWindow(int id, const GamePackage& pkg);
    virtual void on_render(int cx, int cy, int cw, int ch) override;
    virtual void on_key(uint8_t scancode, char c) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

class GameStore {
private:
    static GamePackage catalog[MAX_STORE_PACKAGES];
    static int package_count;

public:
    static void init();
    static int get_package_count();
    static GamePackage* get_package(int index);
    static bool install_package(const uint8_t* ygp_data, uint32_t size);
    static bool scan_vfs_for_packages();
    static void launch_package(int index);
};

#endif // STORE_H
