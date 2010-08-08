#include "../inc/mmu.h"

#ifndef __VMM_H__

#define __VMM_H__

//La mayor cantiad de memoria q muede mapear el kernel
#define MAX_KERNEL_MEMORY \
    (((uint32_t)KERNEL_VIRT_ADDR - (uint32_t)kernel_pts)*1024)

//Ultima direccion virtual q puede utilizar el kernel
#define LAST_KERNEL_VADDR ((void *)(MAX_KERNEL_MEMORY + KERNEL_OFFSET))


/* Recibe un puntero a ``page_t``, y devuelve la direccion fisica de la pagina
 * representada por dicha estructura
 */
#define PAGE_TO_PHADDR(page) ((void *) (((page) - pages) * PAGE_SIZE))

/* Recibe la direccion fisica de una pagina y devuelve un puntero a la
 * estructura ``page_t`` que la representa
 */
#define PHADDR_TO_PAGE(addr) ((page_t *) (pages + ((uint32_t) (addr))/PAGE_SIZE))

typedef struct page_t page_t;

struct page_t {
    int count;
    page_t *next;
    page_t *prev;
};

extern page_t* page_list;
extern page_t pages[];

typedef struct {
    page_t *first_page;
    page_t *last_page;
    long lower;
    long upper;
    long tables_count;
    void* kernel_used_memory;
} memory_info_t;

extern memory_info_t memory_info;

void* new_page(uint32_t pd[], void* vaddr, uint32_t flags);
void* new_pages(uint32_t pd[], void* vaddr, long n, uint32_t flags);
void free_page(uint32_t pd[], void* vaddr);

void *new_page_table(uint32_t pd[], void* vaddr);

void *new_kernel_page();
void *new_kernel_pages(long n);

uint32_t* get_pte(uint32_t pd[], void* vaddr);
void *get_kphaddr(void *kvaddr);

void page_table_map(uint32_t pt[], void* vaddr, void* phaddr, uint32_t flags);

void invalidate_tlb(void *vaddr);
void invalidate_tlb_pages(void *vstart, int n);

void vm_init();

void *clone_pd(uint32_t pd[]);

#endif

