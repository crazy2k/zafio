#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"
#include "../inc/idt.h"
#include "../inc/utils.h"

void sys_exit(task_t *task) {
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
    //current_task()->io_wait = TRUE;
    keyboard_isr_waiting_task = current_task();

    //switch_tasks();

    //current_task()->io_wait = FALSE;

    if (keyboard_isr_buf_idx > 0) {
        int i, j;
        for (i = 0, j = 0; i <= keyboard_isr_buf_idx; i++) {
            //kputui32((uint32_t)keyboard_isr_buf[i]);
            if (keyboard_isr_buf[i] & 0x80)
                continue;
            buf[j++] = sc2ascii(keyboard_isr_buf[i]);
        }
        keyboard_isr_buf_idx = 0;

        return j - 1;
    }
    return 0;
}

void sys_puts(char *str) {
    kputs(str);
}
