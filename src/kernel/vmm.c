#include "../inc/vmm.h"
#include "../inc/debug.h"
#include "../inc/mmu.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/io.h"

// TODO: Ver mejor en que lugares falta invalidar la tlb

page_t pages[] __attribute__ ((section (".pages"))) = { {} }; 
page_t* page_list = NULL;

memory_info_t memory_info;
void* kernel_va_limit;

void vm_init() {
    // Mapeamos los primeros 4MB fisicos en el higher half
    map_kernel_pages(kernel_pd, (void *)0x00000000 + KERNEL_OFFSET, 1024);

    // Decidimos el resto de la memoria a mapear: No puede ser mas que
    // MAX_KERNEL_MEMORY, que es lo maximo que nos permiten mapear las tablas
    // que tenemos
    uint32_t total_memory = (uint32_t)PAGE_TO_PHADDR(memory_info.last_page + 1);

    if (total_memory > MAX_KERNEL_MEMORY)
        total_memory = MAX_KERNEL_MEMORY;

    total_memory -= PAGE_4MB_SIZE;

    int tables_count = ((uint32_t)ALIGN_TO_4MB(total_memory, TRUE))/PAGE_4MB_SIZE;

    // Vamos a mapear a partir de los 4MB (ya mapeamos lo anterior)
    void *mem_vaddr = (void*)KVIRTADDR(PAGE_4MB_SIZE);

    // Apuntamos los PDE a tablas que luego se llenaran
    map_kernel_tables(kernel_pd, mem_vaddr, page_tables, tables_count);

    // Mapeamos las paginas del resto de la memoria a mapear
    map_kernel_pages(kernel_pd, mem_vaddr, 
        ((uint32_t)ALIGN_TO_PAGE(total_memory, TRUE))/PAGE_SIZE);

    
    // Marcar el rango de paginas q no pueden reutilizarse durante la ejecucion del kernel
    set_unavailable_pages(PHADDR_TO_PAGE(KPHADDR(KERNEL_STACK_FST_PAGE)), 4 + tables_count);

    // Limite actual de la memoria virtual
    kernel_va_limit = ALIGN_TO_PAGE((void*)memory_info.last_page + 1, FALSE);

    int pages_count = PHADDR_TO_PAGE(KPHADDR(kernel_va_limit)) - PHADDR_TO_PAGE(KERNEL_PHYS_ADDR);

    set_unavailable_pages(PHADDR_TO_PAGE(KERNEL_PHYS_ADDR), pages_count);

    // Quitamos el identity map de los primeros 4MB del espacio de direcciones
    // virtual
    page_dir_unmap(kernel_pd, (void *)0x00000000);
}

void set_unavailable_pages(page_t* page, int n) {
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
    void *vstop = vstart + PAGE_SIZE*n;

    for (void *vaddr = vstart; vaddr < vstop; vaddr += PAGE_SIZE) {
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

// Mapea 'n' paginas de tablas para las direcciones virtuales desde 'vaddr'
// alojandolas desde la direccion refenciada por 'table_addr' 
void map_kernel_tables(uint32_t pd[], void *vaddr, void *table_addr, int n) {
    for (int i = 0; i < n; i++, vaddr+= PAGE_4MB_SIZE, table_addr += PAGE_SIZE) {
        // Llenamos la nueva tabla con ceros
        memset(table_addr, 0, PAGE_SIZE);

        // Apuntamos el PDE a una tabla nueva
        pd[PDI(vaddr)] = PDE_PT_BASE(KPHADDR(table_addr)) | PDE_P | PDE_PWT | PDE_RW;

        // Invalidamos la TLB para esta pagina
        invalidate_tlb(table_addr);
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
    if (!page_list) 
        kpanic("No hay mas memoria fisica disponible");

    vaddr = ALIGN_TO_PAGE(vaddr,FALSE);
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

void* malloc(long int size) {
    //TODO: Retornar una puntero a memoria de al menos 'size' bytes dentro del 'heap' del kernel

    return NULL;
}


