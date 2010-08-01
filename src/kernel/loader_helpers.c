#include "../inc/multiboot.h"
#include "../inc/mmu.h"
#include "../inc/memlayout.h"
#include "../inc/types.h"
#include "../inc/vmm.h"
#include "../inc/debug.h"

static void kpanic(char *msg) {
    // TODO: Completar
    __asm__ __volatile__("hlt");
}

static void clear_memory(void *start, void *stop) {
    for (char *ptr = start; (void*)ptr < stop; ptr++)
        *ptr = 0;
}

static void map_kernel_pages(uint32_t pd[], void *vstart, void *vstop) {
    for (void *vaddr = vstart; vaddr < vstop; vaddr += PAGE_SIZE) {
        uint32_t new_pte = PTE_PAGE_BASE(KPHADDR(vaddr)) | PTE_G | PTE_PWT |
            PTE_RW | PTE_P;

        uint32_t *pt = (uint32_t *)PDE_PT_BASE(pd[PDI(vaddr)]);
        pt[PTI(vaddr)] = new_pte;
    }
}

static void map_kernel_tables(uint32_t pd[], void *vaddr, void *va_limit, void *table_addr) {
    for (; vaddr < va_limit; vaddr+= PAGE_4MB_SIZE, table_addr += PAGE_SIZE) {
        // Llenamos la nueva tabla con ceros
        clear_memory(table_addr, table_addr + PAGE_SIZE);

        // Apuntamos el PDE a una tabla nueva
        pd[PDI(vaddr)] = PDE_PT_BASE(table_addr) | PDE_P | PDE_PWT | PDE_RW;
    }
}

void verify_multiboot(unsigned int magic) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        kpanic("El kernel debe ser cargado por un bootloader Multiboot.");
}

// TODO: Terminarla y usarla
void modules_gather(multiboot_info_t *mbi) {
    if (!(mbi->flags & (0x1 << 3))) {
        // La informacion sobre modulos no es valida. No entramos en panico
        return;
    }

    int i;
    module_t* mod;
    for (i = 0, mod = (module_t *)mbi->mods_addr;
        i < mbi->mods_count;
        i++, mod++) {

        // mod->mod_start es el inicio del modulo
        // mod->mod_end es el final del modulo
        // mod->string es una cadena asociada al modulo
    }
}

void mmu_init(uint32_t kernel_pd[1024], uint32_t page_tables[][1024],
    memory_info_t *meminfo) {

    // Configurar entries en el directorio de paginas
    uint32_t (*kernel_first_pt)[1024] = &page_tables[0];

    uint32_t pde = PDE_PT_BASE(*kernel_first_pt) | PDE_PWT | PDE_RW| PDE_P;

    kernel_pd[PDI(KERNEL_PHYS_ADDR)] = pde;
    kernel_pd[PDI(KERNEL_VIRT_ADDR)] = pde;

    // No podemos mapear mas memoria que MAX_KERNEL_MEMORY, por la cantidad de
    // tablas que tenemos. Mapeamos en total toda la memoria fisica o
    // MAX_KERNEL_MEMORY, lo que sea mas chico.
    void *mem_limit = PAGE_TO_PHADDR(meminfo->last_page + 1);
    void *vmem_limit = (uint32_t)mem_limit > MAX_KERNEL_MEMORY ?
        KVIRTADDR(MAX_KERNEL_MEMORY) : KVIRTADDR(mem_limit);

    meminfo->tables_count =
        (unsigned long)ALIGN_TO_4MB(mem_limit, TRUE)/PAGE_4MB_SIZE;

    // Apuntamos los PDE a tablas que luego se llenaran
    map_kernel_tables(kernel_pd, KVIRTADDR(0x00000000), vmem_limit,
        page_tables);

    // Mapeamos las paginas de toda la memoria que utilizara el kernel,
    // desde el loader hasta cubrir todas las estructuras page_t
    map_kernel_pages(kernel_pd, KVIRTADDR(LOADER_PHYS_ADDR), vmem_limit);

    //Mapear video
    (*kernel_first_pt)[PTI(VIDEO_PHYS_ADDR)] = PTE_PAGE_BASE(VIDEO_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P;
}

void mbi_gather(multiboot_info_t *mbi, page_t *dest, memory_info_t *meminfo) {
    if (!(mbi->flags & (0x1 << 6))) {
        // El mmap no es valido
        kpanic("El kernel precisa informacion sobre la memoria.");
    }

    page_t *first = NULL;
    page_t *last = NULL;

    // Recorremos el buffer que contiene el memory map
    for (memory_map_t *mmap = (memory_map_t *) mbi->mmap_addr;
        mmap < (memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
        mmap = (memory_map_t *) ((void *)mmap + sizeof(mmap->size) +
            mmap->size)) {

        // Si el tipo no es 1, entonces no es RAM disponible
        if (mmap->type != 1)
            continue;

        // Direcciones en el espacio de memoria fisico
        uint32_t start_addr = (uint32_t) ALIGN_TO_PAGE(mmap->base_addr_low, 1);
        uint32_t stop_addr =
            (uint32_t) ALIGN_TO_PAGE(mmap->base_addr_low + mmap->length_low, 0);

        // Direcciones de las estructuras correspondientes
        page_t *start = dest + start_addr/PAGE_SIZE;
        page_t *stop = dest + stop_addr/PAGE_SIZE;

        // first es la primera estructura de todas
        if (first == NULL) {
            first = start;
            //Dejar en blanco todas las paginas anteriores a first
            clear_memory(dest, first);
        }

        // Ubicamos las estructuras
        for (page_t* current = start; current < stop; current++) {
            current->count = 0;
            current->prev = (page_t *)KVIRTADDR((current - 1));
            current->next = (page_t *)KVIRTADDR((current + 1));
        }

        // Enlazamos con el ultimo "chunk"
        if (last != NULL) {
            start->prev = (page_t *)KVIRTADDR(last);
            last->next = (page_t *)KVIRTADDR(start);
            //Dejar en blanco todas las paginas entre last y start
            clear_memory(last + 1, start);
        }

        last = stop - 1;
    }

    // Enlazamos la ultima de todas con la primera estructura
    first->prev = (page_t *)KVIRTADDR(last);
    last->next = (page_t *)KVIRTADDR(first);

    meminfo->first_page = (page_t *)KVIRTADDR(first);
    meminfo->last_page = (page_t *)KVIRTADDR(last);
    meminfo->lower = mbi->mem_lower;
    meminfo->upper = mbi->mem_upper;
    meminfo->kernel_used_memory = ALIGN_TO_PAGE(meminfo->last_page + 1, TRUE);

}



