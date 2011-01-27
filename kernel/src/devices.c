#include "../inc/devices.h"
#include "../inc/x86.h"
#include "../inc/utils.h"
#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/debug.h"
#include "../inc/syscalls.h"

static void dev_terminal_callback();
static void terminal_move(dev_terminal_t* terminal, int times);

static void dev_terminal_proc_keys(int keyb_dev, int term_dev);

void dev_awake_task(dev_device_t *dev) {
    dev->waiting_task->io_wait = FALSE;
    dev->waiting_task = NULL;
}

/*
 * Pantalla
 */

dev_screen_t screen = {
    .read = NULL,
    .write = dev_screen_write,
    .waiting_task = NULL,
};

int dev_screen_write(int to, char *buf, int bufsize) {
    kputs(buf);
}


/*
 * Teclado
 */


dev_keyboard_t keyboard = {
    .read = dev_keyboard_read,
    .write = NULL,
    .waiting_task = NULL,

    .buffer = { NULL },
    .idx = 0,
    .waiting_kernel_func = dev_terminal_callback,
};

void keyboard_isr(uint32_t index, uint32_t error_code,
    task_state_t *st) {

    dev_keyboard_t *keyboard = (dev_keyboard_t *)devs[DEV_KEYBOARD_NUM];

    if (keyboard->idx >= DEV_KEYBOARD_BUF_LENGTH) {
        kpanic("Buffer de teclado lleno");
    }

    keyboard->buffer[keyboard->idx++] = inb(0x60);

    // Avisar a un subsistema del kernel o despertar a una tarea en espera
    if (keyboard->waiting_kernel_func) {
        keyboard->waiting_kernel_func();
    }
    else if (keyboard->waiting_task) {
        dev_awake_task((dev_device_t *)keyboard);
    }
}

int dev_keyboard_read(int from, char *buf, int bufsize) {

    //if (current_task() != get_terminal_control())
    //    return -1;

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
    .read = &dev_terminal_read,
    .write = NULL,
    .waiting_task = NULL,

    .buffer = { NULL },
    .start = 0,
    .end = 0
};

// Instancias de dispositivos
dev_device_t *devs[DEV_MAX] = {
    [DEV_KEYBOARD_NUM] = (dev_device_t *)&keyboard,
    [DEV_TERMINAL_NUM] = (dev_device_t *)&terminal,
};

static void dev_terminal_callback() {
    dev_terminal_proc_keys(DEV_KEYBOARD_NUM, DEV_TERMINAL_NUM);
}

void dev_terminal_proc_keys(int keyb_dev, int term_dev) {

    dev_terminal_t *terminal = (dev_terminal_t *)devs[term_dev];

    static char buff[DEV_KEYBOARD_BUF_LENGTH] = {0};
    int len = sys_read(keyb_dev, buff, sizeof(buff)); 
    void *cur_pos = NULL;

    for (int i = 0; i < len; i++) {
        char chr = buff[i];

        switch (chr) {
            //carriage return:
            case 13:
                kputc('\n');
                terminal->buffer[terminal->end] = '\n';
                terminal_move(terminal,1);
            break;
            //caracteres imprimibles
            case 32 ... 126:
                kputc(chr);
                terminal->buffer[terminal->end] = chr;
                terminal_move(terminal,1);
            break;

            //backspace:
            case 8:
                if (terminal->end != terminal->start) {
                    cur_pos = get_current_pos();

                    set_current_pos(cur_pos - SCREEN_CHAR_SIZE);
                    kputc(' ');
                    set_current_pos(cur_pos - SCREEN_CHAR_SIZE);
                    terminal->end--;
                }
            break;

            //tab:
            case 9:
                kputs("    ");
                terminal_move(terminal, 4);
            break;

            //delete:
            case 127:

            break;
        }
    }

    dev_awake_task((dev_device_t *)terminal);
}

int dev_terminal_read(int from, char *buf, int bufsize) {

    dev_terminal_t *terminal = (dev_terminal_t *)devs[from];
    int new_line = -1;

    do {
        int buff_end = ((terminal->end - terminal->start) % DEV_TERMINAL_BUF_LENGTH) + terminal->start;
        for (int i = buff_end - 1; i >= terminal->start; i--)
            if (terminal->buffer[i % DEV_TERMINAL_BUF_LENGTH] == '\n')
                new_line = i % DEV_TERMINAL_BUF_LENGTH;

        // Si no hay nada en el buffer, bloquear
        if (new_line == -1) {
            current_task()->io_wait = TRUE;
            terminal->waiting_task = current_task();
            switch_tasks();
        }
    } while (new_line == -1);

    current_task()->io_wait = FALSE;

    int result = (new_line + 1 - terminal->start) % DEV_TERMINAL_BUF_LENGTH;
    int buff_end = result + terminal->start;
    for (int i = terminal->start; i < buff_end; i++)
        buf[i] = terminal->buffer[i % DEV_TERMINAL_BUF_LENGTH];

    terminal->start = buff_end % DEV_TERMINAL_BUF_LENGTH;

    return result;
}

void terminal_move(dev_terminal_t* terminal, int times) {
    if (times > 0) {
        times--;
        terminal->end = ((terminal->end + times) % DEV_TERMINAL_BUF_LENGTH) + 1;

        if (terminal->start == terminal->end)
            terminal->start++;
         
    }/* else if (times < 0) {*/
        /*if (terminal->end != terminal->start) {*/
            /*terminal->end = ((terminal->end + times) % DEV_TERMINAL_BUF_LENGTH) + 1;*/
        /*}*/
    /*}*/
}
