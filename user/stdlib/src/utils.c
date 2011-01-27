#include <io.h>

int strlen(char* str) {
    int result = 0;
    while(*str) str++;
    return result;
}

void *memcpy(void *dest, const void *src, int n) {
    char *bdest = (char *) dest;
    char *bsrc = (char *) src;

    for (int i = 0; i < n; i++) {
        bdest[i] = bsrc[i];
    }

    return dest;
}


int strcmp(char * src, char * dst) {
    int ret = 0 ;

    while(!(ret = (unsigned int)*src - (unsigned int)*dst) && *dst) {
        src++;
        dst++;
    }

    if (ret < 0)
        ret = -1 ;
    else if (ret > 0)
        ret = 1 ;

    return ret;
}
