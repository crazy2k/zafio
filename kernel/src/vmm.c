#include "../inc/vmm.h"
#include "../inc/x86.h"
#include "../inc/debug.h"
#include "../inc/mmu.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/io.h"
#include "../inc/sched.h"
#include "../inc/heap.h"

// TODO: Ver mejor en que lugares falta invalidar la tlb

page_t pages[] __attribute__ ((section (".pages"))) = { {} }; 
page_t* page_list = NULL;

memory_info_t memory_info;

void* heap_start;
void* heap_end;
void* kernel_va_limit;

static void update_gdtr();
static void free_pages_setup();
static void heap_setup();
static page_t *reserve_kernel_pages(page_t* page, int n);

static void* seek_contiguous_kvaddr(long pages);
static void shift_kernel_va_limit(void *vaddr);

static void link_pages(page_t *fst, page_t *sec);
static void add_page_to_list(page_t* head, page_t* new);

static page_t *reserve_page(page_t* reserved);
static void return_page(page_t* returned);


void vm_init() {
    update_gdtr();

    free_pages_setup();

    heap_setup();
}

static void free_pages_setup() {
    //Inicializamos, lista de paginas fisicas libres
    page_list = memory_info.first_page;
    kernel_va_limit = memory_info.kernel_used_memory;

    // Marcamos el rango de paginas q no pueden reutilizarse durante la ejecucion del kernel
    reserve_kernel_pages(PHADDR_TO_PAGE(KPHADDR(KERNEL_STACK_TOP)), 
        1 + KERNEL_STACK_SIZE/PAGE_SIZE + memory_info.tables_count);

    int pages_count = PHADDR_TO_PAGE(KPHADDR(kernel_va_limit)) - PHADDR_TO_PAGE(KERNEL_PHYS_ADDR);
    reserve_kernel_pages(PHADDR_TO_PAGE(KERNEL_PHYS_ADDR), pages_count);
}

static void heap_setup() {
    heap_start = kernel_va_limit;
    heap_end = KVIRTADDR(memory_info.tables_count * PAGE_4MB_SIZE);

    heap_config_type(sizeof(tss_t), 4);

    // Correr el espacio disponible del heap asumiendo q las estructuras 
    // ya meapeadas no van a liberarse despues
    heap_start = kernel_va_limit;
}

static void update_gdtr() {
    gdtr.addr = gdt;
    lgdt(gdtr);

    // Quitamos el identity map de los primeros 4MB del espacio de direcciones
    // virtual
    void *vaddr = (void *) 0x00000000;
    kernel_pd[PDI(vaddr)] = 0;
    invalidate_tlb_pages(vaddr,1024);
}

//Mapea paginas de uso exclusivo del kernel durante el procesos de inicializacion
static page_t *reserve_kernel_pages(page_t* page, int n) {
    for (int i = 0; i < n; i++) {
        reserve_page(&page[i]);
        page[i].vaddr = KVIRTADDR(PAGE_TO_PHADDR(&page[i]));
    }

    return page;
}

void page_table_map(uint32_t pt[], void* vaddr, void* phaddr, uint32_t flags) {
    pt[PTI(vaddr)] = PDE_PT_BASE(phaddr) | flags;
}

void invalidate_tlb(void *vaddr) {
    __asm__ __volatile__("invlpg (%0)" : : "r" (vaddr));
}

void invalidate_tlb_pages(void *vstart, int n) {
    for (int i = 0; i < n; i++)
        invalidate_tlb(vstart + i*PAGE_SIZE);
}

//Traduce una direccion virtual del kernel a su direccion fisica
void *get_kphaddr(void *kvaddr) {
    return (void*) ( PTE_PAGE_BASE(*get_pte(kernel_pd, kvaddr)) | 
        ((uint32_t) kvaddr & __LOW12_BITS__) );
}

// Devuelve un puntero a la entrada de la tabla de paginas correspondiente a
// la direccion virtual 'vaddr' usando el directorio 'pd'.
uint32_t* get_pte(uint32_t pd[], void* vaddr) {
    uint32_t pde = pd[PDI(vaddr)];

    //Si la tabla de paginas no esta disponible, retornar NULL
    if (!(pde & PDE_P))
        return NULL;

    uint32_t *pt = PHADDR_TO_PAGE(PDE_PT_BASE(pde))->vaddr;
    return &pt[PTI(vaddr)];
}

// Mapea una pagina fisica nueva para una tabla de paginas de page_dir
void *new_page_table(uint32_t pd[], void* vaddr) {
    void *page_va = new_kernel_page();
    page_t *page = PHADDR_TO_PAGE(get_kphaddr(page_va));

    pd[PDI(vaddr)] = PDE_PT_BASE(PAGE_TO_PHADDR(page)) | PDE_P | PDE_PWT |
        PDE_US | PDE_RW;
    memset(page_va, 0, PAGE_SIZE);

    return page_va;
}

// Conecta entre si la paginas fst con sec
static void link_pages(page_t *fst, page_t *sec) {
    fst->next = sec;
    sec->prev = fst;
}

// Pone en la lista a la pagina obj despues de la pagina referenciada por list
static void add_page_to_list(page_t* head, page_t* new) {
    link_pages(new, head->next);
    link_pages(head, new);
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
        new_page_table(pd, vaddr);

    page_t *page = reserve_page(page_list->next);
    page->vaddr = vaddr;
    uint32_t *pte = get_pte(pd, vaddr);

    if (*pte & PDE_P)
        kpanic("La direccion virtual que se intentaba asignar" 
            " ya estaba mapeada en el directorio");

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

// Mapea una pagina fisica nueva sobre una direccion virtual libre en espacio virutal del kernel
void *new_kernel_page() {
    return new_kernel_pages(1);
}

// Mapea 'n' paginas fisicas nuevas sobre direcciones virtuales libres en espacio virtual del kernel
void *new_kernel_pages(long n) {
    void *vaddr = seek_contiguous_kvaddr(n);
    if (!vaddr) kpanic("No hay suficiente espacio virtual disponible contiguo");

    new_pages(kernel_pd, vaddr, n, PTE_G | PTE_PWT | PTE_RW | PTE_P);
    shift_kernel_va_limit(vaddr + (PAGE_SIZE*n));

    return vaddr;
}

static void shift_kernel_va_limit(void *vaddr) {
    if (get_kphaddr(kernel_va_limit) != NULL)
        kernel_va_limit = vaddr;
    
    if (kernel_va_limit == heap_end)
        kernel_va_limit = heap_start;
}

// Busca en el espacio de direcciones virtuales del kernel, 'n' paginas contiguas libres
static void* seek_contiguous_kvaddr(long n) {
    void *result = NULL;

    //Busco en el espacio entre kernel_va_limit y heap_end
    for (void *vaddr = kernel_va_limit; vaddr < heap_end && !result; vaddr += PAGE_SIZE) {
        bool avail = TRUE;
        for (int i = 0; i < n && avail; i++)
            avail = !(*get_pte(kernel_pd, vaddr + i) & PTE_P);
        
        if (avail) result = vaddr;
    }

    if (result) return result;

    //Si no encuentro, busco en el espacio entre heap_start y kernel_va_limit
    for (void *vaddr = heap_start; vaddr < kernel_va_limit && !result; vaddr += PAGE_SIZE) {
        bool avail = TRUE;
        for (int i = 0; i < n && avail; i++)
            avail = !(*get_pte(kernel_pd, vaddr + i) & PTE_P);

        if (avail) result = vaddr;
    }

    return result;
}

// Decrementa el contador de referencias y retorna la pagina a la lista de
// paginas libres si este llego a cero
static void return_page(page_t* reserved) {
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
static page_t *reserve_page(page_t* page) {
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

void *clone_pd(uint32_t pd[]) {
    uint32_t *new_pd = new_kernel_page();
    memcpy(new_pd, pd, PAGE_SIZE);
    return new_pd;
}

