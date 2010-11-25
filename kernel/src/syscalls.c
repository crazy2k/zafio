#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"

void sys_exit(task_t *task) {
    remove_task(task);
    // TODO: La tarea nunca va a volver a ejecutarse porque no estara mas en
    // la lista de tareas. Sin embargo, liberar sus recursos aca es un
    // problema porque switch_tasks() precisa el task_t de la tarea actual.
    // Una solucion posible es dejar como pendiente eliminar los recursos para
    // cuando ocurra la siguiente interrupcion del timer.
    switch_tasks();
}

void sys_puts(char *str) {
    kputs(str);
}
