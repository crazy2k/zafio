#include "../inc/vmmu.h"

page* page_list __attribute__ ((section (".pages"))) = 0; 

void page_table_map(uint32_t page_table[], void* virtual, void* phisical, uint32_t flags) {
	page_table[PTI(virtual)] = PDE_PT_BASE(phisical) | flags;
}

void page_table_unmap(uint32_t page_table[], void* virtual) {
	page_table[PTI(virtual)] = 0x0;
	
}

void page_dir_map(uint32_t page_dir[], void* virtual, void* phisical, uint32_t flags) {
	page_dir[PDI(virtual)] = PDE_PT_BASE(phisical) | flags;
}

void page_dir_unmap(uint32_t page_dir[], void* virtual) {
    page_dir[PDI(virtual)] = 0x0;	
}

// Mapea una pagina fisca nueva a la direccion virutal, pasada por parametro 
void* new_page(uint32_t page_dir[], void* virual_addr) {
     
   return NULL; 
}

// Retorna la pagina fisica correspondiente a la direccion virtual, a la lista de paginas libres
void free_page(uint32_t page_dir[], void* virual_addr) {

}

