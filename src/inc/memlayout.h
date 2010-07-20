#ifndef __MEMLAYOUT_H__

#define __MEMLAYOUT_H__

#include <stdint.h>
#include "types.h"

#define KERNEL_OFFSET 0xC0000000
#define KPHADDR(dir) (((uint32_t) dir) - KERNEL_OFFSET)

#define KERNEL_VIRT_ADDR 0xC0200000
#define KERNEL_PHYS_ADDR KPHADDR(KERNEL_VIRT_ADDR)

#define KERNEL_STACK (KERNEL_VIRT_ADDR - 0x1000 * 3)    // kernel - 12K
#define KERNEL_PAGE_TABLE (KERNEL_VIRT_ADDR - 0x1000 * 2)    // kernel - 8K
#define KERNEL_PAGE_DIRECTORY (KERNEL_VIRT_ADDR - 0x1000)    // kernel - 4K

#define LOADER_ADDR 0x00100000

#define VIDEO_MEMORY 0x000B8000

#endif
