#include "../inc/multiboot.h"
#include "../inc/mmu.h"
#include "../inc/memlayout.h"
#include "../inc/types.h"

#define IS_ALIGNED(addr) !(addr & 0xFFF)

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
            addr += 0x1000;
    }
    return addr;
}

void mbigather(multiboot_info_t *mbi, page *dest) {
    if (!(mbi->flags & (0x1 << 6))) {
        // El mmap no es valido
        panic("El kernel precisa informacion sobre la memoria.");
    }

    page *first = NULL;
    page *last = NULL;
    page *current = NULL;

    // Recorremos el buffer que contiene el memory map
    memory_map_t *mmap;
    for (mmap = (memory_map_t *) mbi->mmap_addr;
        mmap < (memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
        mmap += sizeof(mmap->size) + mmap->size) {

        // Si el tipo no es 1, entonces no es RAM disponible
        if (mmap->type != 1)
            continue;

        // Direcciones en el espacio de memoria fisico
        unsigned long start_addr = page_align(mmap->base_addr_low, 1);
        unsigned long stop_addr =
            page_align(mmap->base_addr_low + mmap->length_low, 0);

        // Direcciones de las estructuras correspondientes
        page *start = dest + start_addr/0x1000;
        page *stop = dest + stop_addr/0x1000;

        // first es la primera estructura de todas
        if (first == NULL)
            first = start;

        // Ubicamos las estructuras
        for (current = start; current < stop; current++) {
            current->count = 0;
            current->next = (page*) ((uint32_t) (current - 1) + KERNEL_OFFSET);
            current->prev = (page*) ((uint32_t) (current + 1) + KERNEL_OFFSET);
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



