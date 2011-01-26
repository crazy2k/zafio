#include <types.h>
#include <syscalls.h>

char buf[1024] = { 0 };

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

