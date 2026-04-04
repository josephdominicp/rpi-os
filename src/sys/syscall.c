#include "syscall.h"
#include "uart.h"

/*
 * Syscall dispatcher
 * Called from svc.S
 */
void syscall_dispatch(uint32_t syscall_num)
{
    switch (syscall_num) {

    case SYS_WRITE:
        uart_puts("SYS_WRITE called\n");
        break;

    default:
        uart_puts("Unknown syscall\n");
        break;
    }
}

/*
 * User-space syscall wrapper
 */
void sys_write(const char *str)
{
    asm volatile(
        "mov r1, %0\n"
        "svc #0\n"
        :
        : "r"(str)
        : "r1"
    );
}