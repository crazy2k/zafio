#include "../inc/sched.h"

#ifndef __DEVICES_H__

#define __DEVICES_H__


// Cantidad maxima de dispositivos
#define DEV_MAX 3


// Numero de dispositivos
#define DEV_KEYBOARD_NUM 0
#define DEV_SCREEN_NUM 1
#define DEV_TERMINAL_NUM 2

#define INCLUDE_DEVICE() \
    int (*read)(int from, char *buf, int bufsize); \
    int (*write)(int to, char *buf, int bufsize); \
    task_t *waiting_task;



typedef struct {
    INCLUDE_DEVICE();
} dev_device_t;


typedef struct {
    INCLUDE_DEVICE();
} dev_screen_t;


// Longitud del buffer para el teclado
#define DEV_KEYBOARD_BUF_LENGTH 1024

typedef struct {
    INCLUDE_DEVICE();

    char buffer[DEV_KEYBOARD_BUF_LENGTH];
    int idx; // Indice del proximo caracter a ingresar
    void (*waiting_kernel_func)(); // Subsistema del kernel esperando
} dev_keyboard_t;


// Longitud del buffer para la terminal
#define DEV_TERMINAL_BUF_LENGTH 1024
typedef struct {
    INCLUDE_DEVICE();

    char buffer[DEV_TERMINAL_BUF_LENGTH];
    int start;
    int len;
} dev_terminal_t;


extern dev_device_t *devs[DEV_MAX];

void keyboard_isr(uint32_t index, uint32_t error_code, task_state_t *st);

int dev_keyboard_read(int from, char *buf, int bufsize);

int dev_terminal_read(int from, char *buf, int bufsize);

int dev_screen_write(int to, char *buf, int bufsize);

#endif
