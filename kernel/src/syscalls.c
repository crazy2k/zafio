#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"

void sys_exit(task_t *task) {
    // TODO: Completar
}

void sys_puts(char *str) {
    kputs(str);
}
