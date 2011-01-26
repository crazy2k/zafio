#ifndef __UTILS_H__

#define __UTILS_H__

#include "types.h"

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void * ptr, int value, size_t num);

long max(long var1, long var2);

long min(long var1, long var2);

void kpanic(char *msg);

char sc2ascii(char sc);

int strcmp(char * src, char * dst);

#endif
