# C++ Program – Opcode and Operand Analyzer


 Compile and Run

Compile:

```bash
g++ assembler.cpp -o assembler
```

Run:

```bash
./assembler
```

Enter:

```text
Enter opcode file name: opcode.txt
Enter assembly program file name: program.asm
```

---

Expected Output

The output will look approximately like:

```text
============================================================
             ASSEMBLY PROGRAM ANALYSIS
============================================================

Mnemonic    Exists    Operand             Type
------------------------------------------------------------
MOV         YES       EAX                  Register
MOV         YES       10                   Constant
MOV         YES       EBX                  Register
MOV         YES       EAX                  Register
ADD         YES       EAX                  Register
ADD         YES       20                   Constant
MOV         YES       EAX                  Register
MOV         YES       [EBX]               Memory
MOV         YES       EAX                  Register
MOV         YES       [EBX+ESI*4+10]      Memory
JMP         YES       LOOP                 Symbol
XYZ         NO        EAX                  Register
XYZ         NO        EBX                  Register
JNE         YES       START                Symbol
RET         YES       -                    None

============================================================
```

### What the program identifies

For:

```asm
MOV EAX, 10
```

it identifies:

```text
MOV     → Mnemonic
YES     → Exists in opcode table
EAX     → Register
10      → Constant
```

For:

```asm
JMP LOOP
```

it identifies:

```text
JMP     → Mnemonic
YES     → Exists
LOOP    → Symbol
```

For:

```asm
MOV EAX, [EBX+ESI*4+10]
```

it identifies:

```text
MOV                  → Mnemonic
YES                  → Exists
EAX                  → Register
[EBX+ESI*4+10]       → Memory
```

For:

```asm
XYZ EAX, EBX
```

it identifies:

```text
XYZ → Mnemonic
NO  → Not present in opcode table
EAX → Register
EBX → Register
```

