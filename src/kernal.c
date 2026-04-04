#include "uart.h"
#include "types.h"

/* Assembly function */
extern void enter_user_mode(uint32_t user_sp, void (*user_pc)(void));

/* User task entry */
extern void user_task(void);

/* Simple user stack */
#define USER_STACK_SIZE 1024
static uint8_t user_stack[USER_STACK_SIZE];

void kernel_main(void)
{
    uart_init();
    uart_puts("Kernel started (SVC mode)\n");

    uint32_t user_sp = (uint32_t)(user_stack + USER_STACK_SIZE);

    uart_puts("Switching to user mode...\n");

    enter_user_mode(user_sp, user_task);

    /* Kernel should never return here */
    while (1);
}