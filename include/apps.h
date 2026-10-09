#ifndef APPS_H
#define APPS_H

#include "window.h"
#include "vfs.h"

class SettingsWindow : public Window {
private:
    int active_tab; // 0: Display, 1: Audio, 2: Language, 3: System Info
public:
    SettingsWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int ch) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

class FileManagerWindow : public Window {
private:
    char current_path[MAX_FILENAME_LEN * 2];
    FileNode* current_node;
    int selected_item;
public:
    FileManagerWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int ch) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

class GameManagerWindow : public Window {
private:
    int selected_game;
    int active_tab; // 0: Library, 1: Store / Package Manager
    char status_msg[64];
public:
    GameManagerWindow(int id);
    virtual void on_render(int cx, int cy, int cw, int ch) override;
    virtual void on_mouse_click(int mx, int my, int btn) override;
};

#endif // APPS_H
