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
    
    int result = (newline - buff_beg + 1) % READ_BUFF_LEN;
    
    if (newline >= buff_beg)
        memcpy(str, &read_buff[buff_beg], newline - buff_beg + 1);
    else {
        memcpy(str, &read_buff[buff_beg], READ_BUFF_LEN - buff_beg);
        memcpy(str, read_buff, newline + 1);
    }
    
    buff_beg = (newline + 1) % READ_BUFF_LEN;
    
    return result;
}

void write_line(char* str) {
    write(TERMINAL, str, strlen(str) + 1);
    write(TERMINAL, "\n", 2);
}

int read(uint32_t from, char *buf, uint32_t bufsize) {
    return syscall(SYSCALLS_NUM_READ, from, (uint32_t)buf, bufsize, 0, 0);
}
    
int write(uint32_t to, char *buf, uint32_t bufsize) {
    return syscall(SYSCALLS_NUM_WRITE, to, (uint32_t)buf, bufsize, 0, 0);
}

int devreq(uint32_t devnum) {
    return syscall(SYSCALLS_NUM_DEVREQ, devnum, 0, 0, 0, 0);
}

int devrel(uint32_t devnum) {
    return syscall(SYSCALLS_NUM_DEVREL, devnum, 0, 0, 0, 0);
}
