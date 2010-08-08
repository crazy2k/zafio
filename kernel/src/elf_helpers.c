#include "../inc/elf_helpers.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"
#include "../inc/utils.h"
#include "../inc/io.h"

void *elf_entry_point(void *file) {
    return (void *)ELFH(file)->e_entry;
}

void* elf_stack_bottom(void *file) {
    //TODO: Stub, completar
    return (void *) 0x40000000;
}

void load_elf_proc_image(void *file, uint32_t pd[]) {
    for (int i = 0; i < ELFH(file)->e_phnum; i++) {
        Elf32_Phdr *ph = &ELFPHS(file)[i];
        void *stop = (void *)ph->p_vaddr + ph->p_memsz;

        for (void *vaddr = (void *)ph->p_vaddr; vaddr < stop; vaddr+= PAGE_SIZE)
            proc_map_vaddr(ph, file, pd, vaddr);
    }
}

uint32_t page_flags_for(uint32_t flags) {
    return flags & PF_X ? CODE_PAGE_FLAGS : DATA_PAGE_FLAGS;
}

void proc_map_vaddr(Elf32_Phdr* ph, void *file, uint32_t pd[], void *vaddr) {
    if ( (void *)ph->p_vaddr >= ALIGN_TO_PAGE(vaddr, FALSE) && 
        vaddr <= ALIGN_TO_PAGE(ph->p_vaddr + ph->p_memsz, TRUE) ) {

        new_page(pd, vaddr, page_flags_for(ph->p_flags));

        void* dest = ALIGN_TO_PAGE(vaddr, FALSE);
        long offset = (dest - (void *)ph->p_vaddr);
        void *src = ELFPH_OFFSET(file, ph) + offset;
        long size = min(PAGE_SIZE, ph->p_filesz - offset);

        memcpy(dest, src, size);

        if(offset + PAGE_SIZE > ph->p_filesz) {
            dest = (void *) dest + size;
            size = min(PAGE_SIZE - size, ph->p_memsz - (offset + size));

            memset(dest, 0, size);
        }
    }
}

void print_prog(program_t *prog) {
    kputs("program name: ");
    kputs(prog->name);
    kputs("\n");

    Elf32_Ehdr* header = ELFH(prog->file);

    kputs("cantidad: ");
    kputd(header->e_phnum);
    kputs("\ntamanio: ");
    kputd(header->e_phentsize);
    kputs("\n");

    for (int i = 0; i < header->e_phnum; i++) {
        kputs("array: ");
        kputui32((uint32_t) (&ELFPHS(prog->file)[i]));
        kputs("\ntipo: ");
        kputd(ELFPHS(prog->file)[i].p_type);
        kputs("\nvaddr: ");
        kputd(ELFPHS(prog->file)[i].p_vaddr);
        kputs("\nmem size: ");
        kputd(ELFPHS(prog->file)[i].p_memsz);
        kputs("\n");
    }
}

