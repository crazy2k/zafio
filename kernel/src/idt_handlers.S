%include "./kernel/inc/nasm_macros.inc"

extern set_handler
extern idt_handle

global set_handlers
global load_state


%define IDT_LENGTH      256
%define IDT_LAST_INDEX  (IDT_LENGTH - 1)

; Funcion llamada desde C. Se ocupa de llamar a la funcion de C set_handler(),
; encargada de registrar un handler para un cierto indice en la GDT, pasandole
; uno a uno los handlers creados.
section .text
set_handlers:
    c_enter

%assign i 0
%rep (IDT_LAST_INDEX + 1)
    push dword idt_handler_ %+ i
    push i
    call set_handler
    add esp, 2*4
    %assign i (i + 1)
%endrep

    c_leave

; Macro que crea un handler para una entrada en la IDT.
; - %1 es el indice de la IDT en el que se ubicara una referencia a el
; - %2 es 1 si la excepcion admite un codigo de error y 0 si no; este
;   parametro es opcional y por omision vale 0.
%macro CREATE_HANDLER 1-2 0
section .text
idt_handler_%1:
    ; Guardamos el codigo de error, si lo hay
    %if %2 != 0
        pop dword [error_code_%1]
    %endif

    ; Salvamos el estado en la pila
    push ds
    push es
    pusha

    ; Llamamos a la funcion que manejara la excepcion/interrupcion
    push esp
    push dword [error_code_%1]
    push %1
    call idt_handle
    add esp, 3*4

    jmp load_state

section .data
error_code_%1: dd 0x0
%endmacro

; Creamos IDT_LENGTH handlers
%assign i 0
%rep IDT_LENGTH
    %if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
        CREATE_HANDLER i, 1
    %else
        CREATE_HANDLER i
    %endif
    %assign i (i + 1)
%endrep

; Carga el estado guardado en la pila.
section .text
load_state:
    popa
    pop es
    pop ds

    iret

