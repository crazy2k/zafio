#include "../inc/mmu.h"

#ifndef __VMMU_H__

#define __VMMU_H__

#define RELOCATE_PTR_TO(pointer, virtual_dir) \
	((void*) ( ((uint32_t)pointer & 0xFFF) | ((uint32_t)virtual_dir & 0xFFFFF000) ))

extern uint32_t kernel_pd[1024];
extern uint32_t kernel_pt[1024];

extern page* page_list;
extern page pages[];

void link_pages(page*, page*);

void page_table_map(uint32_t[], void* , void* , uint32_t);

void page_table_unmap(uint32_t page_dir[], void* virtual);

void page_dir_map(uint32_t[], void* , void* , uint32_t);

void page_dir_unmap(uint32_t page_dir[], void* vlirtual);

#endif
