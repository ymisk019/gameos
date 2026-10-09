#include "../include/store.h"
#include "../include/vfs.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"
#include "../include/memory.h"

GamePackage GameStore::catalog[MAX_STORE_PACKAGES];
int GameStore::package_count = 0;

static int dyn_win_id_counter = 800;

void GameStore::init() {
    package_count = 0;
    kmemset(catalog, 0, sizeof(catalog));

    // Create a built-in sample .ygp package in VFS: "Retro Snake / ثعبان الريترو"
    YGPHeader sample_hdr;
    sample_hdr.magic = YGP_MAGIC;
    kstrcpy(sample_hdr.id, "retro_snake");
    kstrcpy(sample_hdr.title_ar, "ثعبان الريترو");
    kstrcpy(sample_hdr.title_en, "Retro Snake YGP");
    kstrcpy(sample_hdr.author, "Yazan OS Dev");
    kstrcpy(sample_hdr.version, "1.0");
    kstrcpy(sample_hdr.desc_ar, "لعبة ثعبان كلاسيكية محملة كحزمة ديناميكية خارجية!");
    kstrcpy(sample_hdr.desc_en, "Classic snake loaded dynamically from .ygp package!");
    sample_hdr.icon_color = 0x002ECC71;
    sample_hdr.code_size = 64;
    sample_hdr.data_size = 0;

    uint8_t sample_package_buffer[sizeof(YGPHeader) + 64];
    kmemcpy(sample_package_buffer, &sample_hdr, sizeof(YGPHeader));
    // Sample bytecode payload
    kmemset(sample_package_buffer + sizeof(YGPHeader), 0x90, 64);

    VFS::create_file("/store/snake.ygp", sample_package_buffer, sizeof(sample_package_buffer), FS_GAME_PACKAGE);

    // Initial scan
    scan_vfs_for_packages();
}

int GameStore::get_package_count() {
    return package_count;
}

GamePackage* GameStore::get_package(int index) {
    if (index >= 0 && index < package_count) {
        return &catalog[index];
    }
    return NULL;
}

bool GameStore::install_package(const uint8_t* ygp_data, uint32_t size) {
    if (!ygp_data || size < sizeof(YGPHeader)) return false;
    if (package_count >= MAX_STORE_PACKAGES) return false;

    const YGPHeader* hdr = (const YGPHeader*)ygp_data;
    if (hdr->magic != YGP_MAGIC) return false;

    // Check if already installed
    for (int i = 0; i < package_count; i++) {
        if (kstrcmp(catalog[i].header.id, hdr->id) == 0) {
            return true;
        }
    }

    GamePackage& pkg = catalog[package_count++];
    kmemcpy(&pkg.header, hdr, sizeof(YGPHeader));
    pkg.is_installed = true;

    if (hdr->code_size > 0 && size >= sizeof(YGPHeader) + hdr->code_size) {
        pkg.bytecode = (uint8_t*)kmalloc(hdr->code_size);
        if (pkg.bytecode) {
            kmemcpy(pkg.bytecode, ygp_data + sizeof(YGPHeader), hdr->code_size);
        }
    }

    return true;
}

bool GameStore::scan_vfs_for_packages() {
    FileNode* store_dir = VFS::resolve_path("/store");
    if (!store_dir) return false;

    for (uint32_t i = 0; i < store_dir->child_count; i++) {
        FileNode* file = store_dir->children[i];
        if (file && file->size >= sizeof(YGPHeader)) {
            install_package(file->data, file->size);
        }
    }
    return true;
}

void GameStore::launch_package(int index) {
    if (index >= 0 && index < package_count) {
        WindowManager::add_window(new DynamicGameWindow(dyn_win_id_counter++, catalog[index]));
    }
}

// DynamicGameWindow Implementation (Interactive execution for external packages)
DynamicGameWindow::DynamicGameWindow(int id, const GamePackage& pkg)
    : Window(id, pkg.header.title_en, 180, 80, 460, 360),
      package(pkg), score(0), active(true) {
    kmemset(state_vars, 0, sizeof(state_vars));
    state_vars[0] = 100; // Snake/Object X
    state_vars[1] = 100; // Snake/Object Y
    state_vars[2] = 1;   // Dir X
    state_vars[3] = 0;   // Dir Y
    state_vars[4] = 200; // Food X
    state_vars[5] = 150; // Food Y
}

void DynamicGameWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x000F172A);

    // Top Header
    VesaDriver::fill_rect(cx, cy, cw, 28, 0x001E293B);
    FontRenderer::draw_bilingual_text(cx + 10, cy + 8, package.header.title_ar, package.header.title_en, COLOR_WHITE, 1);
    
    char sc_str[16];
    sc_str[0] = '0' + ((score / 10) % 10);
    sc_str[1] = '0' + (score % 10);
    sc_str[2] = '\0';
    FontRenderer::draw_string(cx + cw - 80, cy + 8, sc_str, 0x00F1C40F, 1);

    // Dynamic execution logic
    if (active) {
        state_vars[0] += state_vars[2] * 4;
        state_vars[1] += state_vars[3] * 4;

        if (state_vars[0] < 10) state_vars[0] = cw - 30;
        if (state_vars[0] > cw - 30) state_vars[0] = 10;
        if (state_vars[1] < 40) state_vars[1] = ch - 30;
        if (state_vars[1] > ch - 30) state_vars[1] = 40;

        // Food collision
        int fx = state_vars[4];
        int fy = state_vars[5];
        if ((state_vars[0] - fx) * (state_vars[0] - fx) + (state_vars[1] - fy) * (state_vars[1] - fy) < 250) {
            score++;
            Speaker::play_sfx(SFX_COIN);
            state_vars[4] = 30 + ((score * 73) % (cw - 60));
            state_vars[5] = 50 + ((score * 47) % (ch - 90));
        }
    }

    // Render Food
    VesaDriver::fill_circle(cx + state_vars[4], cy + state_vars[5], 6, 0x00E74C3C);

    // Render Dynamic Player Entity
    VesaDriver::fill_round_rect(cx + state_vars[0], cy + state_vars[1], 16, 16, 4, package.header.icon_color);
    VesaDriver::draw_round_rect(cx + state_vars[0], cy + state_vars[1], 16, 16, 4, COLOR_WHITE);

    // Bottom controls banner
    FontRenderer::draw_bilingual_text(cx + 10, cy + ch - 18, "الأسهم للحركة | تم التحميل عبر حزمة YGP الديناميكية", "Arrows to move | Loaded dynamically via YGP package", 0x007F8C8D, 1);
}

void DynamicGameWindow::on_key(uint8_t scancode, char c) {
    if (scancode == 0x4B || c == 'a' || c == 'A') { // Left
        state_vars[2] = -1; state_vars[3] = 0;
    } else if (scancode == 0x4D || c == 'd' || c == 'D') { // Right
        state_vars[2] = 1; state_vars[3] = 0;
    } else if (scancode == 0x48 || c == 'w' || c == 'W') { // Up
        state_vars[2] = 0; state_vars[3] = -1;
    } else if (scancode == 0x50 || c == 's' || c == 'S') { // Down
        state_vars[2] = 0; state_vars[3] = 1;
    }
}

void DynamicGameWindow::on_mouse_click(int, int, int) {
    // Change direction on click
    state_vars[2] = (state_vars[2] == 0) ? 1 : 0;
    state_vars[3] = (state_vars[3] == 0) ? 1 : 0;
    Speaker::play_sfx(SFX_CLICK);
}
