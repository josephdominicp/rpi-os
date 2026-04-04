#ifndef IRQ_H
#define IRQ_H

/*
 * Initialize IRQ handling
 * (enable required interrupt sources)
 */
void irq_init(void);

/*
 * Global IRQ enable / disable
 */
void irq_enable(void);
void irq_disable(void);

/*
 * Top-level IRQ handler
 * Called from vectors.S
 */
void irq_handler(void);

#endif /* IRQ_H */