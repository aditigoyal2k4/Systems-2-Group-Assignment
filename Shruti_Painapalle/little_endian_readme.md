# Little Endian Converter

This program processes an Assembly program and converts data values into little-endian byte format. It also calculates memory addresses and generates machine-code bytes for supported IA-32 instructions.

## Compile

```bash
g++ -std=c++17 little_endian.cpp -o little_endian
```

## Run

```bash
./little_endian aq
.asm
```

## Input

The program takes one file:

1. **Assembly file** – contains the assembly program with `.data`, `.bss`, and `.text` sections.

## Output

shruti\@shruti:\~/Git-Basics/system2/Systems-2-Group-Assignment/Shruti_Painapalle$ ./little_endian aq.asm

\========== DATA SECTION ==========

Address    Machine Code       Declaration

\========== BSS SECTION ==========

Address    Size(bytes)    Declaration

\========== TEXT SECTION ==========

Address    Machine Code       Instruction

0000    31 C9     xor ecx,ecx

0002    21 D8     and eax,ebx

0004    09 DE     or esi,ebx

0006    C3     ret

Conversion completed.


