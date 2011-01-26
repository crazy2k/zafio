#include <types.h>

#define SYSCALL_READ 3
#define SYSCALL_WRITE 4


uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx,
    uint32_t esi, uint32_t edi);
