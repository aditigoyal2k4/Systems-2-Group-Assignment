# Assembly to Little - Endian Conversion

This program processes an Assembly program and converts data values into little-endian byte format. It also calculates memory addresses and generates machine-code bytes for supported IA-32 instructions.

## Compile

```bash
g++ -std=c++17 little_endian.cpp -o little_endian
```

## Run

```bash
./little_endian program.asm
```
