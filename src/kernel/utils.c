#include "../inc/types.h"

void *memcpy(void *dest, const void *src, size_t n) {
    char *pdest = (char *) dest;
    char *psrc = (char *) src;

    int i;
    for (i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset (void * ptr, int value, size_t num) {

    return ptr;
}
