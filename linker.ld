/* Direccion virtual que sera asignada al kernel.

   El kernel se reservara las direcciones a partir de 3 GB en el
   espacio de direcciones virtual. Por simplicidad, haremos
   corresponder 3GB + 1MB a 1MB. */
_kernel_virt_addr = 0xC0100000;
_kernel_phys_addr = 0x00100000;

_kernel_addr_offset = _kernel_virt_addr - _kernel_phys_addr;

/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS {
    . = _kernel_phys_addr;

    .textloader : {
        *(.textnoreloc)
    }

    .mbihelpers : {
        obj/mbihelpers.o (.*)
    }

    . = . + _kernel_addr_offset;

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
    
    /* Tabla de paginas inicial, ubicada 1MB despues del inicio del kernel */
    . = _kernel_virt_addr + 0x00100000;

    _pt_begin = .;

    .pt : AT(ADDR(.pt) - _kernel_addr_offset) {
        *(.pt)
    }

    /* Directorio de paginas inicial */
    _pd_begin = .;
    _pd_begin_phys = _pd_begin - _kernel_addr_offset;

    .pd : AT(ADDR(.pd) - _kernel_addr_offset) {
        *(.pd)
    }

    /* Stack del kernel al comienzo de 2MB + 8K */
    _stack = .;
    _stack_phys = _stack - _kernel_addr_offset;
    .kernelstack : AT(ADDR(.kernelstack) - _kernel_addr_offset) {
        *(.kernelstack)
        _stack_size = 0x1000;
        . = _stack_size;
    }

    /* GDT */
    _gdt_begin = .;
    _gdt_begin_phys = _gdt_begin - _kernel_addr_offset;

    .gdt : AT(ADDR(.gdt) - _kernel_addr_offset) {
        *(.gdt)
    }

    _gdt_end = .;
    _gdt_len = _gdt_end - _gdt_begin;

    /* Registro para la GDT */
    _gdtr_begin = .;
    _gdtr_begin_phys = _gdtr_begin - _kernel_addr_offset;
    .gdtr : AT(ADDR(.gdtr) - _kernel_addr_offset) {
        *(.gdtr)
    }

    .pages ALIGN(0x1000) : {
        *(.pages)
    }
    _pages_phys = ADDR(.pages) - _kernel_addr_offset;

}
