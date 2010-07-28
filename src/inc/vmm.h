#include "../inc/mmu.h"

#ifndef __VMM_H__

#define __VMM_H__

#define RELOCATE_PTR_TO(pointer, virtual_dir) \
	((void*) ( ((uint32_t)(pointer) & 0xFFF) | ((uint32_t)(virtual_dir) & 0xFFFFF000) ))

/* Recibe un puntero a ``page_t``, y devuelve la direccion fisica de la pagina
 * representada por dicha estructura
 */
#define PAGE_TO_PHADDR(page) ((void *) (((page) - pages) * PAGE_SIZE))

/* Recibe la direccion fisica de una pagina y devuelve un puntero a la
 * estructura ``page_t`` que la representa
 */
#define PHADDR_TO_PAGE(addr) ((page_t *) (pages + ((uint32_t) (addr))/PAGE_SIZE))

#define IS_ALIGNED(addr) !((uint32_t)(addr) & 0xFFF)

extern uint32_t kernel_pd[1024];
extern uint32_t kernel_pt[1024];

extern page_t* page_list;
extern page_t pages[];

typedef struct {
    page_t *first_page;
    page_t *last_page;
    int lower;
    int upper;

} memory_info_t;

extern memory_info_t memory_info;

void link_pages(page_t*, page_t*);

/*void page_table_map(uint32_t[], void* , void* , uint32_t);

void page_table_unmap(uint32_t page_dir[], void* virtual);

void page_dir_map(uint32_t[], void* , void* , uint32_t);

void page_dir_unmap(uint32_t page_dir[], void* vlirtual);*/

uint32_t* get_page_table_entry(uint32_t page_dir[], void* virtual);

void allocate_page_table(uint32_t page_dir[], void* virtual);

void* new_page(uint32_t page_dir[], void* virual_addr, uint32_t flags);

void free_page(uint32_t page_dir[], void* virual_addr);

void return_page(page_t* returned);

page_t *reserve_page(page_t* reserved);

void map_kernel_pages(uint32_t pd[], void *vstart, int n);

void map_kernel_tables(uint32_t pd[], void *vstart, int n);

uint32_t* get_pte(uint32_t pd[], void* vaddr);

void page_table_unmap(uint32_t pt[], void* vaddr);

void page_dir_unmap(uint32_t pd[], void* vaddr);

void invalidate_tlb(void *vaddr);

void invalidate_tlb_pages(void *vstart, int n);

#endif
