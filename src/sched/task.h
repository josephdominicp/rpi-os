#ifndef TASK_H
#define TASK_H

#include "types.h"

/* Task states */
typedef enum {
    TASK_READY = 0,
    TASK_RUNNING,
    TASK_BLOCKED
} task_state_t;

/* Task Control Block (TCB) */
typedef struct task {
    uint32_t    *stack_ptr;     /* Saved stack pointer */
    uint32_t    stack_base;     /* Stack base address */
    task_state_t state;         /* Current task state */
    uint32_t    pid;            /* Task ID */
} task_t;

/* Initialize task subsystem */
void task_init(void);

/* Create a new task */
task_t *task_create(void (*entry)(void), uint32_t *stack, uint32_t pid);

/* Get currently running task */
task_t *task_current(void);

#endif /* TASK_H */