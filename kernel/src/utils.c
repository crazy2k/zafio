#include "../inc/types.h"
#include "../inc/io.h"

int strcmp(char * src, char * dst) {
    int ret = 0 ;

    while(!(ret = (unsigned int)*src - (unsigned int)*dst) && *dst) {
        ++src;
        ++dst;
    }

    if (ret < 0)
        ret = -1;
    else if (ret > 0)
        ret = 1;

    return ret;
}

int strconcat(char *dest, int dest_size, char *src) {
    int i;
    for (i = 0; dest[i] != '\0'; i++)
        ;

    int j;
    for (j = 0; src[j] != '\0'; j++) {
        if (i + j >= dest_size)
            return -1;

        dest[i + j] = src[j];
    }
    dest[i + j] = '\0';

    return i + j;
}

char *align(unsigned int n, char *out, char *in, int outsize) {
    int in_len = strlen(in);
    for (int i = 0; i < n; i++) {
        if (i < in_len)
            out[i] = in[i];
        else
            out[i] = ' ';
    }
    out[n] = '\0';
    return out;
}

char *strcolumns(char *dest, int dest_size, int al, char **srcarr, int src_len) {
    for (int i = 0; i < src_len; i++) {
            align(al, dest + i*al, srcarr[i], dest_size);
    }
    return dest;
}


int strlen(char* str) {
    int result = 0;

    if (str)
        while(str[result]) result++;

    return result;
}


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

char sc2ascii_table[] = {0xFF, 27, '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '-', '=', 8, 9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 13, 2, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', 39, '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 1,
    150, 4, ' ', 12, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 11, 10,
    147, 148, 149, 153, 144, 145, 146, 151, 141, 142, 143, 140, 154, 14, 255,
    255, 138, 139, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 152, 3, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 6, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 155, 7, 15, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 16, 17, 29 ,30, 31, 255, 26, 255, 28, 255, 23, 24, 25, 22,
    0x7f, 255, 255, 255, 255, 255, 255, 255, 20, 21, 18, 255, 255};

char sc2ascii(unsigned char sc) {
    int idx = (int)sc;
    return sc2ascii_table[idx];
}

char *reversestr(char *str) {
    int str_len = strlen(str);
    for (int i = 0; i < str_len/2; i++) {
        char aux = str[i];
        str[i] = str[str_len - 1 - i];
        str[str_len - 1 - i] = aux;
    }
    return str;
}

char *uitoa(unsigned int n, char *str) {
    int i = 0;
    do {
        str[i++] = '0' + (n % 10);
    } while ((n /= 10) > 0);

    str[i] = '\0';

    return reversestr(str);
}


