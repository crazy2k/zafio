%include "./kernel/inc/nasm_macros.inc"

global switch_stack_pointers

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
