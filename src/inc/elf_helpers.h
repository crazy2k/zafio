#ifndef __ELF_HELPERS_H__

#define __ELF_HELPERS_H__

#include "elf.h"
#include "types.h"

void load_elf_image(void *elf_file, uint32_t pd[]);

#endif
