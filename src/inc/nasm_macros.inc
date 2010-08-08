%macro c_enter 0-1 0
    push ebp
    mov ebp, esp
    %if %1 != 0
        sub esp, %1
    %endif
    push edi
    push esi
    push ebx
%endmacro

%macro c_leave 0-1 0
    pop ebx
    pop esi
    pop edi
    %if %1 != 0
        add esp, %1
    %endif
    pop ebp

    ret
%endmacro
