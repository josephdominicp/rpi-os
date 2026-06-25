#include "arm_regs.h"

.section ".text.boot"
.global _start

_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, primary_cpu
    
proc_hang:
    wfe
    b       proc_hang

primary_cpu:
    // ====================================================================
    // EXCEPTION LEVEL DROP: EL3 -> EL1
    // QEMU raspi3b starts at EL3. We must drop to EL1 before configuring
    // EL1 system registers (MAIR, TCR, TTBR, SCTLR, VBAR).
    // ====================================================================

    // Check current exception level
    mrs     x0, CurrentEL
    and     x0, x0, #0xC           // Extract EL field (bits [3:2])
    cmp     x0, #0x4               // Are we at EL1 already?
    b.eq    at_el1                  // If so, skip the drop

    // --- At EL3: Configure EL2/EL1 and drop to EL1 ---

    // Allow EL1 to access the physical timer
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3             // EL1PCTEN | EL1PCEN
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr       // Clear virtual timer offset

    // Configure HCR_EL2: Set EL1 to use AArch64
    mov     x0, #(1 << 31)         // RW bit = AArch64 for EL1
    msr     hcr_el2, x0

    // Configure SCR_EL3: Route to EL1 via EL2, set AArch64 for EL2
    mov     x0, #(1 << 10)         // RW bit = AArch64 for EL2
    orr     x0, x0, #(1 << 0)      // NS bit = Non-Secure
    msr     scr_el3, x0

    // Set up SPSR_EL3: Return to EL1h (SP_EL1) with DAIF masked
    mov     x0, #0x3c5             // EL1h | DAIF masked
    msr     spsr_el3, x0

    // Set the return address to at_el1
    adr     x0, at_el1
    msr     elr_el3, x0

    eret                            // Drop from EL3 -> EL1

at_el1:
    // ====================================================================
    // NOW RUNNING AT EL1
    // ====================================================================

    // 1. Clear the BSS section to zero
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
bss_loop:
    cmp     x0, x1
    b.hs    bss_done
    str     xzr, [x0], #8
    b       bss_loop
bss_done:

    // 2. Set up the Stack Pointer
    mov     x0, #0x80000
    mov     sp, x0

    // 3. Configure Memory Attributes and Control rules
    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0

    ldr     x0, =TCR_VALUE
    msr     tcr_el1, x0
    isb

    // 4. Call C code to build the 4-Level Page Tables
    bl      mmu_init

    // 5. Tell the CPU where the L0 table sits
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
    
    // Memory Barriers
    dsb     sy
    isb

    // 6. Jump to your main kernel logic 
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0
    bl      kernel_main

    // If kernel_main returns, hang
    b       proc_hang