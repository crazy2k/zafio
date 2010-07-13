global loader

extern cmain
extern _gdt_begin
extern _gdt_end
extern _gdt_len
extern _pd_begin


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

loader:
    cli

    ; cargamos la GDT
    lgdt [gdtr]

    ; indicamos ubicacion del directorio de paginas
    mov eax, _pd_begin
    mov cr3, eax

    ; activamos paginacion
    mov eax, cr0
    or eax, 0x80000000  ; activamos el bit PG
    mov cr0, eax

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


section .data
gdt_len: equ _gdt_len
gdtr: dw _gdt_len - 1 ; se escribe sizeof(gdt) - 1
      dd _gdt_begin


section .kernelstack
stack:
    resb STACK_SIZE
