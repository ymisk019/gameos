#include "../include/apps.h"
#include "../include/vesa.h"
#include "../include/font.h"
#include "../include/speaker.h"
#include "../include/vfs.h"

FileManagerWindow::FileManagerWindow(int id)
    : Window(id, "مدير الملفات | File Manager", 150, 80, 480, 340), selected_item(-1) {
    kstrcpy(current_path, "/");
    current_node = VFS::get_root();
}

void FileManagerWindow::on_render(int cx, int cy, int cw, int ch) {
    VesaDriver::fill_rect(cx, cy, cw, ch, 0x001B2028);

    // Path Bar
    VesaDriver::fill_rect(cx, cy, cw, 28, 0x00141820);
    VesaDriver::draw_line(cx, cy + 28, cx + cw, cy + 28, 0x002A3240);

    // Back button
    VesaDriver::fill_round_rect(cx + 6, cy + 4, 32, 20, 3, 0x002C3E50);
    FontRenderer::draw_string(cx + 14, cy + 6, "<-", COLOR_WHITE, 1);

    // Path text
    FontRenderer::draw_string(cx + 46, cy + 8, current_path, 0x0000D2D3, 1);

    // Files List Header
    int list_y = cy + 34;
    VesaDriver::fill_rect(cx, list_y, cw, 20, 0x00202732);
    FontRenderer::draw_bilingual_text(cx + 12, list_y + 4, "الاسم", "Name", COLOR_LIGHT_GRAY, 1);
    FontRenderer::draw_bilingual_text(cx + 260, list_y + 4, "النوع", "Type", COLOR_LIGHT_GRAY, 1);
    FontRenderer::draw_bilingual_text(cx + 380, list_y + 4, "الحجم", "Size", COLOR_LIGHT_GRAY, 1);

    if (!current_node || current_node->type != FS_DIRECTORY) return;

    int item_y = list_y + 24;
    for (uint32_t i = 0; i < current_node->child_count; i++) {
        FileNode* child = current_node->children[i];
        if (!child) continue;

        bool is_sel = (selected_item == (int)i);
        uint32_t bg = is_sel ? 0x000F4C81 : ((i % 2 == 0) ? 0x001A1E26 : 0x00171B22);
        VesaDriver::fill_rect(cx + 4, item_y, cw - 8, 22, bg);

        // Icon
        if (child->type == FS_DIRECTORY) {
            VesaDriver::fill_round_rect(cx + 8, item_y + 4, 14, 14, 2, 0x00F39C12); // Folder icon
        } else if (child->type == FS_GAME_PACKAGE) {
            VesaDriver::fill_round_rect(cx + 8, item_y + 4, 14, 14, 2, 0x00E74C3C); // Game icon
        } else {
            VesaDriver::fill_round_rect(cx + 8, item_y + 4, 14, 14, 2, 0x003498DB); // File icon
        }

        // File name
        FontRenderer::draw_string(cx + 28, item_y + 5, child->name, COLOR_WHITE, 1);

        // File type
        const char* type_str = (child->type == FS_DIRECTORY) ? "Folder" : ((child->type == FS_GAME_PACKAGE) ? "Game YGP" : "Text/Data");
        FontRenderer::draw_string(cx + 260, item_y + 5, type_str, 0x0095A5A6, 1);

        // File size
        char size_str[16];
        if (child->type == FS_DIRECTORY) {
            kstrcpy(size_str, "--");
        } else {
            int sz = child->size;
            if (sz < 1024) {
                size_str[0] = '0' + (sz / 100);
                size_str[1] = '0' + ((sz / 10) % 10);
                size_str[2] = '0' + (sz % 10);
                size_str[3] = ' ';
                size_str[4] = 'B';
                size_str[5] = '\0';
            } else {
                kstrcpy(size_str, "1 KB");
            }
        }
        FontRenderer::draw_string(cx + 380, item_y + 5, size_str, 0x0095A5A6, 1);

        item_y += 24;
        if (item_y > cy + ch - 24) break;
    }
}

void FileManagerWindow::on_mouse_click(int mx, int my, int) {
    // Back button
    if (mx >= 6 && mx < 38 && my >= 4 && my < 24) {
        if (current_node && current_node->parent) {
            current_node = current_node->parent;
            // Update path string
            int last_slash = -1;
            int len = kstrlen(current_path);
            for (int i = len - 1; i >= 0; i--) {
                if (current_path[i] == '/') {
                    last_slash = i;
                    break;
                }
            }
            if (last_slash > 0) {
                current_path[last_slash] = '\0';
            } else {
                current_path[1] = '\0';
            }
            selected_item = -1;
            Speaker::play_sfx(SFX_CLICK);
        }
        return;
    }

    // Item selection / double-click
    int list_y = 34 + 24;
    if (my >= list_y && current_node) {
        int idx = (my - list_y) / 24;
        if (idx >= 0 && idx < (int)current_node->child_count) {
            if (selected_item == idx) {
                // Open folder
                FileNode* child = current_node->children[idx];
                if (child->type == FS_DIRECTORY) {
                    current_node = child;
                    if (kstrcmp(current_path, "/") != 0) {
                        kstrcat(current_path, "/");
                    }
                    kstrcat(current_path, child->name);
                    selected_item = -1;
                    Speaker::play_sfx(SFX_CLICK);
                    return;
                }
            }
            selected_item = idx;
            Speaker::play_sfx(SFX_CLICK);
        }
    }
}
