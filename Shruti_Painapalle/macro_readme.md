# Macro Processor – Pass 1

## Description

This C++ program implements Pass 1 of a Macro Processor. It reads NASM-style macro definitions and generates the Macro Name Table (MNT) and Macro Definition Table (MDT).

## Features

- Reads an assembly file using a command-line argument.
- Generates MNT and MDT.
- Replaces formal parameters (`%1`, `%2`) with (`#1`, `#2`).
- Detects duplicate and invalid macro definitions.

## Compilation

```bash
g++ macro.cpp -o macro
```

## Execution

```bash
./macro macro.asm
```

## Sample Output

```text
========== MACRO NAME TABLE (MNT) ==========
Index     Macro Name        Operands          Start Index    End Index      
----------------------------------------------------------------------------
1         INCR              1                 1              2              

2         ADD_NUM           2                 3              5              

3         MOVE_VALUE        2                 6              7              


========== MACRO DEFINITION TABLE (MDT) ==========
Index     Definition
----------------------------------------
1         add #1, 1
2         MEND
3         mov eax, #1
4         add eax, #2
5         MEND
6         mov #1, #2
7         MEND

```

## Files

- `macro.cpp` – Source code.
- `macro.asm` – Input assembly file.
- `README.md` – Project description.

