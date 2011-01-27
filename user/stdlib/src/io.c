#include <types.h>
#include <io.h>
#include <utils.h>
#include <syscalls.h>

#define READ_BUFF_LEN 1024

int read_line(char* str, int buff_size) {
    static char read_buff[READ_BUFF_LEN] = {0};
    static int buff_beg = 0,
               buff_end = 0;

    int newline = -1, count = 0;

    if ((READ_BUFF_LEN - buff_end) >= buff_size)
        count = read(TERMINAL, &read_buff[buff_end], buff_size);
    else {
        count = read(TERMINAL, &read_buff[buff_end], READ_BUFF_LEN - buff_end);
        count += read(TERMINAL, read_buff, buff_size - count);
    }
    
    if (count == -1)
        return -1;
    
    buff_end = (buff_end + count) % READ_BUFF_LEN;

    for (int i = 0; i < count; i++) {
        if (read_buff[buff_beg + i] == '\n') {
            newline = buff_beg + i;
            break;
        }
    }
    
    int result = (newline - buff_beg) % READ_BUFF_LEN;
    
    if (newline >= buff_beg)
        memcpy(str, &read_buff[buff_beg], newline - buff_beg + 1);
    else {
        memcpy(str, &read_buff[buff_beg], READ_BUFF_LEN - buff_beg);
        memcpy(str, read_line, newline + 1);
    }
    
    buff_beg = (newline + 1) % READ_BUFF_LEN;
    
    return result;
}

void write_line(char* str) {
    write(TERMINAL, str, strlen(str) + 1);
    write(TERMINAL, "\n", 2);
}

int read(int dev, char *buf, int buf_size) {
    return syscall(SYSCALL_READ, dev, (uint32_t)buf, buf_size, 0, 0);
}
    
int write(int dev, char *buf, int buf_size) {
    return syscall(SYSCALL_WRITE, dev, (uint32_t)buf, buf_size, 0, 0);
}
