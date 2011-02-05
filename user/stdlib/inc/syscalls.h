#ifndef __SYSCALLS_H__

#define __SYSCALLS_H__

#include <types.h>


#define SYSCALLS_NUM_EXIT 1
#define SYSCALLS_NUM_READ 3
#define SYSCALLS_NUM_WRITE 4
#define SYSCALLS_NUM_LS 5
#define SYSCALLS_NUM_PS 6
#define SYSCALLS_NUM_RUN 7
#define SYSCALLS_NUM_TERMREQ 8
#define SYSCALLS_NUM_NICE 9


uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx,
    uint32_t esi, uint32_t edi);

#endif
