#include "../inc/mmu.h"

void cmain() {

    unsigned char *video_mem = (unsigned char *) 0xB8000;
    int i;
    for (i = 0; i < 80*25*2; i++) {
        video_mem[i] = 0;
    }


}
