    .section .text
    .global svc_handler

/*
 * SVC handler
 * Called when user executes SVC instruction
 */

svc_handler:
    /* Save context (minimal) */
    push {r0-r3, r12, lr}

    /*
     * Extract syscall number
     * SVC instruction is at (LR - 4)
     */
    ldr r0, [lr, #-4]
    bic r0, r0, #0xFF000000   /* isolate immediate value */

    /* Call C syscall dispatcher */
    bl syscall_dispatch

    /* Restore context */
    pop {r0-r3, r12, lr}

    /* Return from exception */
    subs pc, lr, #4