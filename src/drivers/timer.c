#include "timer.h"
#include "arm_regs.h"
#include "utils.h"

/*
 * Timer interval (microseconds)
 * 1,000,000 us = 1 second
 */
#define TIMER_INTERVAL 1000000

/*
 * Initialize system timer
 * Uses System Timer Compare 1
 */
void timer_init(void)
{
    uint32_t current_time;

    /* Read current timer value */
    current_time = mmio_read(SYS_TIMER_CLO);

    /* Schedule next interrupt */
    mmio_write(SYS_TIMER_C1, current_time + TIMER_INTERVAL);

    /* Clear any pending timer interrupt */
    mmio_write(SYS_TIMER_CS, (1 << 1));
}