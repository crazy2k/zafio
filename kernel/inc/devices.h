#include "../inc/sched.h"

#ifndef __DEVICES_H__

#define __DEVICES_H__


// Cantidad maxima de dispositivos
#define DEV_WAITING_MAX 1


// Numero de dispositivos
#define DEV_KEYBOARD_NUM 0
#define DEV_SCREEN_NUM 1


// Longitud del buffer para el teclado
#define DEV_KEYBOARD_BUF_LENGTH 1024

typedef struct {
    char buffer[DEV_KEYBOARD_BUF_LENGTH];
    int idx; // Indice del proximo caracter a ingresar
} dev_keyboard_t;


typedef struct {
    task_t *task;
    void *device;
} waited_device_t;

extern waited_device_t wdevs[DEV_WAITING_MAX];

void keyboard_isr(uint32_t index, uint32_t error_code, task_state_t *st);

#endif
