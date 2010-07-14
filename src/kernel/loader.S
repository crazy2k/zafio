global loader

extern _kernel_addr_offset
extern _gdt_begin_phys
extern _gdt_len
extern _gdtr_begin_phys
extern _pd_begin_phys
extern cmain


; Datos para usar en el header Multiboot
MAGIC equ 0x1BADB002 ; la especificacion requiere este numero magico
FLAGS equ 0          ; por ahora, no le pedimos nada al bootloader
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
    cli

    ; cargamos la GDT
    lgdt [_gdtr_begin_phys]

    ; indicamos ubicacion del directorio de paginas
    mov ecx, _pd_begin_phys
    mov cr3, ecx

    ; activamos paginacion
    mov ecx, cr0
    or ecx, 0x80000000  ; activamos el bit PG
    mov cr0, ecx

    mov esp, stack + STACK_SIZE

    ; si fuimos cargados por un bootloader Multiboot, entonces:
    ; - eax contiene el numero magico,
    ; - ebx contiene la direccion fisica de la estructura con informacion
    ;   que nos dejo el bootloader
    
    ; le pasamos esta informacion al codigo C
    push ebx
    push eax

    call cmain

    ; nunca deberiamos llegar aca
parar:
    hlt
    jmp parar


section .gdtr
gdtr: dw _gdt_len - 1 ; se escribe sizeof(gdt) - 1
      dd _gdt_begin_phys


section .kernelstack
stack:
    resb STACK_SIZE
