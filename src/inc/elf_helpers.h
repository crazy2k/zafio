#ifndef __ELF_HELPERS_H__

#define __ELF_HELPERS_H__

#include "elf.h"
#include "types.h"

#define ELFH(file) ((Elf32_Ehdr*)(file))
#define ELFPHS(file) ((Elf32_Phdr*)ELF(file)->e_phoff) 

#define ELFPH(file, idx) ({ \
    Elf32_Ehdr *__elf = ELF(file); \
    long __idx = idx; \
    (Elf32_Phdr*) (__elf->e_phoff + __elf->e_phentsize * __idx)  }) 

typedef struct {
    char *name;
    void *file;
    long size;
} program_t;

extern program_t programs[];
extern long programs_size;

void load_elf_image(void *elf_file, uint32_t pd[]);
void* elf_entry(void *file);

void print_prog(program_t *file);

#endif
