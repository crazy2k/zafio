#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"
#include "../inc/idt.h"
#include "../inc/utils.h"
#include "../inc/devices.h"
#include "../inc/vmm.h"
#include "../inc/debug.h"

void sys_exit(task_t *task) {
    //if (get_terminal_control() == current_task())
    //    set_terminal_control(get_past_terminal_control());

    // Quitamos la tarea de la lista de tareas
    remove_task(task);
    // TODO: La tarea nunca va a volver a ejecutarse porque no estara mas en
    // la lista de tareas. Sin embargo, liberar sus recursos aca es un
    // problema porque switch_tasks() precisa el task_t de la tarea actual.
    // Una solucion posible es dejar como pendiente eliminar los recursos para
    // cuando ocurra la siguiente interrupcion del timer.
    put_zombie(task);

    if (task->waited != NULL)
        task->parent->waiting = FALSE;

    switch_tasks();
}

int sys_read(uint32_t from, char *buf, uint32_t bufsize) {
    if (devs[from]->access == current_task())
        return devs[from]->read(from, buf, bufsize);
    else
        return -1;
}

int sys_write(uint32_t to, char *buf, uint32_t bufsize) {
    if (devs[to]->access == current_task())
        return devs[to]->write(to, buf, bufsize);
    else
        return -1;
}

#define SYSCALLS_SEP ("    ")

int sys_ls(uint32_t mode, char *buf, uint32_t bufsize) {
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

#define PS_COL_SIZE 10
static char *task_state(task_t *task) {
    if (task->waiting)
        return "WAITING";
    if (task == current_task())
        return "RUNNING";

    return "READY";
}
int sys_ps(uint32_t mode, char *buf, uint32_t bufsize) {
    int end = 0;
    if (mode == 0) {
        task_t *first, *curr;
        first = curr = current_task();

        char *headers[] = { "PROGNAME", "PID", "QUANTUM", "TICKS", "STATE" };
        strcolumns(buf, bufsize, PS_COL_SIZE, headers,
            sizeof(headers)/sizeof(char *));

        do {
            char row[SCREEN_COLS + 1] = { 0 };
            strconcat(row, SCREEN_COLS + 1, "\n");

            char pid[PS_COL_SIZE] = { 0 };
            char quantum[PS_COL_SIZE] = { 0 };
            char ticks[PS_COL_SIZE] = { 0 };
            char *cols[] = {
                    curr->prog->name,
                    uitoa(curr->pid, pid),
                    uitoa(curr->quantum, quantum),
                    uitoa(curr->ticks, ticks),
                    task_state(curr),
                };

            strcolumns(row + 1, SCREEN_COLS, PS_COL_SIZE, cols,
                sizeof(cols)/sizeof(char *));

            end = strconcat(buf, bufsize, row);

            curr = curr->next;
        } while(curr != first);
    }
    return end;
}

int sys_run(char *progname) {
    program_t *prog = get_prog_by_name(progname);

    uint32_t *pd = clone_pd(kernel_pd);
    task_t *new_task = create_task(pd, prog);
    add_task(new_task);

    new_task->parent = current_task();

    return new_task->pid;
}

int sys_nice(uint32_t pid, uint32_t value) {
    task_t *task = get_task_by_pid(pid);
    if (task == NULL)
        return -1;

    task->quantum = value;
    return 0;
}

int sys_devreq(uint32_t devnum) {
    if ((devnum < 0) || (devnum >= DEV_MAX))
        return -1;

    if (devs[devnum]->access == NULL) {
        devs[devnum]->access = current_task();
        return 0;
    }
    else
        return -1;
}

int sys_devrel(uint32_t devnum) {
    if ((devnum < 0) || (devnum >= DEV_MAX))
        return -1;

    if (devs[devnum]->access == current_task()) {
        devs[devnum]->access = NULL;
        return 0;
    }
    else
        return -1;
}


int sys_waitpid(uint32_t pid) {
    current_task()->waiting = TRUE;

    task_t *task = get_task_by_pid(pid);
    if (task == NULL)
        return -1;

    task->waited = TRUE;

    switch_tasks();

    return 0;
}
