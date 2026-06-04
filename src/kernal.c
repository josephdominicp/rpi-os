#include <stdint.h>
#include "task.h"

// Pull in external subsystem initialization prototypes
extern void uart_init(void);              // Your baseline UART initializer
extern void uart_puts(const char* s);     // Your baseline UART print function
extern void mmu_init(void);               // Day 3/4/5 MMU setup
extern void timer_init(void);             // Day 9 Timer setup
extern void exception_vector_table(void); // Day 10 Exception Vector Table

// --- TEST TASK 1 ---
void task_1(void) {
    while (1) {
        uart_puts(" [1] Task 1 is executing...\n");
        // A simple delay loop so the UART output isn't entirely flooded
        for (volatile int i = 0; i < 5000000; i++);
    }
}

// --- TEST TASK 2 ---
void task_2(void) {
    while (1) {
        uart_puts(" [2] Task 2 is executing...\n");
        for (volatile int i = 0; i < 5000000; i++);
    }
}

// --- MAIN KERNEL ENTRY POINT ---
void kernel_main(void) {
    // 1. Initialize your hardware UART for diagnostic logs
    uart_init();
    uart_puts("\n--- RPIOS Bare-Metal OS Booting ---\n");

    // 2. Initialize the Virtual Memory System (MMU & Caches)
    mmu_init();
    uart_puts("[OK] MMU Identity Mapping and Caching Active.\n");

    // 3. Initialize the Task Control Pool (Registers the current line as Task 0)
    task_init();
    uart_puts("[OK] Task Control Subsystem Initialized.\n");

    // 4. Register our two separate test functions as isolated threads
    task_create(task_1, 1);
    task_create(task_2, 1);
    uart_puts("[OK] Preemptive Thread Tasks Created.\n");

    // 5. Hand our Day 10 Exception Table address over to the CPU hardware core
    uint64_t vbar = (uint64_t)&exception_vector_table;
    asm volatile("msr vbar_el1, %0" :: "r"(vbar));
    uart_puts("[OK] Exception Vector Table Registered via VBAR_EL1.\n");

    // 6. Program the ARM Generic Timer to strike every 10ms
    timer_init();
    uart_puts("[OK] ARM Generic Timer programmed for 10ms intervals.\n");

    // 7. CRITICAL STEP: Globally unmask IRQ hardware interrupts on the processor core
    uart_puts("[LIVE] Unmasking Core IRQ Lines. Multi-Tasking starting now...\n\n");
    asm volatile("msr daifclr, #2"); // Clears the IRQ mask bit in DAIF register

    // 8. THE IDLE LOOP (Task 0)
    // The main execution stream falls back to being an idle background thread.
    // Every 10ms, the timer will hit, look at this loop, freeze it, and 
    // cycle out to Task 1 and Task 2.
    while (1) {
        // Background baseline execution context
        asm volatile("wfi"); // "Wait For Interrupt" - puts CPU into low-power mode until the next tick
    }
}