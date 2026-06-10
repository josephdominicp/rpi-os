#include "irq.h"
#include "arm_regs.h"
#include "utils.h"
#include "uart.h"
#include "timer.h"

static uint32_t tick = 0;

void irq_enable(void)
{
    asm volatile("msr daifclr, #2");
}

void irq_disable(void)
{
    asm volatile("msr daifset, #2");
}

void irq_init(void)
{
    /* Enable System Timer Compare 1 interrupt */
    mmio_write(ENABLE_IRQS_1, (1 << 1));
}

void irq_handler(void)
{
    uint32_t irq_pending;

    irq_pending = mmio_read(IRQ_PENDING_1);

    /* System Timer Compare 1 interrupt */
    if (irq_pending & (1 << 1)) {

        /* Clear interrupt */
        mmio_write(SYS_TIMER_CS, (1 << 1));

        /* Increment tick counter */
        tick++;

        /* Debug output (temporary for Day 8) */
        uart_puts("Timer tick: ");
        uart_putc('0' + (tick % 10));
        uart_puts("\n");

        /* Schedule next interrupt */
        timer_init();
    }
}