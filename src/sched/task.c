#include "task.h"

/* Maximum number of tasks */
#define MAX_TASKS 4

/* Task table */
static task_t tasks[MAX_TASKS];

/* Index of current task */
static uint32_t current_task = 0;

/*
 * Initialize task system
 */
void task_init(void)
{
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_BLOCKED;
        tasks[i].pid   = i;
    }
}

/*
 * Create a new task
 * entry : function where task starts
 * stack : top of allocated stack memory
 */
task_t *task_create(void (*entry)(void), uint32_t *stack, uint32_t pid)
{
    task_t *t = &tasks[pid];

    /*
     * ARM stack frame (simplified)
     * ----------------------------
     * r0-r12
     * lr
     * pc   ← entry point
     * cpsr
     */

    stack -= 16;               /* Reserve space for registers */
    stack[15] = (uint32_t)entry;  /* PC */

    t->stack_ptr  = stack;
    t->stack_base = (uint32_t)stack;
    t->state      = TASK_READY;
    t->pid        = pid;

    return t;
}

/*
 * Get currently running task
 */
task_t *task_current(void)
{
    return &tasks[current_task];
}