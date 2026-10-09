#ifndef RTC_H
#define RTC_H

#include "types.h"

struct SystemTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
};

class RTC {
public:
    static void init();
    static SystemTime get_time();
    static void format_time_string(char* buf);
};

#endif // RTC_H
