#include "../include/kernel.h"

// Multiboot 1 info structure definition
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed));

extern "C" void kernel_main(uint32_t magic, uint32_t* info_ptr) {
    // 1. Initialize CPU Core structures
    gdt_init();
    idt_init();
    pic_remap(0x20, 0x28);
    pit_init(PIT_FREQUENCY_HZ);

    // 2. Initialize Memory Manager (Heap from 4MB to 36MB = 32 MB heap)
    memory_init(0x00400000, 32 * 1024 * 1024);

    // 3. Setup Graphics Framebuffer
    // Default fallback to 0xFD000000 (standard QEMU/Bochs/Bochs-VBE linear address) or Multiboot VBE
    uint32_t* lfb_addr = (uint32_t*)0xFD000000;
    int screen_w = 800;
    int screen_h = 600;
    int screen_bpp = 32;

    if (magic == 0x2BADB002 && info_ptr != NULL) {
        multiboot_info* mbi = (multiboot_info*)info_ptr;
        if (mbi->flags & (1 << 12)) { // Framebuffer info available
            if (mbi->framebuffer_addr != 0) {
                lfb_addr = (uint32_t*)(uintptr_t)mbi->framebuffer_addr;
                screen_w = mbi->framebuffer_width;
                screen_h = mbi->framebuffer_height;
                screen_bpp = mbi->framebuffer_bpp;
            }
        }
    }

    VesaDriver::init(lfb_addr, screen_w, screen_h, screen_bpp);

    // 4. Initialize Hardware Drivers
    Keyboard::init();
    Mouse::init(screen_w, screen_h);
    Speaker::init();
    Gamepad::init();
    RTC::init();

    // 5. Initialize VFS Filesystem & Font Engine
    VFS::init();
    FontRenderer::init();
    FontRenderer::set_language(LANG_ARABIC); // Default to Arabic

    // 6. Initialize Window Manager & Compositor
    WindowManager::init();
    DesktopCompositor::init();

    // 7. Initialize Game Store & Dynamic Packages
    GameStore::init();

    // 8. Enable CPU Interrupts
    sti();

    // 9. Show Beautiful Animated Boot Splash Screen with Sound!
    DesktopCompositor::show_splash_screen();

    // 10. Open Initial Welcome Windows
    // Open Game Manager & Store
    WindowManager::add_window(new GameManagerWindow(1));
    // Open Game 1 (Yazan's Quest)
    WindowManager::add_window(new AdventureGameWindow(2));

    // 11. Main OS Event Loop
    int prev_mx = Mouse::get_x();
    int prev_my = Mouse::get_y();

    while (1) {
        // Poll Gamepad
        Gamepad::update();

        // Check Keyboard character events
        while (Keyboard::has_char()) {
            char c = Keyboard::read_char();
            WindowManager::handle_key(0, c);
        }

        // Check Keyboard raw keys for active window / games
        for (uint8_t sc = 1; sc < 128; sc++) {
            if (Keyboard::is_key_pressed(sc)) {
                WindowManager::handle_key(sc, 0);
            }
        }

        // Mouse motion detection
        int cur_mx = Mouse::get_x();
        int cur_my = Mouse::get_y();
        if (cur_mx != prev_mx || cur_my != prev_my) {
            WindowManager::handle_mouse_move(cur_mx, cur_my);
            prev_mx = cur_mx;
            prev_my = cur_my;
        }

        // Mouse clicks
        if (Mouse::is_left_clicked()) {
            DesktopCompositor::handle_click(cur_mx, cur_my, 1);
        }
        if (!Mouse::is_left_down()) {
            WindowManager::handle_mouse_up(cur_mx, cur_my, 1);
        }

        // Composite and render desktop frame
        DesktopCompositor::update_and_render();

        // Frame rate limiter / power-saving sleep (~30-60 FPS)
        sleep_ms(16);
    }
}
