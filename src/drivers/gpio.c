#include "gpio.h"
#include "arm_regs.h"
#include "utils.h"
static inline void mmio_write(unsigned int reg, unsigned int data)
{
    *(volatile unsigned int *)reg = data;
}

static inline unsigned int mmio_read(unsigned int reg)
{
    return *(volatile unsigned int *)reg;
}
void gpio_init_uart(void)
{
    unsigned int r;

    /* Set GPIO14 and GPIO15 to ALT5 */
    r = mmio_read(GPFSEL1);
    r &= ~((7 << 12) | (7 << 15));
    r |=  (2 << 12) | (2 << 15);
    mmio_write(GPFSEL1, r);

    /* Disable pull-up/down */
    mmio_write(GPPUD, 0);
    for (r = 0; r < 150; r++);

    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    for (r = 0; r < 150; r++);

    mmio_write(GPPUDCLK0, 0);
}