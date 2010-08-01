/* TODO: Comentar un poco esto */

__loader_phys_addr = 0x00100000;

__kernel_virt_addr = 0xC0200000;
__kernel_phys_addr = 0x00200000;
__kernel_addr_offset = __kernel_virt_addr - __kernel_phys_addr;

__ktables_virt_addr = __kernel_virt_addr - 0x1000*253;
__ktables_phys_addr = __kernel_phys_addr - 0x1000*253;

/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS {

    . = __loader_phys_addr;

    /* Codigo de loader.S. Tiene que alinearse a 32 bits para que se
     *  encuentre el header Multiboot.
     */
    .textloader ALIGN(0x4) : {
        *(.textloader)
        obj/loader_helpers.o (.*)
    }

    . = __ktables_virt_addr;

    /* Principio del stack del loader/kernel al comienzo de 2MB - 8K
     * (crece hacia abajo)
     */
    __stack_bottom = .;


    /* Directorio de paginas inicial */
    .pd : AT(ADDR(.pd) - __kernel_addr_offset) {
        *(.pd)
    }

    kernel_pts = .;

    . = __kernel_virt_addr;

    /* Codigo del kernel */
    .text : AT(ADDR(.text) - __kernel_addr_offset) {
        *(.text)
    }

    .rodata : AT(ADDR(.rodata) - __kernel_addr_offset) {
        *(.rodata)
    }

    .data : AT(ADDR(.data) - __kernel_addr_offset) {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - __kernel_addr_offset) {
        *(COMMON)
        *(.bss)
    }

    .pages : AT(ADDR(.pages) - __kernel_addr_offset) {
        *(.pages)
    }

}
