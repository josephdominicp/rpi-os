#ifndef _TASK_H
#define _TASK_H

#include <stdint.h>

// Task State Definitions
#define TASK_RUNNING    0
#define TASK_READY      1
#define TASK_ZOMBIE     2

// --- STEP 1: Define the CPU Register Storage Context ---
// This structure holds the exact CPU snapshot when a task is paused.
struct cpu_context {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;  // Frame Pointer (x29)
    uint64_t lr;  // Link Register (x30) - Where the task resumes executing
    uint64_t sp;  // Stack Pointer - This task's private memory stack line
};

// --- STEP 2: Define the Task Control Block (TCB) ---
// This tracking structure represents a unique thread of execution.
struct task_struct {
    struct cpu_context cpu_context; // Saved CPU state for this task
    int64_t id;                     // Unique Process/Task ID (PID)
    int64_t state;                  // Current state (RUNNING, READY, ZOMBIE)
    int64_t priority;               // Scheduler priority metric
    int64_t counter;                // Time slice remaining (Ticks left to run)
};
// External tracking parameters managed by task.c
extern int64_t nr_tasks;
extern struct task_struct* current_task;

// Lifecycle management prototypes
void task_init(void);
int64_t task_create(void (*entry_point)(void), int64_t priority);
void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

void schedule(void);
void timer_tick(void);

#endif