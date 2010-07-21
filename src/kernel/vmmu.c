#include "../inc/vmmu.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"

page pages[] __attribute__ ((section (".pages"))) = { {} }; 
page* page_list = NULL; 

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

/*void page_table_map(uint32_t page_table[], void* virtual, void* phisical, uint32_t flags) {
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
}*/


// Obtiene el entry correspondiente a la direccion virtual dentro del directorio page_dir
uint32_t* get_page_table_entry(uint32_t page_dir[], void* virtual) {
    uint32_t dir_entry = page_dir[PDI(virtual)];

    //Si la tabla de paginas no esta disponible, retornar NULL
    if (!(dir_entry & PDE_P)) return NULL;

    uint32_t *table = KVIRTADDR( PDE_PT_BASE(dir_entry) );
    return &table[PTI(virtual)];
}

// Mapea una pagina fisica nueva para una tabla de paginas de page_dir
void allocate_page_table(uint32_t page_dir[], void* virtual) {
    page *res = reserve_page(page_list->next);
    void *phisical = PAGE_TO_PHADDR(res);
    page_dir[PDI(virtual)] = PDE_PT_BASE(phisical) | PDE_P | PDE_PWT;
    memset(KVIRTADDR(phisical), 0, PAGE_SIZE);
}

// Mapea una pagina fisca nueva para la direccion virtual, pasada por parametro 
void* new_page(uint32_t page_dir[], void* virtual, uint32_t flags) {
    /*
    TODO: Si no hay mas paginas libres kpanic!!!
    if (!page_list) kpanic("No hay mas memoria fisica disponible");
    */  

    // Si la tabla de paginas no estaba presente mapearla
    if (!(page_dir[PDI(virtual)] & PDE_P))
        allocate_page_table(page_dir, virtual);

    uint32_t *entry = get_page_table_entry(page_dir, virtual);
    page *res = reserve_page(page_list->next);
    void *phisical = PAGE_TO_PHADDR(res);

    *entry = PTE_PAGE_BASE(phisical) | PTE_P | flags;

    return virtual; 
}

// Retorna la pagina fisica correspondiente a la direccion virtual, a la lista de paginas libres
void free_page(uint32_t page_dir[], void* virtual) {
    uint32_t *entry = get_page_table_entry(page_dir, virtual);
    page* fpage = PHADDR_TO_PAGE( PTE_PAGE_BASE(*entry) );

    return_page(fpage);
    *entry = 0;
}


// Decrementa el contador de referencias y retorna la pagina a la lista de paginas libres al llegar a 0
void return_page(page* returned) {
    if ( --returned->count == 0 ) {
        if (page_list)
            add_page_to_list(page_list, returned);
        else
            page_list = returned->next = returned->prev = returned;
    }
}

// Saca a la pagina de la lista de paginas libres e incrementa el contador de referencias
page *reserve_page(page* reserved) {
    // Si tiene nodos adyacentes linkearlos entre si
    if (reserved->next && reserved->prev)
        link_pages(reserved->prev, reserved->next);

    // Si la pagina era la primera de la lista, poner a otra como primera
    if (reserved == page_list) {
        if ( reserved->next != reserved )
            page_list = reserved->next;
        else page_list = NULL;
    }

    // Incrementar contador, y marcar nodos adyacentes como nulos
    reserved->count++;
    reserved->next = reserved->prev = NULL;

    return reserved;
}

