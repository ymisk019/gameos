#include "../include/rtc.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static int bcd_to_bin(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

void RTC::init() {
    // Initial read
    get_time();
}

SystemTime RTC::get_time() {
    SystemTime t;
    
    // Wait until RTC update in progress bit (bit 7 of register A) is clear
    outb(CMOS_ADDRESS, 0x0A);
    while (inb(CMOS_DATA) & 0x80);

    t.second = get_rtc_register(0x00);
    t.minute = get_rtc_register(0x02);
    t.hour   = get_rtc_register(0x04);
    t.day    = get_rtc_register(0x07);
    t.month  = get_rtc_register(0x08);
    uint32_t year = get_rtc_register(0x09);

    uint8_t regB = get_rtc_register(0x0B);

    // Convert BCD to binary if necessary
    if (!(regB & 0x04)) {
        t.second = bcd_to_bin(t.second);
        t.minute = bcd_to_bin(t.minute);
        t.hour   = ((t.hour & 0x7F) != t.hour) ? (bcd_to_bin(t.hour & 0x7F) | 0x80) : bcd_to_bin(t.hour);
        t.day    = bcd_to_bin(t.day);
        t.month  = bcd_to_bin(t.month);
        year     = bcd_to_bin(year);
    }

    t.year = 2000 + year;
    return t;
}

void RTC::format_time_string(char* buf) {
    SystemTime t = get_time();
    
    // Format: HH:MM
    buf[0] = '0' + (t.hour / 10);
    buf[1] = '0' + (t.hour % 10);
    buf[2] = ':';
    buf[3] = '0' + (t.minute / 10);
    buf[4] = '0' + (t.minute % 10);
    buf[5] = '\0';
}
