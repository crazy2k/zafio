#ifndef __IO_H__

#define __IO_H__

#include "../inc/memlayout.h"

void scroll_down();

void putchr(void *pos, char chr);

void clline(void *start);

void cls();

void print(char *str);

#endif
