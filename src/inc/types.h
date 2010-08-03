#ifndef __TYPES_H__

#define __TYPES_H__

#include <stdint.h>
// Null
#define NULL 0

// bool type
typedef signed int bool;

#define TRUE 1
#define FALSE 0

// Miselaneaous types
typedef unsigned int reg_t;
typedef void *ptr_t;
typedef unsigned int size_t;
typedef signed int time_t;

#define __LOW2_BITS__ 0x00000003
#define __LOW16_BITS__ 0x0000FFFF
#define __LOW12_BITS__ 0x00000FFF
#define __HIGH16_BITS__ 0xFFFF0000
#define __16_23_BITS__ 0x00FF0000
#define __24_31_BITS__ 0xFF000000
#define __16_19_BITS__ 0x000F0000
#define __12_31_BITS__ 0xFFFFF000

#define __22_31_BITS__ 0xFFC00000
#define __12_21_BITS__ 0x003FF000


#endif
