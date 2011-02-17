#include <syscalls.h>
#include <types.h>

int ps(uint32_t mode, char *buf, uint32_t bufsize) {
    return syscall(SYSCALLS_NUM_PS, mode, (uint32_t)buf, bufsize, 0, 0);
}

int ls(uint32_t mode, char *buf, uint32_t bufsize) {
    return syscall(SYSCALLS_NUM_LS, mode, (uint32_t)buf, bufsize, 0, 0);
}

int run(char *progname) {
    return syscall(SYSCALLS_NUM_RUN, (uint32_t)progname, 0, 0, 0, 0);
}

int nice(uint32_t pid, uint32_t value) {
    return syscall(SYSCALLS_NUM_NICE, pid, value, 0, 0, 0);
}

int waitpid(uint32_t pid) {
    return syscall(SYSCALLS_NUM_WAITPID, pid, 0, 0, 0, 0);
}

int kill(uint32_t pid) {
    return syscall(SYSCALLS_NUM_KILL, pid, 0, 0, 0, 0);
}
