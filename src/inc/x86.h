#ifndef __X86_H__

#define __X86_H__

#include "types.h"
#include "idt.h"

void outb(uint32_t port, char value);

char inb(uint32_t port);

void lidt(idtr_t idtr);

void sti();

#endif
