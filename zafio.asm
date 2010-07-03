%define BASE 0x1200

ORG BASE
BITS 16

jmp init

init:
    cli

    mov ax, 0xB800
    mov es, ax
    
    mov byte [es:0], 0x30

    jmp $
