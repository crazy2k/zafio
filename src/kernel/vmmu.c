#include "../inc/vmmu.h"

page pages[] __attribute__ ((section (".pages"))) = { {} }; 
page* page_list = 0; 

// Conecta entre si la paginas fst con sec
void link_pages(page *fst, page *sec) {
    fst->next = sec;
    sec->prev = fst;
}

// Pone en la lista a la pagina obj despues de la pagina referenciada por list
void add_page_to_list(page* list, page* obj) {
    page *next = list->next;
    link_pages(list, obj);
    link_pages(obj, next);
}

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

void return_page(page* returned) {
    if ( --returned->count == 0 ) {
        if (page_list)
            add_page_to_list(page_list, returned);
        else
            page_list = returned->next = returned->prev = returned;
    }
}

// Saca a la pagina de la lista de paginas libres
void reserve_page(page* reserved) {
    if (reserved->next && reserved->prev)
        link_pages(reserved->next, reserved->prev);

    if (reserved == page_list) {
        if ( reserved->next == reserved )
            page_list = NULL;
        else page_list = reserved->next;
    }

    reserved->count++;
    reserved->next = reserved->prev = NULL;
}

