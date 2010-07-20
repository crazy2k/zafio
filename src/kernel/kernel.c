#include "../inc/mmu.h"
#include "../inc/vmmu.h"

void cmain() {

    unsigned char *video_mem = (unsigned char *) 0xB8000;
    int i;
    for (i = 0; i < 80*25*2; i++) {
        video_mem[i] = 0;
    }

   reserve_page((page*) (0xC200200) );

}
