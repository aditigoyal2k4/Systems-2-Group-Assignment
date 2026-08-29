# Opcode Checker

This program checks an Assembly program using an opcode table stored in a PDF file.

## Compile

```bash
g++ analyzer.cpp -o analyzer
```

## Run

```bash
./analyzer system_2.pdf program.asm
```

## Input

The program takes two files:

1. **Opcode PDF** – contains the opcode table.
2. **Assembly file** – contains the assembly program.

## Output

shruti\@shruti:\~/System-II$ ./a system\_2.pdf 6\_08.asm

Mnemonic: MOV

Exists in opcode table: YES

Operand used: eax,ebx

Type of operand: REGISTER, REGISTER

\----------------------------------

Mnemonic: MOV

Exists in opcode table: YES

Operand used: edx,10

Type of operand: REGISTER, IMMEDIATE


