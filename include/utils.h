#ifndef UTILS_H
#define UTILS_H

#include "types.h"

/*
 * MMIO access helpers
 * -------------------
 * Used for reading/writing hardware registers safely.
 */

static inline void mmio_write(uint32_t reg, uint32_t value)
{
    *(volatile uint32_t *)reg = value;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t *)reg;
}

/*
 * Simple delay loop
 * -----------------
 * Used when precise timers are not yet available
 * (e.g., GPIO pull-up/down timing).
 */
static inline void delay(uint32_t count)
{
    while (count--) {
        asm volatile("nop");
    }
}

/*
 * Memory barrier helpers
 * ----------------------
 * Prevent compiler/CPU reordering of MMIO operations.
 */
static inline void dmb(void)
{
    asm volatile("dmb sy" ::: "memory");
}

static inline void dsb(void)
{
    asm volatile("dsb sy" ::: "memory");
}

static inline void isb(void)
{
    asm volatile("isb" ::: "memory");
}

#endif /* UTILS_H */