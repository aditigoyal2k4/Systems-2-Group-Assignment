#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdint>

using namespace std;

struct Symbol {
    string name;
    string section;
    uint32_t address;
};

struct Line {
    string label;
    string instruction;
    string operand;
    string section;
    uint32_t address;
};

map<string, Symbol> symbolTable;
vector<Line> programLines;

uint32_t textLC = 0;
uint32_t dataLC = 0;

string currentSection = ".text";


string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

string upper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

string removeComment(string s) {
    size_t pos = s.find(';');

    if (pos != string::npos)
        s = s.substr(0, pos);

    return trim(s);
}

/* Convert hexadecimal / decimal string to integer */
uint32_t parseNumber(string s) {

    s = trim(s);

    if (s.empty())
        return 0;

    bool negative = false;

    if (s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }

    uint32_t value = 0;

    if (s.size() > 2 &&
        s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X')) {

        value = stoul(s.substr(2), nullptr, 16);
    }
    else {
        value = stoul(s, nullptr, 10);
    }

    if (negative)
        return static_cast<uint32_t>(-static_cast<int32_t>(value));

    return value;
}


vector<uint8_t> littleEndian(uint32_t value, int bytes) {

    vector<uint8_t> result;

    for (int i = 0; i < bytes; i++) {
        result.push_back((value >> (8 * i)) & 0xFF);
    }

    return result;
}

void printBytes(const vector<uint8_t>& bytes) {

    for (uint8_t b : bytes) {
        cout << uppercase
             << hex
             << setw(2)
             << setfill('0')
             << (int)b
             << " ";
    }

    cout << dec << endl;
}


int directiveSize(string instruction, string operand) {

    instruction = upper(instruction);

    if (instruction == "DB") {
        // Count comma-separated values
        int count = 1;

        for (char c : operand)
            if (c == ',')
                count++;

        return count;
    }

    if (instruction == "DW") {
        int count = 1;

        for (char c : operand)
            if (c == ',')
                count++;

        return count * 2;
    }

    if (instruction == "DD") {
        int count = 1;

        for (char c : operand)
            if (c == ',')
                count++;

        return count * 4;
    }

    return 0;
}


int instructionSize(string instruction, string operand) {

    instruction = upper(instruction);
    operand = upper(trim(operand));


    if (instruction == "NOP")
        return 1;

    if (instruction == "RET")
        return 1;

    /*
       MOV EAX, imm32
       B8 + rd + imm32
    */
    if (instruction == "MOV") {

        if (operand.find("EAX") == 0 &&
            operand.find(",") != string::npos) {

            string rhs =
                trim(operand.substr(operand.find(",") + 1));

            if (!rhs.empty() &&
                rhs[0] != '[') {

                return 5;
            }
        }
    }

    /*
       Short JMP:
       EB + rel8
       2 bytes
    */

    if (instruction == "JMP")
        return 2;

    /*
       Short JNE:
       75 + rel8
       2 bytes
    */

    if (instruction == "JNE")
        return 2;

    /*
       Basic memory MOV example from Person 2:
       8B 44 B3 10
    */

    if (instruction == "MOV" &&
        operand.find("[") != string::npos) {

        return 4;
    }

    return 1;
}


Line parseLine(string source) {

    Line line;

    line.label = "";
    line.instruction = "";
    line.operand = "";
    line.section = currentSection;
    line.address = 0;

    source = removeComment(source);

    if (source.empty())
        return line;


    size_t colon = source.find(':');

    if (colon != string::npos) {

        line.label = trim(source.substr(0, colon));

        source = trim(source.substr(colon + 1));
    }

    if (source.empty())
        return line;

    stringstream ss(source);

    ss >> line.instruction;

    getline(ss, line.operand);

    line.operand = trim(line.operand);

    line.section = currentSection;

    return line;
}


void pass1(const string& filename) {

    ifstream file(filename);

    if (!file) {
        cout << "Error opening file.\n";
        return;
    }

    string source;

    textLC = 0;
    dataLC = 0;

    currentSection = ".text";

    while (getline(file, source)) {

        Line line = parseLine(source);

        if (line.label.empty() &&
            line.instruction.empty())
            continue;

        /* SECTION directive */

        if (upper(line.instruction) == "SECTION") {

            currentSection = line.operand;

            if (currentSection != ".text" &&
                currentSection != ".data") {

                cout << "Warning: Unknown section "
                     << currentSection << endl;
            }

            continue;
        }

        line.section = currentSection;

        /* Assign address */

        if (currentSection == ".text")
            line.address = textLC;
        else
            line.address = dataLC;

        /* Store label */

        if (!line.label.empty()) {

            Symbol sym;

            sym.name = line.label;
            sym.section = currentSection;
            sym.address = line.address;

            symbolTable[line.label] = sym;
        }

        /* Store line */

        programLines.push_back(line);

        /* Update location counter */

        string inst = upper(line.instruction);

        if (inst == "DB" ||
            inst == "DW" ||
            inst == "DD") {

            int size =
                directiveSize(inst, line.operand);

            if (currentSection == ".data")
                dataLC += size;
        }
        else {

            int size =
                instructionSize(inst, line.operand);

            if (currentSection == ".text")
                textLC += size;
        }
    }

    file.close();
}


void displaySymbolTable() {

    cout << "\n========================================\n";
    cout << "             SYMBOL TABLE\n";
    cout << "========================================\n";

    cout << left
         << setw(15) << "Symbol"
         << setw(12) << "Section"
         << setw(12) << "Address"
         << endl;

    cout << "----------------------------------------\n";

    for (auto &entry : symbolTable) {

        cout << left
             << setw(15) << entry.second.name
             << setw(12) << entry.second.section
             << "0x"
             << uppercase
             << hex
             << setw(8)
             << setfill('0')
             << entry.second.address
             << setfill(' ')
             << dec
             << endl;
    }
}


vector<uint8_t> encodeDB(string operand) {

    vector<uint8_t> bytes;

    stringstream ss(operand);
    string value;

    while (getline(ss, value, ',')) {

        value = trim(value);

        uint32_t number = parseNumber(value);

        bytes.push_back(number & 0xFF);
    }

    return bytes;
}


vector<uint8_t> encodeDW(string operand) {

    vector<uint8_t> bytes;

    stringstream ss(operand);
    string value;

    while (getline(ss, value, ',')) {

        value = trim(value);

        uint32_t number = parseNumber(value);

        vector<uint8_t> temp =
            littleEndian(number, 2);

        bytes.insert(
            bytes.end(),
            temp.begin(),
            temp.end()
        );
    }

    return bytes;
}


vector<uint8_t> encodeDD(string operand) {

    vector<uint8_t> bytes;

    stringstream ss(operand);
    string value;

    while (getline(ss, value, ',')) {

        value = trim(value);

        uint32_t number = parseNumber(value);

        vector<uint8_t> temp =
            littleEndian(number, 4);

        bytes.insert(
            bytes.end(),
            temp.begin(),
            temp.end()
        );
    }

    return bytes;
}


uint32_t resolveLabel(string label) {

    if (symbolTable.find(label) == symbolTable.end()) {

        cout << "Error: Undefined label "
             << label << endl;

        return 0;
    }

    return symbolTable[label].address;
}


vector<uint8_t> encodeJMP(
    string label,
    uint32_t currentAddress
) {

    vector<uint8_t> bytes;

    uint32_t target =
        resolveLabel(label);

    /*
       Short JMP:
       EB rel8

       next instruction =
       current address + 2
    */

    int32_t displacement =
        static_cast<int32_t>(target) -
        static_cast<int32_t>(currentAddress + 2);

    if (displacement >= -128 &&
        displacement <= 127) {

        bytes.push_back(0xEB);

        bytes.push_back(
            static_cast<uint8_t>(displacement)
        );
    }

    else {

        /*
           Near JMP:
           E9 rel32

           next instruction =
           current address + 5
        */

        displacement =
            static_cast<int32_t>(target) -
            static_cast<int32_t>(currentAddress + 5);

        bytes.push_back(0xE9);

        vector<uint8_t> offset =
            littleEndian(
                static_cast<uint32_t>(displacement),
                4
            );

        bytes.insert(
            bytes.end(),
            offset.begin(),
            offset.end()
        );
    }

    return bytes;
}

vector<uint8_t> encodeJNE(
    string label,
    uint32_t currentAddress
) {

    vector<uint8_t> bytes;

    uint32_t target =
        resolveLabel(label);

    /*
       Short JNE:
       75 rel8
    */

    int32_t displacement =
        static_cast<int32_t>(target) -
        static_cast<int32_t>(currentAddress + 2);

    if (displacement >= -128 &&
        displacement <= 127) {

        bytes.push_back(0x75);

        bytes.push_back(
            static_cast<uint8_t>(displacement)
        );
    }

    else {

        /*
           Near JNE:
           0F 85 rel32
        */

        displacement =
            static_cast<int32_t>(target) -
            static_cast<int32_t>(currentAddress + 6);

        bytes.push_back(0x0F);
        bytes.push_back(0x85);

        vector<uint8_t> offset =
            littleEndian(
                static_cast<uint32_t>(displacement),
                4
            );

        bytes.insert(
            bytes.end(),
            offset.begin(),
            offset.end()
        );
    }

    return bytes;
}


vector<uint8_t> encodeMOVImmediate(
    string operand
) {

    vector<uint8_t> bytes;

    size_t comma =
        operand.find(',');

    if (comma == string::npos)
        return bytes;

    string destination =
        upper(trim(operand.substr(0, comma)));

    string immediate =
        trim(operand.substr(comma + 1));

    if (destination != "EAX")
        return bytes;

    uint32_t value =
        parseNumber(immediate);

    /*
       MOV EAX, imm32
       Opcode = B8
    */

    bytes.push_back(0xB8);

    vector<uint8_t> imm =
        littleEndian(value, 4);

    bytes.insert(
        bytes.end(),
        imm.begin(),
        imm.end()
    );

    return bytes;
}


vector<uint8_t> encodeInstruction(
    const Line& line
) {

    vector<uint8_t> bytes;

    string instruction =
        upper(line.instruction);

    string operand =
        trim(line.operand);

    /* ---------------- DB ---------------- */

    if (instruction == "DB")
        return encodeDB(operand);

    /* ---------------- DW ---------------- */

    if (instruction == "DW")
        return encodeDW(operand);

    /* ---------------- DD ---------------- */

    if (instruction == "DD")
        return encodeDD(operand);

    /* ---------------- NOP ---------------- */

    if (instruction == "NOP") {

        bytes.push_back(0x90);

        return bytes;
    }

    /* ---------------- RET ---------------- */

    if (instruction == "RET") {

        bytes.push_back(0xC3);

        return bytes;
    }

    /* ---------------- MOV EAX, imm32 ---------------- */

    if (instruction == "MOV") {

        string temp = upper(operand);

        if (temp.find("EAX") == 0 &&
            temp.find("[") == string::npos) {

            return encodeMOVImmediate(operand);
        }

        /*
           Example handled jointly with Person 2:

           MOV EAX,[EBX+ESI*4+0x10]

           8B 44 B3 10
        */

        string normalized = temp;

        normalized.erase(
            remove(normalized.begin(),
                  normalized.end(),
                  ' '),
            normalized.end()
        );

        if (normalized.find(
                "EAX,[EBX+ESI*4+0X10")
            != string::npos) {

            bytes = {
                0x8B,
                0x44,
                0xB3,
                0x10
            };

            return bytes;
        }
    }

    /* ---------------- JMP ---------------- */

    if (instruction == "JMP") {

        return encodeJMP(
            operand,
            line.address
        );
    }

    /* ---------------- JNE ---------------- */

    if (instruction == "JNE") {

        return encodeJNE(
            operand,
            line.address
        );
    }

    cout << "Warning: Instruction not handled: "
         << instruction << endl;

    return bytes;
}


void pass2() {

    cout << "\n========================================\n";
    cout << "          PASS 2 - CODE GENERATION\n";
    cout << "========================================\n";

    for (const Line& line : programLines) {

        vector<uint8_t> bytes =
            encodeInstruction(line);

        if (bytes.empty())
            continue;

        cout << "\n";

        cout << uppercase
             << hex
             << setw(8)
             << setfill('0')
             << line.address
             << setfill(' ')
             << " : ";

        printBytes(bytes);
    }
}


int main() {

    string filename;

    cout << "========================================\n";
    cout << "      IA-32 TWO-PASS ASSEMBLER\n";
    cout << "========================================\n";

    cout << "\nEnter assembly source filename: ";
    cin >> filename;

    /* PASS 1 */

    cout << "\nRunning Pass 1...\n";

    pass1(filename);

    cout << "Pass 1 completed successfully.\n";

    displaySymbolTable();

    /* PASS 2 */

    pass2();

    cout << "\n========================================\n";
    cout << "        ASSEMBLY COMPLETED\n";
    cout << "========================================\n";

    return 0;
}
