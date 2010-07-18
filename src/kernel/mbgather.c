#include "../inc/multiboot.h"
#include "../inc/mmu.h"

#define IS_ALIGNED(addr) !(addr & 0xFFF)

void mbgather(multiboot_info_t *mbi, page *dest) {
    if !(mbi->flags & (0x1 << 6)) {
        // El mmap no es valido
    }

    page *first = NULL;
    page *last = NULL;
    page *current = NULL;

    // Recorremos el buffer que contiene el memory map
    memory_map_t *mmap;
    for (mmap = mbi->mmap_addr;
        mmap < mbi->mmap_addr + mbi->mmap_length;
        mmap += sizeof(mmap->size) + mmap->size) {

        // Si el tipo no es 1, entonces no es RAM disponible
        if (mmap->type != 1)
            continue;

        // Direcciones en el espacio de memoria fisico
        unsigned long start_addr = page_align(mmap->base_addr_low, 1);
        unsigned long stop_addr =
            page_align(mmap->base_addr_low + mmap_length, 0);


        // Direcciones de las estructuras correspondientes
        page *start = dest + start_addr/0x1000;
        page *stop = dest + stop_addr/0x1000;

        // first es la primera estructura de todas
        if (first == NULL)
            first = start;

        // Ubicamos las estructuras
        for (current = start; current < stop; current++) {
            *current = {
                .count = 0, 
                .next = (page*) ((uint32_t) (current - 1) + KERNEL_OFFSET),
                .prev = (page*) ((uint32_t) (current + 1) + KERNEL_OFFSET)
            };
        }

        // Enlazamos con el ultimo "chunk"
        if (last != NULL) {
            start->prev = last;
            last->next = start;
        }

        last = stop - 1;
    }

    // Enlazamos la ultima de todas con la primera estructura
    first->prev = last;
    last->next = first;
}


uint32_t page_align(uint32_t addr, int ceil) {
    // Si la direccion no esta alineada,
    if (!IS_ALIGNED(addr)) {
        addr &= 0xFFFFF000;
        if (ceil)
            addr += 0x1000;
    }
    return addr;
}
