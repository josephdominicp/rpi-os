#include "task.h"

// Pull in our global task variables from task.c (Day 8)
extern struct task_struct task_pool[];
extern int64_t nr_tasks;
extern struct task_struct* current_task;

// --- THE CORE SCHEDULER ---
void schedule(void) {
    // 1. If there's only 1 task registered, switching is impossible!
    if (nr_tasks <= 1) return;

    // 2. Start looking for the next task right after the current one
    int64_t next_id = (current_task->id + 1) % nr_tasks;

    // 3. Loop through the pool until we find a task that is READY to run
    while (task_pool[next_id].state != TASK_READY) {
        next_id = (next_id + 1) % nr_tasks;
        
        // Safety check: If we looped all the way back to ourselves, just stay here.
        if (next_id == current_task->id) return;
    }

    // 4. Set up our pointers for the swap
    struct task_struct *prev = current_task;
    struct task_struct *next = &task_pool[next_id];

    // 5. Update their running states
    if (prev->state == TASK_RUNNING) {
        prev->state = TASK_READY;
    }
    next->state = TASK_RUNNING;
    current_task = next; // Update the global pointer

    // 6. Execute the Day 7 assembly register swap engine!
    cpu_switch_to(prev, next);
}

// --- THE TIMER TICK TRIGGER ---
// The hardware timer from Day 9 calls this function exactly every 10ms
void timer_tick(void) {
    // Force a task switch on every single clock tick!
    schedule();
}