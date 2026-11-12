%macro INCREMENT 1
    MOV AX, %1
    ADD AX, 1
    MOV %1, AX
%endmacro

%macro ADDITION 2
    MOV AX, %1
    ADD AX, %2
    MOV %1, AX
%endmacro

%macro DISPLAY 1
    MOV DX, %1
    MOV AH, 09
    INT 21H
%endmacro
