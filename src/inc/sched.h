#ifndef __SCHED_H__

#define __SCHED_H__

#include "types.h"

typedef struct {
    uint16_t prev;  // Previous Task Link
    uint16_t :16;
    uint32_t esp0;  // Stack pointer (nivel 0)
    uint16_t ss0;   // Stack segment (nivel 0)
    uint16_t :16;
    uint32_t esp1;  // Stack pointer (nivel 1)
    uint16_t ss1;   // Stack segment (nivel 1)
    uint16_t :16;
    uint32_t esp2;  // Stack pointer (nivel 2)
    uint16_t ss2;   // Stack segment (nivel 2)
    uint16_t :16;
    uint32_t cr3;   // Page directory base register
    uint32_t eip;
    uint32_t eflags;

    // Registros de proposito general
    uint32_t eax;   
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    // Registros de segmento
    uint16_t es;
    uint16_t :16;
    uint16_t cs;
    uint16_t :16;
    uint16_t ss;
    uint16_t :16;
    uint16_t ds;
    uint16_t :16;
    uint16_t fs;
    uint16_t :16;
    uint16_t gs;
    uint16_t :16;
    uint16_t ldt;
    uint16_t :16;

    uint16_t t:1;   // Debug trap flag
    uint16_t :15;
    uint16_t io;    // I/O map base address
} tss_t;


typedef struct {
    // Registros a ser cargados usando ``popa``
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t :32;   // Espaciado: ``popa`` se saltea estos bits del stack
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // Registros de segmento
    uint16_t es;
    uint16_t :16;
    uint16_t ds;
    uint16_t :16;

    // Estado a ser cargado por ``iret``
    uint32_t eip;
    uint16_t cs;
    uint16_t :16;
    uint32_t eflags;

    // Datos del stack que ``iret`` solo carga si hay cambio de nivel
    uint32_t esp;
    uint16_t ss;
    uint16_t :16;
} task_state_t;

typedef struct task_t task_t;

struct task_t {
    void *pd;               // Direccion fisica del directorio de paginas
    void *kernel_stack;
    void *kernel_stack_top;
    task_t *next;
    task_t *prev;
};

extern uint64_t gdt[];

void sched_init();

void setup_tss(uint32_t kernel_stack);

task_t *create_task(uint32_t pd[], int level, void *entry_point,
    void *stack_pointer);

void switch_tasks();

#endif
