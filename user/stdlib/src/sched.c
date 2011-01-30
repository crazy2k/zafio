#include <syscalls.h>

int ps(int mode, char *buf, int buf_size) {
    return syscall(SYSCALLS_NUM_PS, mode, (uint32_t)buf, buf_size, 0, 0);
}
