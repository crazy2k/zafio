#include "../inc/io.h"

#define READ_BUFF_LEN 1024

int read_line(char* str, int buff_size) {
    static char* buff[READ_BUFF_LEN] = {0};

}

void write_line(char* str) {
    write(TERM_OUTPUT, str, strlen(str));
    write(TERM_OUTPUT, "\n", 1);
}

int read(int dev, char *buf, int buf_size) {
    return syscall(SYSCALL_READ, dev, (uint32_t)buf, buf_size, 0, 0);
}
    
int write(int dev, char *buf, int buf_size) {
    return syscall(SYSCALL_WRITE, dev, (uint32_t)buf, buf_size, 0, 0);
}
