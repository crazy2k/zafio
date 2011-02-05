#include <syscalls.h>

int ps(int mode, char *buf, int buf_size) {
    return syscall(SYSCALLS_NUM_PS, mode, (uint32_t)buf, buf_size, 0, 0);
}

int ls(int mode, char *buf, int buf_size) {
    return syscall(SYSCALLS_NUM_LS, mode, (uint32_t)buf, buf_size, 0, 0);
}

int run(int mode, char *progname) {
    return syscall(SYSCALLS_NUM_RUN, mode, (uint32_t)progname, 0, 0, 0);
}

int nice(int process, int value) {
    return syscall(SYSCALLS_NUM_NICE, process, value, 0, 0, 0);
}
