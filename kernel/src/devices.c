#include "../inc/devices.h"

/*
 * Teclado
 */

dev_keyboard_t keyboard = {
    .buffer = NULL,
    .idx = 0
};

void keyboard_isr(uint32_t index, uint32_t error_code,
    task_state_t *st) {

    waited_device_t *keyboard_wdev = &wdevs[DEV_KEYBOARD_NUM];
    dev_keyboard_t *keyboard = (dev_keyboard_t *)keyboard_wdev->device;


    if (keyboard->idx >= DEV_KEYBOARD_BUF_LENGTH) {
        kpanic("Buffer de teclado lleno");
    }

    keyboard->buffer[keyboard->idx++] = inb(0x60);

    if (keyboard_wdev->task) {
        keyboard_wdev->task->io_wait = FALSE;
        keyboard_wdev->task = NULL;
    }
}

// Dispositivos con sus tareas en espera
waited_device_t wdevs[DEV_WAITING_MAX] = {
    [0] = { NULL, &keyboard },
};

