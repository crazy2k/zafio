/* TODO: Comentar un poco esto */

__loader_phys_addr = 0x00100000;

__kernel_virt_addr = 0xC0200000;
__kernel_phys_addr = 0x00200000;
__kernel_addr_offset = __kernel_virt_addr - __kernel_phys_addr;

__stack_size = 0x1000*2;

/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS {

    . = __loader_phys_addr;

    /* Codigo de loader.S. Tiene que alinearse a 32 bits para que se
     *  encuentre el header Multiboot.
     */
    .textloader ALIGN(0x4) : {
        *(.textloader)
        kernel/obj/loader_helpers.o (.*)
    }

    . = ALIGN(0x1000) + __kernel_addr_offset;

    /* Tope del stack */
    __stack_top = .;

    /* Principio del stack del loader/kernel al comienzo de 2MB - 8K
     * (crece hacia abajo)
     */
    . += __stack_size;
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

    .textuser ALIGN(0x1000) : AT(ADDR(.textuser) - __kernel_addr_offset) {
        *(.textuser)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - __kernel_addr_offset) {
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
