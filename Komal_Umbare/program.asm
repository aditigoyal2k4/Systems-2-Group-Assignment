section .text
global main

main:

    MOV EAX, EBX
    MOV ECX, 10

    ADD AX, CX
    SUB AX, VALUE

    MOV [1000], EAX
    MOV EBX, [2000]

    INC EAX
    DEC EDX

    CMP AX, BX

    AND AX, 5

    MUL BX

    JMP LOOP

LOOP:
    ADD EAX, 1
    CMP AX, 10
    JMP LOOP

    CALL FUNCTION
    RET

FUNCTION:
    MOV DX, AX
    ADD DX, 20
    RET

    XYZ AX            
