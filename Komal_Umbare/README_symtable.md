# Symbol Table Generator

A simple C++ program that reads a NASM-style assembly (.asm) source
file and an opcode table, then generates a symbol table containing
symbols declared in the .data, .bss, and .text sections.

| Field | Description |
|---|---|
| #id | Unique ID assigned to each symbol |
| Name | Variable name or label |
| Section | Section in which the symbol is declared |
| Size | Size in bytes |
| Address | Section-relative address (offset) |

# To Compile:

```bash
g++ symtable.cpp -o symtable
```

# To Run:

```bash
./symtable opcode2.txt program2.asm
```

## Output:

```text
komal@komal:~/MSc_CS_1st_year/Semester3/System2/symtableAssign$ g++ symtable.cpp -o symtable
komal@komal:~/MSc_CS_1st_year/Semester3/System2/symtableAssign$ ./symtable opcode2.txt program2.asm

            SYMBOL TABLE

#id   Name           Section     Size      Address     Value       
-------------------------------------------------------------------
1     num1           .data       1         0x0        10          
2     num2           .data       2         0x1        20          
3     num3           .data       4         0x3        100         
4     result         .bss        1         0x0        -           
5     count          .bss        2         0x1        -           
6     _start         .text       0         0x0        -           
7     loop           .text       0         0x4        -           
8     exit           .text       0         0xD        -           
-------------------------------------------------------------------

Total symbols: 8
