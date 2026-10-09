#include "../include/power.h"
#include "../include/types.h"

void Power::shutdown() {
    // 1. QEMU shutdown via isa-debug-exit or acpi port
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);
    outw(0x4004, 0x3400);

    // 2. APM shutdown via BIOS call emulator port
    outw(0x8900, 0x2000);

    // If still alive, halt CPU in tight loop
    cli();
    while (1) {
        hlt();
    }
}

void Power::reboot() {
    cli();

    // Pulse reset line using 8042 Keyboard Controller
    uint8_t temp;
    do {
        temp = inb(0x64);
        if (temp & 1) inb(0x60);
    } while (temp & 2);

    outb(0x64, 0xFE); // Pulse reset

    // Fallback: triple fault via empty IDT
    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) null_idt = { 0, 0 };

    __asm__ volatile("lidt %0" : : "m"(null_idt));
    __asm__ volatile("int $3");

    while (1) {
        hlt();
    }
}
