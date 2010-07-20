global loader

extern kernel_pd
extern pages
extern gdtr
extern verify_multiboot
extern mbigather
extern cmain
extern _stack_bottom


%define KERNEL_OFFSET 0xC0000000
%define KPHADDR(dir) (dir - KERNEL_OFFSET) 


; Datos para usar en el header Multiboot
MAGIC equ 0x1BADB002 ; la especificacion requiere este numero magico
MEMINFO equ (0x1 << 1)
FLAGS equ MEMINFO    ; le pedimos la informacion de memoria al bootloader
CHECKSUM equ (- MAGIC - FLAGS)


section .textloader
; Header Multiboot
dd MAGIC
dd FLAGS
dd CHECKSUM

loader:
    ; si fuimos cargados por un bootloader Multiboot, entonces:
    ; - eax contiene el numero magico,
    ; - ebx contiene la direccion fisica de la estructura con informacion
    ;   que nos dejo el bootloader (multiboot info)

    xchg bx, bx     ; magic breakpoint para bochs
    cli

    ; cargamos la GDT
    lgdt [KPHADDR(gdtr)]

    ; cargamos los selectores de segmento
    jmp 0x8:selectors

selectors:
    mov dx, 0x10
    mov ds, dx
    mov es, dx
    mov ds, dx
    mov fs, dx
    mov gs, dx
    mov ss, dx

    ; armamos un stack para el codigo C
    mov esp, KPHADDR(_stack_bottom)

    ; morir si no nos cargo un bootloader multiboot
    push eax
    call verify_multiboot
    add esp, 4

    ; tomar datos del multiboot info y armar estructuras convenientes
    push KPHADDR(pages)
    push ebx 
    call mbigather
    add esp, 8

    ; indicamos ubicacion del directorio de paginas
    mov ecx, KPHADDR(kernel_pd)
    mov cr3, ecx

    ; activamos paginacion
    mov ecx, cr0
    or ecx, 0x80000000  ; activamos el bit PG
    mov cr0, ecx

    ; actualizamos esp con la direccion virtual del stack y lo limpiamos
    mov esp, _stack_bottom
    
    call cmain

    ; nunca deberiamos llegar aca
die:
    hlt
    jmp die
