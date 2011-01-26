#ifndef __IO_H__

#define __IO_H__

#define TERM_INPUT 0
#define TERM_OUTPUT 1

int read_line(char* str, int buff_size);
void write_line(char* str);

int read(int dev, char* buff, int buff_size);
void write(int dev, char* str);

#endif
