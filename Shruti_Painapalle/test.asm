
section .data
    name db "shruti", 10, 0
    num1 dd 10, 20, 12, 13, 40
    num2 dw 20
    num3 db 5
    message db "Loop started", 10, 0

section .bss
    result resd 1
    buffer resb 10
    numbers resd 5

section .text
    global _start

_start:
    mov ecx, 5          ; loop counter
    mov esi, num1       ; address of array
    mov eax, 0          ; initialize sum

loop_start:
    add eax, [esi]      ; add current array element
    add esi, 4          ; move to next 32-bit element
    loop loop_start     ; decrement ECX and repeat if not zero

    mov [result], eax   ; store sum

    cmp eax, 100
    jg greater_than_100

    mov ebx, 0
    jmp program_exit

greater_than_100:
    mov ebx, 1

program_exit:
    mov eax, 1          ; Linux sys_exit system call
    int 0x80

