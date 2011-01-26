#include <types.h>

char buf[1024] = { 0 };

uint32_t syscall(int num, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    uint32_t ret;
    asm volatile("int %1\n"
        : "=a" (ret)
        : "i" (0x80),
        "a" (num),
        "b" (a1),
        "c" (a2),
        "d" (a3),
        "D" (a4),
        "S" (a5)
        : "cc", "memory");

    return ret;
}

int main() {
    while (1) {
        int n = syscall(3, 0, (uint32_t)buf, 1024, 0, 0);

        buf[n] = '\0';
        if (n > 0) {
            syscall(4, (uint32_t)buf, 0, 0, 0, 0);
        }
    }

    return 0;
}

