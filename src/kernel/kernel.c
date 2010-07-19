#include "../inc/mmu.h"

unsigned long long int segment = GDT_DESC_BASE(0) | GDT_DESC_LIMIT(0xFFFFF) /*| GDT_TYPE(STA_W) | 
       GDT_PRIVILEGE_LEVEL(0) | GDT_APPLICATION_SEGMENT | GDT_32_BITS_SEGMENT |
       GDT_GRANULARITY_LIMIT_4KB | GDT_PRESENT_SEGMENT*/;

void cmain(unsigned int magic_number, void* mbinfo) {

    unsigned char *video_mem = (unsigned char *) 0xB8000;
    int i;
    for (i = 0; i < 80*25*2; i++) {
        video_mem[i] = 0;
    }


}
