%include "./kernel/inc/nasm_macros.inc"

global loader

extern kernel_pd
extern kernel_pts
extern pages
extern memory_info
extern gdtr
extern verify_multiboot
extern mbi_gather
extern mmu_init
extern cmain
extern __stack_bottom


%define KERNEL_OFFSET 0xC0000000
%define KPHADDR(dir) ((dir) - KERNEL_OFFSET)

; Datos para usar en el header Multiboot
MAGIC equ 0x1BADB002 ; la especificacion requiere este numero magico
MEMINFO equ (0x1 << 1)      ; flag de informacion de memoria
FLAGS equ MEMINFO
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

    ;xchg bx, bx     ; magic breakpoint para bochs
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
    mov esp, KPHADDR(__stack_bottom)

    ; morir si no nos cargo un bootloader multiboot
    CCALL verify_multiboot, eax

    ; tomar datos del multiboot info y armar estructuras convenientes
    CCALL mbi_gather, ebx, KPHADDR(pages), KPHADDR(memory_info)

    ; configurar directorio y tablas de paginas
    CCALL mmu_init, KPHADDR(kernel_pd), KPHADDR(kernel_pts), KPHADDR(memory_info)

    ; indicamos ubicacion del directorio de paginas
    mov ecx, KPHADDR(kernel_pd)
    mov cr3, ecx

    ; activamos paginacion
    mov ecx, cr0
    or ecx, 0x80000000  ; activamos el bit PG
    mov cr0, ecx

    ; actualizamos esp con la direccion virtual del stack y lo limpiamos
    mov esp, __stack_bottom
    
    call cmain

    ; nunca deberiamos llegar aca
die:
    hlt
    jmp die
