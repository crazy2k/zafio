#ifndef __ELF_HELPERS_H__

#define __ELF_HELPERS_H__

#include "elf.h"
#include "types.h"
#include "progs.h"

#define ELFH(file) ((Elf32_Ehdr*) (file))

#define ELFPHS(file) ({ \
        void* __file = (file); \
        (Elf32_Phdr*) (__file + ELFH(__file)->e_phoff); })


#define ELFPH_OFFSET(file, ph) ((void *) ((file) + (ph)->p_offset))

#define ELFSHS(file) ({ \
        void* __file = (file); \
        (Elf32_Shdr*) (__file + ELFH(__file)->e_shoff); })


void* elf_entry_point(void *file);

void* elf_stack_bottom(void *file);

uint32_t page_flags_for(uint32_t flags);

void load_elf_proc_image(void *file, uint32_t pd[]);

void proc_map_vaddr(Elf32_Phdr* ph, void *file, uint32_t pd[], void *vaddr);

#endif
