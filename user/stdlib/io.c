#include "../inc/io.h"

#define READ_BUFF_LEN 1024

int read_line(char* str, int buff_size) {
    static char* buff[READ_BUFF_LEN] = {0};

}

void write_line(char* str) {
    write(TERM_OUTPUT, str);
    write(TERM_OUTPUT, "\n");
}

int read(int dev, char* buff, int buff_size) {
    
}
    
void write(int dev, char* str) {
    
}
