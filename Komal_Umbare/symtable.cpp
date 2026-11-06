#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

struct Symbol
{
    int id;
    string name;
    string section;
    int size;
    int address;
    string value;
};

struct Opcode
{
    string code;
    int size;
};

string trim(string str)
{
    size_t start = str.find_first_not_of(" \t\r\n");

    if (start == string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");

    return str.substr(start, end - start + 1);
}

string toUpper(string str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./symbol <opcode_file> <assembly_file>\n";
        return 1;
    }


    ifstream opcodeFile(argv[1]);

    if (!opcodeFile)
    {
        cout << "Unable to open opcode file!\n";
        return 1;
    }

    map<string, Opcode> opcodeTable;

    string mnemonic, code;
    int instructionSize;

    while (opcodeFile >> mnemonic >> code >> instructionSize)
    {
        opcodeTable[toUpper(mnemonic)] = {code, instructionSize};
    }

    opcodeFile.close();


    ifstream asmFile(argv[2]);

    if (!asmFile)
    {
        cout << "Unable to open assembly file!\n";
        return 1;
    }

    vector<Symbol> symbolTable;

    string line;
    string section = "";

    int dataAddress = 0;
    int bssAddress = 0;
    int textAddress = 0;

    while (getline(asmFile, line))
    {

        size_t comment = line.find(';');

        if (comment != string::npos)
            line = line.substr(0, comment);

        line = trim(line);

        if (line.empty())
            continue;


        if (line.find("section .data") == 0)
        {
            section = ".data";
            continue;
        }

        if (line.find("section .bss") == 0)
        {
            section = ".bss";
            continue;
        }

        if (line.find("section .text") == 0)
        {
            section = ".text";
            continue;
        }

        stringstream ss(line);
        string name, directive, value;

        ss >> name;


        if (name == "global" || name == "extern")
            continue;


        if (section == ".data" || section == ".bss")
        {
            ss >> directive;
            getline(ss, value);

            value = trim(value);

            int unitSize = 0;

            if (directive == "db" || directive == "resb")
                unitSize = 1;
            else if (directive == "dw" || directive == "resw")
                unitSize = 2;
            else if (directive == "dd" || directive == "resd")
                unitSize = 4;
            else if (directive == "dq" || directive == "resq")
                unitSize = 8;
            else
                continue;


            int elements = 1;

            if (section == ".data")
            {
                for (char ch : value)
                {
                    if (ch == ',')
                        elements++;
                }
            }
            else
            {
                elements = stoi(value);
            }

            int totalSize = unitSize * elements;

            int address;

            if (section == ".data")
            {
                address = dataAddress;
                dataAddress += totalSize;
            }
            else
            {
                address = bssAddress;
                bssAddress += totalSize;
            }

            Symbol s;

            s.id = symbolTable.size() + 1;
            s.name = name;
            s.section = section;
            s.size = totalSize;
            s.address = address;
            s.value = (section == ".data") ? value : "-";

            symbolTable.push_back(s);
        }


        else if (section == ".text")
        {
            string label = "";


            size_t colon = line.find(':');

            if (colon != string::npos)
            {
                label = trim(line.substr(0, colon));

                Symbol s;

                s.id = symbolTable.size() + 1;
                s.name = label;
                s.section = ".text";
                s.size = 0;
                s.address = textAddress;
                s.value = "-";

                symbolTable.push_back(s);


                line = trim(line.substr(colon + 1));

                if (line.empty())
                    continue;
            }


            stringstream instruction(line);

            string operation;
            instruction >> operation;

            operation = toUpper(operation);


            if (opcodeTable.find(operation) != opcodeTable.end())
            {
                textAddress += opcodeTable[operation].size;
            }
        }
    }

    asmFile.close();


    cout << "\n\t\t\tSYMBOL TABLE\n\n";

    cout << left
         << setw(6) << "#id"
         << setw(15) << "Name"
         << setw(12) << "Section"
         << setw(10) << "Size"
         << setw(12) << "Address"
         << setw(12) << "Value" << endl;

    cout << string(67, '-') << endl;

    for (const auto &s : symbolTable)
    {
        cout << left
             << setw(6) << s.id
             << setw(15) << s.name
             << setw(12) << s.section
             << setw(10) << s.size
             << "0x" << hex << uppercase << s.address
             << dec << nouppercase
             << setw(8) << ""
             << setw(12) << s.value
             << endl;
    }

    cout << string(67, '-') << endl;

    cout << "\nTotal symbols: " << symbolTable.size() << endl;

    return 0;
}
