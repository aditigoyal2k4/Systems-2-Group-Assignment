#include "opcode_table.h"

#include <iostream>
#include <iomanip>

using namespace std;


// Insert a new opcode entry
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
)
{
    OpcodeNode* newNode = new OpcodeNode;


    newNode->mnemonic = mnemonic;


    newNode->destinationType = destinationType;
    newNode->destinationSize = destinationSize;


    newNode->sourceType = sourceType;
    newNode->sourceSize = sourceSize;


    newNode->opcode = opcode;
    newNode->opcode2 = opcode2;


    newNode->twoByteOpcode = twoByteOpcode;

    newNode->usesD = usesD;
    newNode->usesS = usesS;
    newNode->dBit = dBit;
    newNode->sBit = sBit;

    newNode->requiresModRM = requiresModRM;
    newNode->allowsSIB = allowsSIB;


    newNode->encodingPattern = encodingPattern;


    newNode->next = nullptr;


    // Empty list
    if (head == nullptr)
    {
        head = newNode;
        return;
    }


    // Find last node
    OpcodeNode* temp = head;

    while (temp->next != nullptr)
    {
        temp = temp->next;
    }


    temp->next = newNode;
}


// Search opcode table
OpcodeNode* searchOpcode(
    OpcodeNode* head,

    const string& mnemonic,

    OperandType destinationType,
    OperandSize destinationSize,

    OperandType sourceType,
    OperandSize sourceSize
)
{
    OpcodeNode* temp = head;


    while (temp != nullptr)
    {
        if (
            temp->mnemonic == mnemonic &&

            temp->destinationType == destinationType &&
            temp->destinationSize == destinationSize &&

            temp->sourceType == sourceType &&
            temp->sourceSize == sourceSize
        )
        {
            return temp;
        }


        temp = temp->next;
    }


    return nullptr;
}


// Convert operand type to text
string operandTypeToString(OperandType type)
{
    switch (type)
    {
        case OperandType::REGISTER:
            return "REGISTER";

        case OperandType::MEMORY:
            return "MEMORY";

        case OperandType::REGISTER_OR_MEMORY:
            return "REGISTER/MEMORY";

        case OperandType::IMMEDIATE:
            return "IMMEDIATE";

        default:
            return "UNKNOWN";
    }
}


// Convert operand size to text
string operandSizeToString(OperandSize size)
{
    switch (size)
    {
        case OperandSize::BYTE_8:
            return "8-bit";

        case OperandSize::WORD_16:
            return "16-bit";

        case OperandSize::DWORD_32:
            return "32-bit";

        default:
            return "UNKNOWN";
    }
}


// Display opcode table
void displayOpcodes(OpcodeNode* head)
{
    OpcodeNode* temp = head;


    while (temp != nullptr)
    {
        cout << "Mnemonic : "
             << temp->mnemonic << endl;


        cout << "Destination : "
             << operandTypeToString(temp->destinationType)
             << " ("
             << operandSizeToString(temp->destinationSize)
             << ")"
             << endl;


        cout << "Source      : "
             << operandTypeToString(temp->sourceType)
             << " ("
             << operandSizeToString(temp->sourceSize)
             << ")"
             << endl;


        cout << "Opcode      : 0x"
             << hex
             << setw(2)
             << setfill('0')
             << static_cast<int>(temp->opcode);


        if (temp->twoByteOpcode)
        {
            cout << " 0x"
                 << setw(2)
                 << static_cast<int>(temp->opcode2);
        }


        cout << dec << endl;


        cout << "d bit       : ";

	if (temp->usesD)
	{
    		cout << temp->dBit;
	}
	else
	{
    		cout << "not used";
	}

	cout << endl;

        cout << "s bit       : ";

	if (temp->usesS)
	{
    		cout << temp->sBit;
	}
	else
	{
    		cout << "not used";
	}

	cout << endl;

        cout << "ModR/M      : "
             << (temp->requiresModRM ? "required" : "not required")
             << endl;


        cout << "SIB         : "
             << (temp->allowsSIB ? "possible" : "not used")
             << endl;


        cout << "Encoding    : "
             << temp->encodingPattern
             << endl;


        cout << "----------------------------------------"
             << endl;


        temp = temp->next;
    }
}


// Delete linked list
void deleteTable(OpcodeNode*& head)
{
    while (head != nullptr)
    {
        OpcodeNode* temp = head;

        head = head->next;

        delete temp;
    }
}
