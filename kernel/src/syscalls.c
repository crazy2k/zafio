#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"
#include "../inc/idt.h"
#include "../inc/utils.h"
#include "../inc/devices.h"
#include "../inc/vmm.h"
#include "../inc/debug.h"

void sys_exit(task_t *task) {
    if (get_terminal_control() == current_task())
        set_terminal_control(get_past_terminal_control());

    // Quitamos la tarea de la lista de tareas
    remove_task(task);
    // TODO: La tarea nunca va a volver a ejecutarse porque no estara mas en
    // la lista de tareas. Sin embargo, liberar sus recursos aca es un
    // problema porque switch_tasks() precisa el task_t de la tarea actual.
    // Una solucion posible es dejar como pendiente eliminar los recursos para
    // cuando ocurra la siguiente interrupcion del timer.
    put_zombie(task);

    switch_tasks();
}

int sys_read(int from, char *buf, int bufsize) {
    return devs[from]->read(from, buf, bufsize);
}

int sys_write(int to, char *buf, int bufsize) {
    return devs[to]->write(to, buf, bufsize);
}

#define SYSCALLS_SEP ("    ")

int sys_ls(int mode, char *buf, int bufsize) {
    int end = 0;
    if (mode == 0) {
        buf[0] = '\0';
        for (int i = 0; i < programs_size; i++) {
            strconcat(buf, bufsize, (&programs[i])->name);
            end = strconcat(buf, bufsize, SYSCALLS_SEP);
        }
        buf[end] = '\0';
    }
    return end;
}

int sys_ps(int mode, char *buf, int bufsize) {
    int end = 0;
    if (mode == 0) {
        task_t *first, *curr;
        first = curr = current_task();

        buf[0] = '\0';
        do {
            strconcat(buf, bufsize, curr->prog->name);
            end = strconcat(buf, bufsize, SYSCALLS_SEP);
            
            curr = curr->next;
        } while(curr != first);
        buf[end] = '\0';
    }
    return end;
}

int sys_run(int mode, char *progname) {
    program_t *prog = get_prog_by_name(progname);

    uint32_t *pd = clone_pd(kernel_pd);
    task_t *new_task = create_task(pd, prog);
    add_task(new_task);

    // Foreground
    if (mode == 0) {
        set_terminal_control(new_task);
    }

    return 0;
}

int sys_termreq() {
    if (!get_terminal_control()) {
        set_terminal_control(current_task());
        return 0;
    }
    return -1;
}
