#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

/* Syscall numbers */
#define SYS_WRITE   0

/* Dispatcher called from SVC handler */
void syscall_dispatch(uint32_t syscall_num);

/* User-facing syscall wrapper */
void sys_write(const char *str);

#endif /* SYSCALL_H */