#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

#include <string>
#include <cstdint>

using namespace std;


// IA-32 operand types
enum class OperandType
{
    REGISTER,          // r8, r16, r32
    MEMORY,            // memory operand
    REGISTER_OR_MEMORY,// r/m8, r/m16, r/m32
    IMMEDIATE,         // imm8, imm16, imm32
    UNKNOWN
};


// IA-32 operand sizes
enum class OperandSize
{
    BYTE_8,             // 8-bit
    WORD_16,            // 16-bit
    DWORD_32,           // 32-bit
    UNKNOWN
};


// One node = one IA-32 instruction encoding form
struct OpcodeNode
{
    // Mnemonic
    string mnemonic;


    // Destination operand
    OperandType destinationType;
    OperandSize destinationSize;


    // Source operand
    OperandType sourceType;
    OperandSize sourceSize;


    // Opcode bytes
    uint8_t opcode;
    uint8_t opcode2;


    // true for 0F xx opcode
    bool twoByteOpcode;

    bool usesD;
    bool usesS;

    int dBit;
    int sBit;

    // Addressing bytes
    bool requiresModRM;
    bool allowsSIB;


    // Encoding description
    string encodingPattern;


    // Next linked-list node
    OpcodeNode* next;
};


// Insert opcode entry
void insertOpcode(
    OpcodeNode*& head,

    const string& mnemonic,

    OperandType destinationType,
    OperandSize destinationSize,

    OperandType sourceType,
    OperandSize sourceSize,

    uint8_t opcode,
    uint8_t opcode2,

    bool twoByteOpcode,

    bool usesD,
    bool usesS,

    int dBit,
    int sBit,

    bool requiresModRM,
    bool allowsSIB,

    const string& encodingPattern
);


// Search opcode table
OpcodeNode* searchOpcode(
    OpcodeNode* head,

    const string& mnemonic,

    OperandType destinationType,
    OperandSize destinationSize,

    OperandType sourceType,
    OperandSize sourceSize
);


// Display opcode table
void displayOpcodes(OpcodeNode* head);


// Delete opcode table
void deleteTable(OpcodeNode*& head);

// Helper functions for displaying operand information
string operandTypeToString(OperandType type);

string operandSizeToString(OperandSize size);

#endif
