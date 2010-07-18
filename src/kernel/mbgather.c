#include "../inc/multiboot.h"
#include "../inc/mmu.h"

void mbgather(multiboot_info_t *mbinfo, page *dest) {
    if !(mbinfo->flags & (0x1 << 7)) {
        // El mmap no es valido
    }

    page *current = NULL, *first = NULL, last = NULL;

    memory_map_t *mmap;
    for (mmap = mbinfo->mmap_addr;
        mmap < mbinfo->mmap_addr + mbinfo->mmap_length;
        mmap = mmap + sizeof(mmap->size) + mmap->size) {
    
        if (mmap->type != 1) continue;
       
        int not_aligned = (mmap->base_addr_low & 0XFFF) ? 1 : 0; 
        // Correrse a la siguiente pagina si la direccion no esta alineada a 4K
        int offset = mmap->base_addr_low / 0x1000 + not_aligned; 
        current = dest + offset;

        if (first == NULL) first = current;
        if (last) {
            current->prev = last;
            last->next = current;
        }

        int offset_to_last = (mmap->base_addr_low + mmap->length_low) / 0x1000;
        last = dest + offset_to_last;
        for (; current < last; current++) {
            *current = { .count = 0, 
                .next = (page*) ((uint32_t) (current[i + 1]) + KERNEL_OFFSET),
                .prev = (page*) ((uint32_t) (current[i - 1]) + KERNEL_OFFSET) };
        }
    }

    first->prev = last;
    last->next = first;
}
