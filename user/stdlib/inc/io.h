#ifndef __IO_H__

#define __IO_H__

#include <types.h>

#define KEYBOARD 0
#define SCREEN 1
#define TERMINAL 2

int read_line(char* str, int buff_size);
void write_line(char* str);

int read(uint32_t from, char *buf, uint32_t bufsize);
int write(uint32_t to, char *buf, uint32_t bufsize);
int devreq(uint32_t devnum);
int devrel(uint32_t devnum);

#endif
