global loader

extern cmain

; Datos para usar en el header Multiboot
MAGIC equ 0x1BADB002 ; la especificacion requiere este numero magico
FLAGS equ 0          ; por ahora, no le pedimos nada al bootloader
CHECKSUM equ (- MAGIC - FLAGS)


section .text
dd MAGIC
dd FLAGS
dd CHECKSUM

STACK_SIZE equ 0x00001000

loader:
    mov esp, stack + STACK_SIZE

    ; si fuimos cargados por un bootloader Multiboot, entonces:
    ; - eax contiene el numero magico,
    ; - ebx contiene la direccion fisica de la estructura con informacion
    ;   que nos dejo el bootloader
    
    ; le pasamos esta informacion al codigo C
    push ebx
    push eax
    xchg bx, bx

    call cmain

    ; nunca deberiamos llegar aca
parar:
    hlt
    jmp parar


section .bss
stack:
    resb STACK_SIZE
