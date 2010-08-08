#ifndef __PROGS_H__

#define __PROGS_H__

typedef struct {
    char *name;
    void *file;
    long size;
} program_t;

extern program_t programs[];
extern long programs_size;

#define CODE_PAGE_FLAGS (PTE_P | PTE_PWT | PTE_US)
#define DATA_PAGE_FLAGS (PTE_P | PTE_PWT | PTE_US | PTE_RW) 

#endif
