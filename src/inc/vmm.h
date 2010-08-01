#include "../inc/mmu.h"

#ifndef __VMM_H__

#define __VMM_H__

/* Recibe un puntero a ``page_t``, y devuelve la direccion fisica de la pagina
 * representada por dicha estructura
 */
#define PAGE_TO_PHADDR(page) ((void *) (((page) - pages) * PAGE_SIZE))

/* Recibe la direccion fisica de una pagina y devuelve un puntero a la
 * estructura ``page_t`` que la representa
 */
#define PHADDR_TO_PAGE(addr) ((page_t *) (pages + ((uint32_t) (addr))/PAGE_SIZE))

#define PAGE_TO_KVADDR(page) (KVIRTADDR(PAGE_TO_PHADDR(page)))
#define KVADDR_TO_PAGE(page) (PHADDR_TO_PAGE(KPHADDR(page)))

#define CACHE_LINE 8
#define CACHE_LINE_MASK 0xFFFFFFF8

#define ALIGN_TO_CACHE(num, ceil) \
({  unsigned long __num = (unsigned long)(num); \
    ((__num) & CACHE_LINE_MASK) + (((ceil) && ((__num) & ~CACHE_LINE_MASK)) ? CACHE_LINE : 0); })

extern uint32_t kernel_pd[1024];

extern page_t* page_list;
extern page_t pages[];

extern uint32_t kernel_pts[][1024];

typedef struct {
    page_t *first_page;
    page_t *last_page;
    long lower;
    long upper;
    long tables_count;
    void* kernel_used_memory;
} memory_info_t;

extern memory_info_t memory_info;

extern void* kernel_va_limit;
extern void* used_mem_limit; 

void *malloc_page();
void allocate_page_table(uint32_t page_dir[], void* virtual);

void* new_page(uint32_t page_dir[], void* virual_addr, uint32_t flags);
void* new_pages(uint32_t pd[], void* vaddr, long n, uint32_t flags);

void free_page(uint32_t page_dir[], void* virual_addr);

void return_page(page_t* returned);
void reserve_pages(page_t* pages, int n);

page_t *reserve_page(page_t* reserved);

uint32_t* get_pte(uint32_t pd[], void* vaddr);

void page_table_unmap(uint32_t pt[], void* vaddr);

void page_dir_unmap(uint32_t pd[], void* vaddr);

void invalidate_tlb(void *vaddr);

void invalidate_tlb_pages(void *vstart, int n);

void vm_init();

void *malloc_page();

void *clone_pd(uint32_t pd[]);

#endif

