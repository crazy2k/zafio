#ifndef __IO_H__

#define __IO_H__

#include "../inc/memlayout.h"

void scroll_down();

void clline(void *pos);

void kcls();

void copychar(void *pos, char chr);

void kputc(char chr);

void kputs(char *str);

void kputui32(uint32_t n);

void kputud(unsigned long int num);
void kputd(long int num);

#endif
