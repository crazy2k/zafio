/* Direccion virtual que sera asignada al kernel.

   El kernel se reservara las direcciones a partir de 3 GB en el
   espacio de direcciones virtual. Por simplicidad, haremos
   corresponder 3GB + 1MB a 1MB. */
_dirs_virt_addr = 0xC0200000 - 0x1000 * 2;
_dirs_phys_addr = 0x00200000 - 0x1000 * 2;
_kernel_virt_addr = 0xC0200000;
_kernel_phys_addr = 0x00200000;
_loader_phys_addr = 0x00100000;

_kernel_addr_offset = _kernel_virt_addr - _kernel_phys_addr;

/* Definicion del punto de entrada */
ENTRY (loader)

SECTIONS {
    
    . = _loader_phys_addr;
    /* Codigo de loader.S. Tiene que alinearse a 32 bits para que se
       encuentre el header Multiboot. */
    .textloader ALIGN(0x4) : {
        *(.textloader)
    }

    .mbihelpers : {
        obj/mbihelpers.o (.*)
    }


    . = _dirs_virt_addr;

    /* Principio del stack del kernel al comienzo de 2MB - 8K */
    _stack_bottom = .;
    _stack_bottom_phys = _stack_bottom - _kernel_addr_offset;

    /* Tabla de paginas del kernel */
    .pt : AT(ADDR(.pt) - _kernel_addr_offset) {
        *(.pt)
    }

    /* Directorio de paginas inicial */
    _pd_begin_phys = pd - _kernel_addr_offset;

    .pd : AT(ADDR(.pd) - _kernel_addr_offset) {
        *(.pd)
    }


    . = _kernel_phys_addr;

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


    /* Custom Symbols */

    _gdt_begin_phys = gdt - _kernel_addr_offset;
    _gdt_len = 8 * 5; /* Se, ya se, me queres matar, tenes idea de como hacer esto mejor? (sin section) */

    _pages_phys = page_list - _kernel_addr_offset;

}
