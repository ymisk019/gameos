#include "../include/pit.h"
#include "../include/idt.h"

static volatile uint32_t timer_ticks = 0;

static void timer_callback(registers_t*) {
    timer_ticks++;
}

void pit_init(uint32_t frequency) {
    register_interrupt_handler(32, timer_callback);

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);
}

uint32_t pit_get_ticks(void) {
    return timer_ticks;
}

void sleep_ms(uint32_t ms) {
    // 1 tick = 10ms when PIT is 100Hz
    uint32_t target_ticks = timer_ticks + (ms / 10);
    if (ms > 0 && target_ticks == timer_ticks) target_ticks++;

    while (timer_ticks < target_ticks) {
        __asm__ volatile("pause");
    }
}
