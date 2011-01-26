#include "../inc/sched.h"
#include "../inc/syscalls.h"
#include "../inc/io.h"
#include "../inc/idt.h"
#include "../inc/utils.h"
#include "../inc/devices.h"

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

    waited_device_t *keyboard_wdev = &wdevs[DEV_KEYBOARD_NUM];
    dev_keyboard_t *keyboard = (dev_keyboard_t *)keyboard_wdev->device;

    // Si no hay nada en el buffer, bloquear
    if (keyboard->idx <= 0) {
        current_task()->io_wait = TRUE;

        keyboard_wdev->task = current_task();

        switch_tasks();
    }

    current_task()->io_wait = FALSE;

    int i, j;
    for (i = 0, j = 0; i <= keyboard->idx; i++) {
        if (keyboard->buffer[i] & 0x80)
            continue;
        buf[j++] = sc2ascii(keyboard->buffer[i]);
    }
    keyboard->idx = 0;

    return j - 1;
}

void sys_puts(char *str) {
    kputs(str);
}
