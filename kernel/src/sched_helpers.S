%include "./kernel/inc/nasm_macros.inc"

global switch_stack_pointers
global initialize_task

extern load_task_image
extern load_task_stack
extern push_entry_point
extern load_start_task
extern load_state

section .text

%define old_stack_top_ptr [ebp + 8]
%define new_stack_top_ptr [ebp + 12]
switch_stack_pointers:
    push ebp
    mov ebp, esp

    mov eax, old_stack_top_ptr
    mov ecx, new_stack_top_ptr

    ; Guardamos el esp de la tarea vieja
    mov [eax], esp

    ; Cargamos el esp de la tarea a la que vamos
    mov esp, [ecx]

    pop ebp

    ret

initialize_task:
    ; El task_t ya se encuentra en el stack, por lo que no hace falta hacer
    ; push para pasarlo como parametro (le pasamos a las dos funciones lo mismo)
    CCALL load_task_stack
    CCALL push_entry_point
    CCALL load_task_image
    CCALL load_start_task
    add esp, 4

    ; Ahora que tenemos listo nuestro espacio de memoria virtual, cargamos el
    ; estado inicial de la tarea
    jmp load_state
