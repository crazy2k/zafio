#include "../inc/vmm.h"
#include "../inc/mmu.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/io.h"

// TODO: Ver mejor en que lugares falta invalidar la tlb

page_t pages[] __attribute__ ((section (".pages"))) = { {} }; 
page_t* page_list = NULL; 

memory_info_t memory_info;


void vm_init() {
    // Mapeamos toda la memoria en la que estaran las tablas de pagina
    int total_tables_pages = count_pages(page_tables, (void *)KERNEL_VIRT_ADDR);
    map_kernel_pages(kernel_pd, page_tables, total_tables_pages);

    // Mapeamos la memoria desde 4MB en adelante
    void *last_phpage = PAGE_TO_PHADDR(memory_info.last_page);
    void *last_addrpage = (void *)(total_tables_pages*PAGE_4MB_SIZE - PAGE_SIZE);

    int total_tables;
    if (last_phpage < last_addrpage)
        total_tables = (uint32_t)ALIGN_TO_4MB(last_phpage, TRUE)/PAGE_4MB_SIZE;
    else
        total_tables = total_tables_pages;

    map_kernel_tables(kernel_pd, (void *)0xC0400000, page_tables, total_tables);

    // Quitamos el mapeo de los primeros 4MB del espacio de direcciones virtual
    page_dir_unmap(kernel_pd, (void *)0x00000000);

}

/* Recibe una direccion de inicio ``saddr`` y una de fin ``eaddr`` y devuelve
 * la cantidad de paginas entre ambas direcciones incluyendo las paginas en
 * las que se encuentran ellas.
 */
uint32_t count_pages(void *saddr, void *eaddr) {
    void *diff = (void *)(ALIGN_TO_PAGE(eaddr, FALSE) - ALIGN_TO_PAGE(saddr, TRUE));
    return ((uint32_t)diff)/PAGE_SIZE;
}

// Conecta entre si la paginas fst con sec
void link_pages(page_t *fst, page_t *sec) {
    fst->next = sec;
    sec->prev = fst;
}

// Pone en la lista a la pagina obj despues de la pagina referenciada por list
void add_page_to_list(page_t* head, page_t* new) {
    link_pages(new, head->next);
    link_pages(head, new);
}

void page_table_unmap(uint32_t pt[], void* vaddr) {
	pt[PTI(vaddr)] = 0x0;
    invalidate_tlb(vaddr);
}

void page_dir_unmap(uint32_t pd[], void* vaddr) {
    pd[PDI(vaddr)] = 0x0;
    // Invalidar la TLB para los 4MB
    invalidate_tlb_pages(ALIGN_TO_4MB(vaddr, FALSE), 1024);
}

void invalidate_tlb(void *vaddr) {
    __asm__ __volatile__("invlpg (%0)" : : "r" (vaddr));
}

void invalidate_tlb_pages(void *vstart, int n) {
    int i;
    for (i = 0; i < n; i++)
        invalidate_tlb(vstart + i*PAGE_SIZE);
}

/*void page_table_map(uint32_t page_table[], void* virtual, void* phisical, uint32_t flags) {
	page_table[PTI(virtual)] = PDE_PT_BASE(phisical) | flags;
}



void page_dir_map(uint32_t page_dir[], void* virtual, void* phisical, uint32_t flags) {
	page_dir[PDI(virtual)] = PDE_PT_BASE(phisical) | flags;
}

*/


// TODO: Tomar una decision con respecto al problema de que se precise otra
// tabla. 
void map_kernel_pages(uint32_t pd[], void *vstart, int n) {
    void *vaddr, *vstop = vstart + PAGE_SIZE*n;

    for (vaddr = vstart; vaddr < vstop; vaddr += PAGE_SIZE) {
        uint32_t new_pte = PTE_PAGE_BASE(KPHADDR(vaddr)) | PTE_G | PTE_PWT |
            PTE_RW | PTE_P;

        uint32_t *pte;
        // Si no existe una tabla, no podemos seguir, ya que esta funcion se
        // utiliza cuando aun no tenemos un mecanismo para reservar paginas
        if (!(pte = get_pte(pd, vaddr)))
            kpanic("map_kernel_pages: No hay tabla para mapear la direccion");

        if (*pte & PTE_P) {
            // Si la pagina ya fue mapeada, solo proseguimos si esta mapeada como
            // queremos. Los bits A y D no nos interesan (son alterados por el
            // hardware)
            if ((*pte & ~(PTE_A) & ~(PTE_D)) != new_pte)
                kpanic("map_kernel_pages: La pagina ya se encuentra" 
                    " mapeada de manera diferente");
        }
        else
            *pte = new_pte;

        invalidate_tlb(vaddr);
    }
}

void map_kernel_tables(uint32_t pd[], void *vstart, void *tables, int n) {
    void *vaddr = vstart, *table_addr = tables;
    int i;

    for (i = 0; i < n; i++) {
        pd[PDI(vaddr + i*PAGE_4MB_SIZE)] = 
            PDE_PT_BASE(KPHADDR(table_addr + i*PAGE_SIZE)) | PDE_P | PDE_PWT |
            PDE_RW;
        memset(vaddr, 0, PAGE_SIZE);
    }
}

// Devuelve un puntero a la entrada en la tabla de paginas correspondiente a
// la direccion virtual ``virt`` usando el directorio ``pd``.
uint32_t* get_pte(uint32_t pd[], void* vaddr) {
    uint32_t pde = pd[PDI(vaddr)];

    //Si la tabla de paginas no esta disponible, retornar NULL
    if (!(pde & PDE_P))
        return NULL;

    uint32_t *pt = KVIRTADDR(PDE_PT_BASE(pde));
    return &pt[PTI(vaddr)];
}

// Mapea una pagina fisica nueva para una tabla de paginas de page_dir
void allocate_pt(uint32_t pd[], void* vaddr) {
    page_t *p = reserve_page(page_list->next);
    void *phpage = PAGE_TO_PHADDR(p);
    pd[PDI(vaddr)] = PDE_PT_BASE(phpage) | PDE_P | PDE_PWT;
    memset(KVIRTADDR(phpage), 0, PAGE_SIZE);
}

// Mapea una pagina fisica nueva para la direccion virtual pasada por parametro 
void* new_page(uint32_t pd[], void* vaddr, uint32_t flags) {
    /*
    TODO: Si no hay mas paginas libres kpanic!!!
    if (!page_list) kpanic("No hay mas memoria fisica disponible");
    */  

    // Si la tabla de paginas no estaba presente mapearla
    if (!(pd[PDI(vaddr)] & PDE_P))
        allocate_pt(pd, vaddr);

    page_t *p = reserve_page(page_list->next);
    void *phpage = PAGE_TO_PHADDR(p);

    uint32_t *pte = get_pte(pd, vaddr);
    *pte = PTE_PAGE_BASE(phpage) | PTE_P | flags;

    return vaddr; 
}

// Retorna la pagina fisica correspondiente a la direccion virtual a la lista
// de paginas libres
void free_page(uint32_t pd[], void* vaddr) {
    uint32_t *pte = get_pte(pd, vaddr);
    page_t *p = PHADDR_TO_PAGE(PTE_PAGE_BASE(*pte));

    return_page(p);
    *pte = 0;
}


// Decrementa el contador de referencias y retorna la pagina a la lista de
// paginas libres si este llego a cero
void return_page(page_t* reserved) {
    (reserved->count)--;
    if (reserved->count == 0) {
        if (page_list)
            add_page_to_list(page_list, reserved);
        else
            page_list = reserved->next = reserved->prev = reserved;
    }
}

// Saca a la pagina de la lista de paginas libres e incrementa su contador de
// referencias
page_t *reserve_page(page_t* page) {
    // Si tiene nodos adyacentes enlazarlos entre si
    if (page->next && page->prev)
        link_pages(page->prev, page->next);

    // Si la pagina era la cabeza de la lista, cambiar la cabeza por otra
    if (page_list == page) {
        if (page->next != page)
            page_list = page->next;
        else page_list = NULL;
    }

    // Incrementar contador, y marcar adyacencias como nulas
    page->count++;
    page->next = page->prev = NULL;

    return page;
}

