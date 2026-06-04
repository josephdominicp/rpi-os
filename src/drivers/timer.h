#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

extern volatile uint64_t system_ticks;

void timer_init(void);
void handle_timer_irq(void);

#endif