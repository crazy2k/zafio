#include "../inc/vmm.h"
#include "../inc/debug.h"
#include "../inc/mmu.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/io.h"
#include "../inc/heap.h"

// TODO: Ver mejor en que lugares falta invalidar la tlb

page_t pages[] __attribute__ ((section (".pages"))) = { {} }; 
page_t* page_list = NULL;

memory_info_t memory_info;
void* kernel_va_limit;
void* used_mem_limit;

static void update_gdtr();

void vm_init() {
    update_gdtr();

    BOCHS_BREAK;

    //Inicializamos, lista de paginas fisicas libres
    page_list = memory_info.first_page;

    // Marcamos el rango de paginas q no pueden reutilizarse durante la ejecucion del kernel
    reserve_pages(PHADDR_TO_PAGE(KPHADDR(KERNEL_STACK_FST_PAGE)), 3 + memory_info.tables_count);

    // Puntero a la siguiente posicion de memoria sin utilizar (alineada a 8 bytes)
    used_mem_limit = memory_info.kernel_used_memory;
 
    // Limite actual de la memoria virtual
    kernel_va_limit = kernel_va_limit; 

    int pages_count = PHADDR_TO_PAGE(KPHADDR(kernel_va_limit)) - PHADDR_TO_PAGE(KERNEL_PHYS_ADDR);

    reserve_pages(PHADDR_TO_PAGE(KERNEL_PHYS_ADDR), pages_count);

    //TODO: Configurar heap
    //heap_configure_type(32, 50);
}

static void update_gdtr() {
    gdtr.addr = gdt;
    __asm__ __volatile__ ( "lgdt %0" : : "m"(gdtr) ); 

    // Quitamos el identity map de los primeros 4MB del espacio de direcciones
    // virtual
    page_dir_unmap(kernel_pd, (void *)0x00000000);
}

void reserve_pages(page_t* page, int n) {
    page_t* page_next;

    for (int i = 0; i < n; i++) {
        page_next = page->next;
        reserve_page(page);
        page = page_next;
    }
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
    for (int i = 0; i < n; i++)
        invalidate_tlb(vstart + i*PAGE_SIZE);
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

void *allocate_pages(int n) {
    //TODO
    return NULL;
}

void *map_kernel_page(page_t* page) {
    void* page_va = NULL;
    //TODO
    return page_va;
}

// Mapea una pagina fisica nueva para una tabla de paginas de page_dir
void allocate_table(uint32_t pd[], void* vaddr) {
    page_t *page = reserve_page(page_list->next);
    void *page_va = map_kernel_page(page);

    pd[PDI(vaddr)] = PDE_PT_BASE(PAGE_TO_PHADDR(page)) | PDE_P | PDE_PWT;
    memset(page_va, 0, PAGE_SIZE);
}

// Mapea 'n' paginas fisicas nuevas a apartir de la direccion virtual pasada por parametro 
void* new_pages(uint32_t pd[], void* vaddr, long n, uint32_t flags) {
    for (int i = 0; i < n ; i++)
        new_page(pd, vaddr + PAGE_SIZE*i, flags);

    return vaddr;
}

// Mapea una pagina fisica nueva para la direccion virtual pasada por parametro 
void* new_page(uint32_t pd[], void* vaddr, uint32_t flags) {
    if (!page_list) 
        kpanic("No hay mas memoria fisica disponible");

    vaddr = ALIGN_TO_PAGE(vaddr,FALSE);

    // Si la tabla de paginas no estaba presente mapearla
    if (!(pd[PDI(vaddr)] & PDE_P))
        allocate_table(pd, vaddr);

    page_t *page = reserve_page(page_list->next);
    uint32_t *pte = get_pte(pd, vaddr);

    if (!(*pte & PDE_P))
        kpanic("La direccion virtual que se intentaba asignar" 
            "ya estaba mapeada en el directorio");

    *pte = PTE_PAGE_BASE(PAGE_TO_PHADDR(page)) | PTE_P | flags;

    return vaddr; 
}

// Retorna la pagina fisica correspondiente a la direccion virtual a la lista
// de paginas libres, y la borra la entry en la tabla de paginas
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


//kbrk y ksbrk, solo corren el limite de la vaddr del kernel hacia adelante, no permite q el kernel ceda paginas
//En caso de intentar correr el limite hacia atras las funcs no tiene efecto
long kbrk(void* vaddr) {
    long bytes = ALIGN_TO_PAGE(vaddr, TRUE) - kernel_va_limit;

    if (bytes < 0) bytes = 0;
    ksbrk(bytes);

    return bytes;
}

void* ksbrk(unsigned long bytes) {
    bytes = (long int) ALIGN_TO_PAGE(bytes, TRUE);

    while (bytes) {
        new_page(kernel_pd, kernel_va_limit, PTE_G | PTE_PWT | PTE_RW | PTE_P);

        kernel_va_limit += PAGE_SIZE;
        bytes -= PAGE_SIZE;
    }

    return kernel_va_limit;
}

