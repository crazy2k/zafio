%include "./kernel/inc/nasm_macros.inc"

global switch_stack_pointers
global initialize_task

extern load_task_image
extern load_state

section .text

%define old_stack_top_ptr [esp + 4]
%define new_stack_top_ptr [esp + 8]
switch_stack_pointers:

    ; Guardamos el esp de la tarea vieja
    mov eax, old_stack_top_ptr
    mov [eax], esp

    ; Cargamos el esp de la tarea a la que vamos
    mov eax, new_stack_top_ptr
    mov esp, [eax]

    ret

initialize_task:
    ; El task_t ya se encuentra en el stack, por lo que no hace falta hacer
    ; push para pasarlo como parametro
    CCALL load_task_image
    add esp, 4

    ; Ahora que tenemos listo nuestro espacio de memoria virtual, cargamos el
    ; estado inicial de la tarea
    jmp load_state
