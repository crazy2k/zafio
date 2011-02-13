#ifndef __IO_H__

#define __IO_H__

#define KEYBOARD 0
#define SCREEN 1
#define TERMINAL 2

int read_line(char* str, int buff_size);
void write_line(char* str);

int read(int dev, char* buff, int buff_size);
int write(int dev, char *buf, int buf_size);

int devreq(int devnum);
int devrel(int devnum);

#endif
