#include "../inc/io.h"

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
