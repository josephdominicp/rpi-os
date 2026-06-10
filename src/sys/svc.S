    .section .text
    .global svc_handler

/*
 * SVC handler
 * Called when user executes SVC instruction
 */

svc_handler:
    // Save registers x0-x8 and lr (x30)
    sub     sp, sp, #96
    stp     x0, x1, [sp, #0]
    stp     x2, x3, [sp, #16]
    stp     x4, x5, [sp, #32]
    stp     x6, x7, [sp, #48]
    str     x8, [sp, #64]
    str     x30, [sp, #80]

    // In AArch64, the syscall number is passed in x8
    // We pass it to syscall_dispatch as the first argument (x0)
    mov     x0, x8
    bl      syscall_dispatch

    // Restore registers
    ldp     x0, x1, [sp, #0]
    ldp     x2, x3, [sp, #16]
    ldp     x4, x5, [sp, #32]
    ldp     x6, x7, [sp, #48]
    ldr     x8, [sp, #64]
    ldr     x30, [sp, #80]
    add     sp, sp, #96

    // Return from exception
    eret