#include "opcode_table.h"

#include <iostream>
#include <string>

using namespace std;

// ---------------------------------------------------------
// Opcode selection
// ---------------------------------------------------------
//
// The assembler uses:
//
//     mnemonic
//     destination operand type
//     destination operand size
//     source operand type
//     source operand size
//
// to search the linked-list opcode table.
//
// ---------------------------------------------------------

OpcodeNode* selectOpcode(
    OpcodeNode* head,

    const string& mnemonic,

    OperandType destinationType,
    OperandSize destinationSize,

    OperandType sourceType,
    OperandSize sourceSize
)
{
    return searchOpcode(
        head,
        mnemonic,

        destinationType,
        destinationSize,

        sourceType,
        sourceSize
    );
}


// Display selected opcode
void displaySelectedOpcode(OpcodeNode* result)
{
    if (result == nullptr)
    {
        cout << "No matching opcode found." << endl;
        return;
    }


    cout << endl;
    cout << "Opcode selection successful"
         << endl;

    cout << "-----------------------------"
         << endl;


    cout << "Mnemonic : "
         << result->mnemonic
         << endl;


    cout << "Destination : "
         << operandTypeToString(result->destinationType)
         << ", "
         << operandSizeToString(result->destinationSize)
         << endl;


    cout << "Source      : "
         << operandTypeToString(result->sourceType)
         << ", "
         << operandSizeToString(result->sourceSize)
         << endl;


    cout << "Opcode      : 0x"
         << hex
         << static_cast<int>(result->opcode);


    if (result->twoByteOpcode)
    {
        cout << " 0x"
             << static_cast<int>(result->opcode2);
    }


    cout << dec << endl;


    cout << "Encoding    : "
         << result->encodingPattern
         << endl;


    cout << "d bit       : "
         << (result->usesD ? "used" : "not used")
         << endl;


    cout << "s bit       : "
         << (result->usesS ? "used" : "not used")
         << endl;


    cout << "ModR/M      : "
         << (result->requiresModRM ? "required" : "not required")
         << endl;
}


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main()
{
    OpcodeNode* head = nullptr;


    // =====================================================
    // MOV r/m8, r8
    // Opcode: 88 /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::BYTE_8,

        OperandType::REGISTER,
        OperandSize::BYTE_8,

        0x88,
        0x00,

        false,

        true,
        false,
	
	0,  //dBit
	0,  //sBit

        true,
        true,

        "1000100w /r"
    );


    // =====================================================
    // MOV r8, r/m8
    // Opcode: 8A /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::BYTE_8,

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::BYTE_8,

        0x8A,
        0x00,

        false,

        true,
        false,
	
	1, //dBit
	0, //sBit

        true,
        true,

        "1000101w /r"
    );


    // =====================================================
    // MOV r/m16, r16
    // Opcode: 89 /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::WORD_16,

        OperandType::REGISTER,
        OperandSize::WORD_16,

        0x89,
        0x00,

        false,

        true,
        false,
	
	0,  //dBit
	0, //sBit

        true,
        true,

        "1000100w /r"
    );


    // =====================================================
    // MOV r16, r/m16
    // Opcode: 8B /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::WORD_16,

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::WORD_16,

        0x8B,
        0x00,

        false,

        true,
        false,
	
	1,  //dBit
	0,  //sBit

        true,
        true,

        "1000101w /r"
    );


    // =====================================================
    // MOV r/m32, r32
    // Opcode: 89 /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::DWORD_32,

        OperandType::REGISTER,
        OperandSize::DWORD_32,

        0x89,
        0x00,

        false,

        true,
        false,
	
	0, //dBit
	0,  //sBit

        true,
        true,

        "1000100w /r"
    );


    // =====================================================
    // MOV r32, r/m32
    // Opcode: 8B /r
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::DWORD_32,

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::DWORD_32,

        0x8B,
        0x00,

        false,

        true,
        false,
	
	1, //dBit
	0,  //sBit
        
	true,
        true,

        "1000101w /r"
    );


    // =====================================================
    // MOV r8, imm8
    // Opcode: B0 + rb
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::BYTE_8,

        OperandType::IMMEDIATE,
        OperandSize::BYTE_8,

        0xB0,
        0x00,

        false,

        false,
        false,
	
	0,  //dBit
	0,  //sBit

        false,
        false,

        "1011 0reg / ib"
    );


    // =====================================================
    // MOV r16, imm16
    // Opcode: B8 + rw
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::WORD_16,

        OperandType::IMMEDIATE,
        OperandSize::WORD_16,

        0xB8,
        0x00,

        false,

        false,
        false,
	
	0,  //dBit
	0,  //sBit
        false,
        false,

        "1011 1reg / iw"
    );


    // =====================================================
    // MOV r32, imm32
    // Opcode: B8 + rd
    // =====================================================

    insertOpcode(
        head,
        "MOV",

        OperandType::REGISTER,
        OperandSize::DWORD_32,

        OperandType::IMMEDIATE,
        OperandSize::DWORD_32,

        0xB8,
        0x00,

        false,

        false,
        false,

	0,  //dBit
	0,  //sBit

        false,
        false,

        "1011 1reg / id"
    );


    // =====================================================
    // IMUL r32, r/m32
    // Opcode: 0F AF /r
    // =====================================================

    insertOpcode(
        head,
        "IMUL",

        OperandType::REGISTER,
        OperandSize::DWORD_32,

        OperandType::REGISTER_OR_MEMORY,
        OperandSize::DWORD_32,

        0x0F,
        0xAF,

        true,

        false,
        false,
	
	0,  //dBit
	0,  //sBit

        true,
        true,

        "0F AF /r"
    );


    // =====================================================
    // Display table
    // =====================================================

    cout << "=========================================="
         << endl;

    cout << "IA-32 OPCODE LOOKUP TABLE"
         << endl;

    cout << "=========================================="
         << endl << endl;

    displayOpcodes(head);


    // =====================================================
    // Test 1
    //
    // MOV EAX, EBX
    //
    // REGISTER,32 -> REGISTER,32
    //
    // Matching encoding:
    //
    // MOV r32,r/m32
    // Opcode = 8B /r
    // =====================================================

    cout << endl;
    cout << "TEST 1: MOV EAX, EBX"
         << endl;


    OpcodeNode* result1 =
        selectOpcode(
            head,
            "MOV",

            OperandType::REGISTER,
            OperandSize::DWORD_32,

            OperandType::REGISTER_OR_MEMORY,
            OperandSize::DWORD_32
        );


    displaySelectedOpcode(result1);


    // =====================================================
    // Test 2
    //
    // MOV AL, BL
    //
    // 8-bit form
    // =====================================================

    cout << endl;
    cout << "TEST 2: MOV AL, BL"
         << endl;


    OpcodeNode* result2 =
        selectOpcode(
            head,
            "MOV",

            OperandType::REGISTER,
            OperandSize::BYTE_8,

            OperandType::REGISTER_OR_MEMORY,
            OperandSize::BYTE_8
        );


    displaySelectedOpcode(result2);


    // =====================================================
    // Test 3
    //
    // MOV EAX, immediate
    //
    // 32-bit immediate form
    // =====================================================

    cout << endl;
    cout << "TEST 3: MOV EAX, 100"
         << endl;


    OpcodeNode* result3 =
        selectOpcode(
            head,
            "MOV",

            OperandType::REGISTER,
            OperandSize::DWORD_32,

            OperandType::IMMEDIATE,
            OperandSize::DWORD_32
        );


    displaySelectedOpcode(result3);


    // =====================================================
    // Test 4
    //
    // IMUL EAX, EBX
    //
    // 0F AF /r
    // =====================================================

    cout << endl;
    cout << "TEST 4: IMUL EAX, EBX"
         << endl;


    OpcodeNode* result4 =
        selectOpcode(
            head,
            "IMUL",

            OperandType::REGISTER,
            OperandSize::DWORD_32,

            OperandType::REGISTER_OR_MEMORY,
            OperandSize::DWORD_32
        );


    displaySelectedOpcode(result4);


    // Free linked-list memory

    deleteTable(head);

    return 0;
}
