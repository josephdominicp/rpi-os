    .section .text
    .global context_switch

/*
 * void context_switch(uint32_t **old_sp, uint32_t *new_sp)
 *
 * r0 = &old_task->stack_ptr
 * r1 =  new_task->stack_ptr
 */

context_switch:
    /* Save callee-saved registers */
    push {r4-r11, lr}

    /* Save current stack pointer into old task */
    str sp, [r0]

    /* Switch to new task stack */
    mov sp, r1

    /* Restore callee-saved registers of new task */
    pop {r4-r11, lr}

    /* Return to new task */
    bx lr