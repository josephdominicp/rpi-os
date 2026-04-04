#ifndef ARM_REGS_H
#define ARM_REGS_H

/*
 * Raspberry Pi 3 (BCM2837) Peripheral Base
 */
#define MMIO_BASE       0x3F000000UL

/*
 * =====================
 * GPIO REGISTERS
 * =====================
 */
#define GPIO_BASE       (MMIO_BASE + 0x200000)

/* GPIO Function Select */
#define GPFSEL0         (GPIO_BASE + 0x00)
#define GPFSEL1         (GPIO_BASE + 0x04)
#define GPFSEL2         (GPIO_BASE + 0x08)

/* GPIO Pull-up / Pull-down */
#define GPPUD           (GPIO_BASE + 0x94)
#define GPPUDCLK0       (GPIO_BASE + 0x98)

/*
 * =====================
 * AUX / MINI UART
 * =====================
 */
#define AUX_BASE            (MMIO_BASE + 0x215000)

/* AUX enable register */
#define AUX_ENABLES         (AUX_BASE + 0x04)

/* Mini UART registers */
#define AUX_MU_IO_REG       (AUX_BASE + 0x40)
#define AUX_MU_IER_REG      (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG      (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG      (AUX_BASE + 0x4C)
#define AUX_MU_MCR_REG      (AUX_BASE + 0x50)
#define AUX_MU_LSR_REG      (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG     (AUX_BASE + 0x60)
#define AUX_MU_BAUD_REG     (AUX_BASE + 0x68)

/*
 * =====================
 * SYSTEM TIMER
 * (Used later for interrupts)
 * =====================
 */
#define SYS_TIMER_BASE  (MMIO_BASE + 0x3000)

#define SYS_TIMER_CS    (SYS_TIMER_BASE + 0x00)
#define SYS_TIMER_CLO   (SYS_TIMER_BASE + 0x04)
#define SYS_TIMER_CHI   (SYS_TIMER_BASE + 0x08)
#define SYS_TIMER_C1    (SYS_TIMER_BASE + 0x10)

/*
 * =====================
 * INTERRUPT CONTROLLER
 * (Basic IRQ support)
 * =====================
 */
#define IRQ_BASE                (MMIO_BASE + 0xB200)

#define IRQ_PENDING_1           (IRQ_BASE + 0x204)
#define ENABLE_IRQS_1           (IRQ_BASE + 0x210)
#define DISABLE_IRQS_1          (IRQ_BASE + 0x21C)

#endif /* ARM_REGS_H */