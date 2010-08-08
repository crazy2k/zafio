#include "../inc/types.h"
#include "../inc/io.h"

void *memcpy(void *dest, const void *src, size_t n) {
    char *bdest = (char *) dest;
    char *bsrc = (char *) src;

    for (int i = 0; i < n; i++) {
        bdest[i] = bsrc[i];
    }

    return dest;
}

void *memset (void *s, int c, size_t n) {
    char *bs = (char *)s;

    for (int i = 0; i < n; i++)
        bs[i] = (char)c;

    return s;
}

long max(long var1, long var2) {
    return var1 < var2 ? var2 : var1;
}

long min(long var1, long var2) {
    return var1 > var2 ? var2 : var1;
}

void kpanic(char *msg) {
    kputs("PANIC: ");
    kputs(msg);
    kputs("\n");
    __asm__ __volatile__("hlt");
}
