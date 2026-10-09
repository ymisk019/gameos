#ifndef PIT_H
#define PIT_H

#include "types.h"

#define PIT_FREQUENCY_HZ 100

void pit_init(uint32_t frequency);
uint32_t pit_get_ticks(void);
void sleep_ms(uint32_t ms);

#endif // PIT_H
