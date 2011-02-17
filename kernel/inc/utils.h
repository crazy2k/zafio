#ifndef __UTILS_H__

#define __UTILS_H__

#include "types.h"

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void * ptr, int value, size_t num);

long max(long var1, long var2);

long min(long var1, long var2);

void kpanic(char *msg);

char sc2ascii(unsigned char sc);

int strcmp(char * src, char * dst);

int strconcat(char *dest, int dest_size, char *src);

int strlen(char* str);

char *align(unsigned int n, char *out, char *in, int outsize);

char *strcolumns(char *dest, int dest_size, int al, char **srcarr, int src_len);

char *uitoa(unsigned int n, char *str);

#endif
