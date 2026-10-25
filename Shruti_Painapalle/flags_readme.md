# IA-32 Flag Register Implementation

This program implements selected flags of the Intel IA-32 EFLAGS register using C++. It calculates the Carry Flag (CF), Zero Flag (ZF), Sign Flag (SF), and Overflow Flag (OF) for 8-bit addition and subtraction operations.

## Compile

```bash
g++ -std=c++17 flags.cpp -o flags
```

## Run

```bash
./flags
```

## Input

The program accepts:

1. **Operation** – Enter `A` for addition or `S` for subtraction.
2. **First number** – An unsigned 8-bit value between 0 and 255.
3. **Second number** – An unsigned 8-bit value between 0 and 255.

## Implemented Flags

- **Carry Flag (CF)** – Indicates an unsigned carry in addition or borrow in subtraction.
- **Zero Flag (ZF)** – Set when the 8-bit result is zero.
- **Sign Flag (SF)** – Indicates whether the most significant bit of the 8-bit result is 1.
- **Overflow Flag (OF)** – Indicates signed arithmetic overflow.

## Output

The program displays the 8-bit result and the calculated values of CF, ZF, SF, and OF.

## Example

shruti\@shruti:\~/Git-Basics/system2/Systems-2-Group-Assignment/Shruti_Painapalle$ ./flags

Choose operation (A for ADD, S for SUB): A

Enter first 8-bit number (0-255): 233

Enter second 8-bit number (0-255): 45

Result (8-bit): 22

Carry Flag (CF): 1

Zero Flag (ZF): 0

Sign Flag (SF): 0

Overflow Flag (OF): 0

Result (8-bit): 22

Carry Flag (CF): 1

Zero Flag (ZF): 0

Sign Flag (SF): 0

Overflow Flag (OF): 0


