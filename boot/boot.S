#include "arm_regs.h"

.section ".text.boot"
.global _start

_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master_init
    
proc_hang:
    wfe
    b       proc_hang

master_init:
    // 1. Clear the BSS section to zero
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
bss_loop:
    cmp     x0, x1
    b.hs    bss_done
    str     xzr, [x0], #8
    b       bss_loop
bss_done:

    // 2. Set up the Stack Pointer immediately
    // C functions require a valid stack space to execute safely!
    mov     x0, #0x80000
    mov     sp, x0

    // 3. Configure Memory Attributes and Control rules (From Day 2)
    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0

    ldr     x0, =TCR_VALUE
    msr     tcr_el1, x0
    isb

    // 4. Call our Day 4 C code to build the 4-Level Page Tables
    bl      mmu_init

    // 5. Tell the CPU where the top of the pyramid (L0 table) sits
    ldr     x0, =l0_table
    msr     ttbr0_el1, x0
    isb

    // ====================================================================
    // DAY 5 CORE ACTIVATION: Flipping the Master Switch
    // ====================================================================
    
    mrs     x0, sctlr_el1          // Read current System Control state
    ldr     x1, =SCTLR_ENABLE_FLAGS
    orr     x0, x0, x1             // Bind the MMU, Data, and Instruction cache bits
    msr     sctlr_el1, x0          // Write it back to the hardware core!
    
    // Memory Barriers: Force the CPU to complete all pending memory
    // operations and clear its pre-fetch pipe before executing next line
    dsb     sy
    isb

    // 6. Jump to your main kernel logic 
    // From this exact instruction onward, you are officially running in Virtual Memory!
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0                // Vector Base Address Register
    bl      kernel_main