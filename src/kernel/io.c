#include "../inc/memlayout.h"

#define SCREEN_ROWS 25
#define SCREEN_COLS 80
#define SCREEN_CHAR_SIZE 2
#define SCREEN_BEGIN VIDEO_MEMORY
#define SCREEN_END VIDEO_MEMORY+80*25*2

int current_pos = 0;

void scroll_down() {
    //memcopy(SCREEN_BEGIN + COLS*SCREEN_CHAR_SIZE, SCREEN_END, SCREEN_BEGIN);
}

void putchar(int pos, char chr) {
    char *video_mem = (char *) VIDEO_MEMORY;
    video_mem[pos*2] = 0x0F;
    video_mem[pos*2 + 1] = chr;
}

void cls() {
    int pos;
    for (pos = 0; pos < 80*25*2; pos++)
        putchar(pos, 0);
}

void print(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (current_pos >= 80*24)
            scroll_down();
        putchar(current_pos, str[i]);
        current_pos++;
    }
    current_pos += 80;
}

