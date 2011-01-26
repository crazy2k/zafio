#include <syscalls.h>

uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx,
    uint32_t esi, uint32_t edi) {
    uint32_t ret;
    asm volatile("int %1\n"
        : "=a" (ret)
        : "i" (0x80),
        "a" (eax),
        "b" (ebx),
        "c" (ecx),
        "d" (edx),
        "S" (esi),
        "D" (edi)
        : "cc", "memory");

    return ret;
}
