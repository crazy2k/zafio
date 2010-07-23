#include "../inc/multiboot.h"
#include "../inc/mmu.h"
#include "../inc/memlayout.h"
#include "../inc/types.h"
#include "../inc/vmmu.h"

void panic(char *msg) {
    // TODO: Completar
    __asm__ __volatile__("hlt");
}

void verify_multiboot(unsigned int magic) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("El kernel debe ser cargado por un bootloader Multiboot.");
    }
}

uint32_t page_align(uint32_t addr, int ceil) {
    // Si la direccion no esta alineada,
    if (!IS_ALIGNED(addr)) {
        addr &= 0xFFFFF000;
        if (ceil)
            addr += PAGE_SIZE;
    }
    return addr;
}

void mbigather(multiboot_info_t *mbi, page_t *dest, memory_info_t *meminfo) {
    if (!(mbi->flags & (0x1 << 6))) {
        // El mmap no es valido
        panic("El kernel precisa informacion sobre la memoria.");
    }

    page_t *first = NULL;
    page_t *last = NULL;
    page_t *current = NULL;

    // Recorremos el buffer que contiene el memory map
    memory_map_t *mmap;
    for (mmap = (memory_map_t *) mbi->mmap_addr;
        mmap < (memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
        mmap = (memory_map_t *) ((void *)mmap + sizeof(mmap->size) +
            mmap->size)) {

        // Si el tipo no es 1, entonces no es RAM disponible
        if (mmap->type != 1)
            continue;

        // Direcciones en el espacio de memoria fisico
        unsigned long start_addr = page_align(mmap->base_addr_low, 1);
        unsigned long stop_addr =
            page_align(mmap->base_addr_low + mmap->length_low, 0);

        // Direcciones de las estructuras correspondientes
        page_t *start = dest + start_addr/PAGE_SIZE;
        page_t *stop = dest + stop_addr/PAGE_SIZE;

        // first es la primera estructura de todas
        if (first == NULL)
            first = start;

        // Ubicamos las estructuras
        for (current = start; current < stop; current++) {
            current->count = 0;
            current->prev = (page_t *)KVIRTADDR((current - 1));
            current->next = (page_t *)KVIRTADDR((current + 1));
        }

        // Enlazamos con el ultimo "chunk"
        if (last != NULL) {
            start->prev = last;
            last->next = start;
        }

        last = stop - 1;
    }

    // Enlazamos la ultima de todas con la primera estructura
    first->prev = (page_t *)KVIRTADDR(last);
    last->next = (page_t *)KVIRTADDR(first);

    meminfo->first = (page_t *)KVIRTADDR(first);
    meminfo->last = (page_t *)KVIRTADDR(last);
    meminfo->lower = mbi->mem_lower;
    meminfo->upper = mbi->mem_upper;
}



