# Mnemonic Checker

## Description

This program takes two files:

- Opcode PDF/txt file
- Assembly program file

It reads the opcode table from the PDF and checks whether the mnemonics in the assembly program are present in the opcode table.

The checking starts after `main:`.

## Compile

```bash
g++ analyzer.cpp -o analyzer
```

## Run

```bash
./analyzer system_2.pdf program.asm
```

## Output

```text
Mnemonic: MOV
Exists in opcode table: YES
Operand used: EAX, EBX
Type of operand: REGISTER, REGISTER
----------------------------------------

Mnemonic: MOV
Exists in opcode table: YES
Operand used: ECX, 10
Type of operand: REGISTER, CONSTANT
----------------------------------------


```
