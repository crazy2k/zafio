
/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS{
    /* Direccion virtual que sera asignada al kernel.

       El kernel se reservara las direcciones a partir de 3 GB en el
       espacio de direcciones virtual. Por simplicidad, haremos
       corresponder 3GB + 1MB a 1MB, por lo que la direccion de
       reubicacion inicial sera 3GB + 1MB. */
    _kernel_virt_addr = 0xC0100000;
    _kernel_phys_addr = 0x00100000;

    _kernel_addr_offset = _kernel_virt_addr - _kernel_phys_addr;

    . = _kernel_virt_addr;

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
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }
}
