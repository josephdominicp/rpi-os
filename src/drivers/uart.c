#include "uart.h"
#include "gpio.h"
#include "arm_regs.h"
#include "utils.h"

/*
 * Initialize Mini UART
 */
static inline void mmio_write(unsigned int reg, unsigned int data)
{
    *(volatile unsigned int *)reg = data;
}

static inline unsigned int mmio_read(unsigned int reg)
{
    return *(volatile unsigned int *)reg;
}
void uart_init(void)
{
    /* Enable Mini UART */
    mmio_write(AUX_ENABLES, 1);

    /* Disable transmitter and receiver during configuration */
    mmio_write(AUX_MU_CNTL_REG, 0);

    /* Disable interrupts */
    mmio_write(AUX_MU_IER_REG, 0);

    /* Set UART to 8-bit mode */
    mmio_write(AUX_MU_LCR_REG, 3);

    /* Disable auto flow control */
    mmio_write(AUX_MU_MCR_REG, 0);

    /* Clear FIFOs */
    mmio_write(AUX_MU_IIR_REG, 0xC6);

    /* Set baud rate to 115200 */
    mmio_write(AUX_MU_BAUD_REG, 270);

    /* Configure GPIO pins for UART */
    gpio_init_uart();

    /* Enable transmitter and receiver */
    mmio_write(AUX_MU_CNTL_REG, 3);
}

/*
 * Send a single character
 */
void uart_putc(char c)
{
    /* Wait until transmitter is ready */
    while (!(mmio_read(AUX_MU_LSR_REG) & 0x20))
        ;

    mmio_write(AUX_MU_IO_REG, (unsigned int)c);
}

/*
 * Send a null-terminated string
 */
void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');
        uart_putc(*str++);
    }
}
