#ifndef __X86_H__

#define __X86_H__

#include "types.h"
#include "idt.h"
#include "sched.h"

void outb(uint32_t port, char value);

char inb(uint32_t port);

void lidt(idtr_t idtr);

void sti();

void load_cr3(uint32_t cr3);

void load_state(task_t *task);

#endif
