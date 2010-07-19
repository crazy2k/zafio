global loader

extern init
extern _kernel_addr_offset
extern _gdt_begin_phys
extern _gdt_len
extern _gdtr_begin_phys
extern _pd_begin_phys
extern _stack_phys
extern _pages_phys
extern _selectors_phys
extern verify_multiboot
extern mbigather
extern cmain


; Datos para usar en el header Multiboot
MAGIC equ 0x1BADB002 ; la especificacion requiere este numero magico
MEMINFO equ (0x1 << 1)
FLAGS equ MEMINFO    ; le pedimos la informacion de memoria al bootloader
CHECKSUM equ (- MAGIC - FLAGS)

STACK_SIZE equ 0x00001000


section .text
; Header Multiboot
dd MAGIC
dd FLAGS
dd CHECKSUM


; NOTA: Tener en cuenta que las direcciones de reubicacion no coinciden con
; las direcciones de carga. Hasta que se activa la paginacion de memoria,
; si se quiere referenciar a una etiqueta X hay que hacerlo restandole el
; valor de la etiqueta _kernel_addr_offset.
loader:
    xchg bx, bx
    cli

    ; cargamos la GDT
    lgdt [_gdtr_begin_phys]
    ; Cargar selectores
    jmp 0x8:init

