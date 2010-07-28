/* TODO: Comentar un poco esto */

_loader_phys_addr = 0x00100000;

_kernel_virt_addr = 0xC0200000;
_kernel_phys_addr = 0x00200000;
_kernel_addr_offset = _kernel_virt_addr - _kernel_phys_addr;

_tables_virt_addr = _kernel_virt_addr - 0x1000*253;
_tables_phys_addr = _kernel_phys_addr - 0x1000*253;

/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS {

    . = _loader_phys_addr;

    /* Codigo de loader.S. Tiene que alinearse a 32 bits para que se
     *  encuentre el header Multiboot.
     */
    .textloader ALIGN(0x4) : {
        *(.textloader)
        obj/mbihelpers.o (.*)
    }

    . = _tables_virt_addr;

    /* Principio del stack del loader/kernel al comienzo de 2MB - 8K
     * (crece hacia abajo)
     */
    _stack_bottom = .;

    /* Tabla de paginas del kernel */
    .pt : AT(ADDR(.pt) - _kernel_addr_offset) {
        *(.pt)
    }

    /* Directorio de paginas inicial */
    .pd : AT(ADDR(.pd) - _kernel_addr_offset) {
        *(.pd)
    }

    . = _kernel_virt_addr;

    /* Codigo del kernel */
    .text : AT(ADDR(.text) - _kernel_addr_offset) {
        *(.text)
    }

    .rodata : AT(ADDR(.rodata) - _kernel_addr_offset) {
        *(.rodata)
    }

    .data : AT(ADDR(.data) - _kernel_addr_offset) {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - _kernel_addr_offset) {
        *(COMMON)
        *(.bss)
    }

    .pages : AT(ADDR(.pages) - _kernel_addr_offset) {
        *(.pages)
    }

}
