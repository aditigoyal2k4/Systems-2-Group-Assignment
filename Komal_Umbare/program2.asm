section .data
    num1 db 10
    num2 dw 20
    num3 dd 100

section .bss
    result resb 1
    count resw 1

section .text
    global _start

_start:
    mov eax, ebx
    add eax, 10

loop:
    sub eax, 1
    cmp eax, 0
    jmp loop

exit:
    ret
