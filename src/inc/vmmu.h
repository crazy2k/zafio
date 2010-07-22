#include "../inc/mmu.h"

#ifndef __VMMU_H__

#define __VMMU_H__

#define RELOCATE_PTR_TO(pointer, virtual_dir) \
	((void*) ( ((uint32_t)pointer & 0xFFF) | ((uint32_t)virtual_dir & 0xFFFFF000) ))

/* Recibe un puntero a ``page``, y devuelve la direccion fisica de la pagina
 * representada por dicha estructura
 */
#define PAGE_TO_PHADDR(obj) ((void *) (((obj) - pages)*PAGE_SIZE))

/* Recibe la direccion fisica de una pagina y devuelve un puntero a la
 * estructura ``page`` que la representa
 */
#define PHADDR_TO_PAGE(addr) ((page *) (pages + ((uint32_t) addr)/PAGE_SIZE))

extern uint32_t kernel_pd[1024];
extern uint32_t kernel_pt[1024];

extern page* page_list;
extern page pages[];

typedef struct {
    page *first;
    page *last;

} memory_info_t;

extern memory_info_t memory_info;

void link_pages(page*, page*);

/*void page_table_map(uint32_t[], void* , void* , uint32_t);

void page_table_unmap(uint32_t page_dir[], void* virtual);

void page_dir_map(uint32_t[], void* , void* , uint32_t);

void page_dir_unmap(uint32_t page_dir[], void* vlirtual);*/

uint32_t* get_page_table_entry(uint32_t page_dir[], void* virtual);

void allocate_page_table(uint32_t page_dir[], void* virtual);

void* new_page(uint32_t page_dir[], void* virual_addr, uint32_t flags);

void free_page(uint32_t page_dir[], void* virual_addr);

void return_page(page* returned);

page *reserve_page(page* reserved);

#endif
