#include "task.h"

#define MAX_TASKS   16
#define STACK_SIZE  4096  // Each task gets an isolated 4KB private stack

// Allocate task data structures and memory stacks statically
struct task_struct task_pool[MAX_TASKS];
static uint8_t task_stacks[MAX_TASKS][STACK_SIZE] __attribute__((aligned(16)));

int64_t nr_tasks = 0;
struct task_struct* current_task; // Global tracking pointer to the active thread

// --- INITIALIZE TASK 0 (The Boot Thread) ---
// This registers the main loop currently executing boot.S/kernel_main as Task 0
// so that the scheduler has an outgoing structure to save states into later.
void task_init(void) {
    task_pool[0].id = 0;
    task_pool[0].state = TASK_RUNNING;
    task_pool[0].priority = 1;
    task_pool[0].counter = 10;
    
    current_task = &task_pool[0];
    nr_tasks = 1;
}

// --- CREATE A NEW THREAD TASK ---
int64_t task_create(void (*entry_point)(void), int64_t priority) {
    if (nr_tasks >= MAX_TASKS) {
        return -1; // Out of available processing slots
    }

    int64_t id = nr_tasks++;
    struct task_struct* p = &task_pool[id];

    p->id = id;
    p->state = TASK_READY;
    p->priority = priority;
    p->counter = priority * 10; // Give a raw processing time allocation

    // Clear general callee-saved registers to ensure a clean slate
    p->cpu_context.x19 = 0; p->cpu_context.x20 = 0; p->cpu_context.x21 = 0;
    p->cpu_context.x22 = 0; p->cpu_context.x23 = 0; p->cpu_context.x24 = 0;
    p->cpu_context.x25 = 0; p->cpu_context.x26 = 0; p->cpu_context.x27 = 0;
    p->cpu_context.x28 = 0;
    p->cpu_context.fp = 0;  // Reset Frame Pointer

    // ====================================================================
    // CORE CRITICAL SURGERY: Faking the Execution Context
    // ====================================================================

    extern void ret_from_fork(void);

    // 1. Set the Link Register (lr) to ret_from_fork. 
    // When cpu_switch_to finishes loading this context and runs 'ret', 
    // the CPU will jump directly to ret_from_fork.
    p->cpu_context.lr = (uint64_t)ret_from_fork;

    // 2. Set ELR_EL1 and SPSR_EL1 so ret_from_fork's eret returns to entry_point
    // with interrupts enabled (EL1h mode = 0x5).
    p->cpu_context.elr_el1 = (uint64_t)entry_point;
    p->cpu_context.spsr_el1 = 0x5;

    // 3. Set the Stack Pointer (sp) to the top of our private block minus
    // the 128-byte space we reserved for the initial faked exception frame.
    p->cpu_context.sp = (uint64_t)&task_stacks[id][STACK_SIZE] - 128;

    return id;
}