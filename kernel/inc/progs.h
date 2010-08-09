#ifndef __PROGS_H__

#define __PROGS_H__

#include "sched.h"
#include "elf_helpers.h"

struct program_t {
    char *name;
    void *file;
    long size;
};

typedef struct program_t program_t;

extern program_t programs[];
extern long programs_size;

struct task_t; 

void load_task_image(struct task_t* );

#define CODE_PAGE_FLAGS (PTE_P | PTE_PWT | PTE_US)
#define DATA_PAGE_FLAGS (PTE_P | PTE_PWT | PTE_US | PTE_RW) 

#endif
