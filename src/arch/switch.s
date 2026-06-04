.section ".text"
.global cpu_switch_to

// void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
// Argument mapping via ARM64 ABI:
//   x0 = Pointer to 'prev' task_struct (Outgoing)
//   x1 = Pointer to 'next' task_struct (Incoming)
//
// Note: Since 'struct cpu_context' is the first member inside 'struct task_struct',
// x0 and x1 point directly to the start of the saved register blocks.

cpu_switch_to:
    // ----------------------------------------------------------------
    // 1. SAVE OUTGOING CPU STATE (prev)
    // ----------------------------------------------------------------
    // stp (Store Pair) copies two 64-bit registers into memory sequentially
    stp     x19, x20, [x0, #0]
    stp     x21, x22, [x0, #16]
    stp     x23, x24, [x0, #32]
    stp     x25, x26, [x0, #48]
    stp     x27, x28, [x0, #64]
    stp     x29, x30, [x0, #80]   // x29 = Frame Pointer (fp), x30 = Link Register (lr)
    
    // Save the current Stack Pointer (sp) via a temporary register
    mov     x9, sp
    str     x9, [x0, #96]

    // ----------------------------------------------------------------
    // 2. LOAD INCOMING CPU STATE (next)
    // ----------------------------------------------------------------
    // ldp (Load Pair) restores two 64-bit registers from memory simultaneously
    ldp     x19, x20, [x1, #0]
    ldp     x21, x22, [x1, #16]
    ldp     x23, x24, [x1, #32]
    ldp     x25, x26, [x1, #48]
    ldp     x27, x28, [x1, #64]
    ldp     x29, x30, [x1, #80]   // Restores new fp and lr execution path
    
    // Restore the incoming Stack Pointer
    ldr     x9, [x1, #96]
    mov     sp, x9

    // ----------------------------------------------------------------
    // 3. SWITCH EXECUTION PATH
    // ----------------------------------------------------------------
    // 'ret' jumps to the address currently loaded inside x30 (lr).
    // Because we just loaded the incoming task's 'lr' into x30, this instruction
    // will cause the CPU to warp directly into the next task's execution path!
    ret