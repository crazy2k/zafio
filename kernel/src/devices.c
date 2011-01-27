#include "../inc/devices.h"
#include "../inc/x86.h"
#include "../inc/utils.h"
#include "../inc/vmm.h"
#include "../inc/io.h"

/*
 * Teclado
 */

dev_keyboard_t keyboard = {
    .read = dev_keyboard_read,
    .write = NULL,
    .waiting_task = NULL,

    .buffer = { NULL },
    .idx = 0
};

void keyboard_isr(uint32_t index, uint32_t error_code,
    task_state_t *st) {

    dev_keyboard_t *keyboard = (dev_keyboard_t *)devs[DEV_KEYBOARD_NUM];

    if (keyboard->idx >= DEV_KEYBOARD_BUF_LENGTH) {
        kpanic("Buffer de teclado lleno");
    }

    keyboard->buffer[keyboard->idx++] = inb(0x60);

    if (keyboard->waiting_task) {
        keyboard->waiting_task->io_wait = FALSE;
        keyboard->waiting_task = NULL;
    }
}

int dev_keyboard_read(int from, char *buf, int bufsize) {

    if (current_task() != get_terminal_control())
        return -1;

    dev_keyboard_t *keyboard = (dev_keyboard_t *)devs[from];

    // Si no hay nada en el buffer, bloquear
    if (keyboard->idx <= 0) {
        current_task()->io_wait = TRUE;

        keyboard->waiting_task = current_task();

        switch_tasks();
    }

    current_task()->io_wait = FALSE;

    int i, j;
    for (i = 0, j = 0; i <= keyboard->idx; i++) {
        if (keyboard->buffer[i] & 0x80)
            continue;
        buf[j++] = sc2ascii((unsigned char)keyboard->buffer[i]);
    }
    keyboard->idx = 0;

    return j - 1;
}

/*
 * Terminal
 */


dev_terminal_t terminal = {
    .buffer = { NULL },
    .start = 0,
    .end = 0
};

// Instancias de dispositivos
dev_device_t *devs[DEV_MAX] = {
    [DEV_KEYBOARD_NUM] = (dev_device_t *)&keyboard,
    [DEV_TERMINAL_NUM] = (dev_device_t *)&terminal,
};

