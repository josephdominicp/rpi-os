.section ".text"
.align 11 // Exception Vector Tables must be strictly aligned to a 2048-byte boundary!
.global exception_vector_table

exception_vector_table:
    // ----------------------------------------------------------------
    // CASE 1: Exceptions from Current EL with SP_EL0 (Not used by our kernel)
    // ----------------------------------------------------------------
    .align 7; b default_handler     // Synchronous
    .align 7; b default_handler     // IRQ
    .align 7; b default_handler     // FIQ
    .align 7; b default_handler     // SError

    // ----------------------------------------------------------------
    // CASE 2: Exceptions from Current EL with SP_ELx (Our Kernel Lives Here!)
    // ----------------------------------------------------------------
    .align 7; b default_handler     // Synchronous 
    .align 7; b irq_handler_el1     // IRQ (Our 10ms Timer Tick will hit this slot!)
    .align 7; b default_handler     // FIQ
    .align 7; b default_handler     // SError

    // ----------------------------------------------------------------
    // CASE 3: Exceptions from Lower EL using AArch64 (For future User Mode apps)
    // ----------------------------------------------------------------
    .align 7; b default_handler
    .align 7; b default_handler
    .align 7; b default_handler
    .align 7; b default_handler

    // ----------------------------------------------------------------
    // CASE 4: Exceptions from Lower EL using AArch32 (Unused)
    // ----------------------------------------------------------------
    .align 7; b default_handler
    .align 7; b default_handler
    .align 7; b default_handler
    .align 7; b default_handler

// ====================================================================
// CORE IRQ ROUTER
// ====================================================================
irq_handler_el1:
    // 1. Save caller-saved registers to the stack.
    // Unlike a context switch, an interrupt happens unexpectedly, so we must 
    // protect all temporary working registers!
    sub     sp, sp, #128
    stp     x0, x1, [sp, #0]
    stp     x2, x3, [sp, #16]
    stp     x4, x5, [sp, #32]
    stp     x6, x7, [sp, #48]
    stp     x8, x9, [sp, #64]
    stp     x10, x11, [sp, #80]
    stp     x12, x13, [sp, #96]
    str     x30, [sp, #112]         // Save Link Register

    // 2. Call the C-code interrupt service routine we wrote on Day 9
    bl      handle_timer_irq

    // 3. Restore all registers exactly how we found them
    ldp     x0, x1, [sp, #0]
    ldp     x2, x3, [sp, #16]
    ldp     x4, x5, [sp, #32]
    ldp     x6, x7, [sp, #48]
    ldp     x8, x9, [sp, #64]
    ldp     x10, x11, [sp, #80]
    ldp     x12, x13, [sp, #96]
    ldr     x30, [sp, #112]
    add     sp, sp, #128

    // 4. Exception Return: Restores the saved program counter and drops 
    // the execution right back to where it was interrupted!
    eret

default_handler:
    b       default_handler         // Catch-all loop for unexpected errors

.global ret_from_fork
ret_from_fork:
    // We just returned from cpu_switch_to.
    // The stack pointer points to the 128-byte interrupt frame.
    // We restore caller-saved registers and eret!
    ldp     x0, x1, [sp, #0]
    ldp     x2, x3, [sp, #16]
    ldp     x4, x5, [sp, #32]
    ldp     x6, x7, [sp, #48]
    ldp     x8, x9, [sp, #64]
    ldp     x10, x11, [sp, #80]
    ldp     x12, x13, [sp, #96]
    ldr     x30, [sp, #112]
    add     sp, sp, #128
    eret