#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

string upper(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

string operandType(string op)
{
    op.erase(remove(op.begin(), op.end(), ','), op.end());
    op = upper(op);

    string registers[] = {
        "AX", "BX", "CX", "DX",
        "SI", "DI", "SP", "BP",
        "EAX", "EBX", "ECX", "EDX",
        "ESI", "EDI", "ESP", "EBP"
    };

    for (string reg : registers)
    {
        if (op == reg)
            return "REGISTER";
    }

    if (op.size() >= 2 &&
        op.front() == '[' &&
        op.back() == ']')
    {
        return "MEMORY OPERAND";
    }

    bool number = !op.empty();

    for (char c : op)
    {
        if (!isdigit(c) && c != 'X' &&
            !(c >= 'A' && c <= 'F'))
        {
            number = false;
            break;
        }
    }

    if (number)
        return "CONSTANT";

    return "SYMBOL";
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./analyzer opcode.txt program.asm\n";
        return 1;
    }

    ifstream opcodeFile(argv[1]);
    ifstream asmFile(argv[2]);

    if (!opcodeFile || !asmFile)
    {
        cout << "Error opening file.\n";
        return 1;
    }

    vector<string> opcodes;
    string line;

    while (getline(opcodeFile, line))
    {
        stringstream ss(line);

        string mnemonic, opcode;
        ss >> mnemonic >> opcode;

        if (!mnemonic.empty() && upper(mnemonic) != "MNEMONIC")
            opcodes.push_back(upper(mnemonic));
    }

    while (getline(asmFile, line))
    {
        size_t comment = line.find(';');

        if (comment != string::npos)
            line = line.substr(0, comment);

        stringstream ss(line);

        string mnemonic;
        ss >> mnemonic;

        if (mnemonic.empty())
            continue;

        if (mnemonic.back() == ':')
            continue;

        mnemonic = upper(mnemonic);

        if (mnemonic == "SECTION" ||
            mnemonic == "GLOBAL" ||
            mnemonic == "BITS" ||
            mnemonic == "DB" ||
            mnemonic == "DW" ||
            mnemonic == "DD")
        {
            continue;
        }

        bool found = false;

        for (string op : opcodes)
        {
            if (mnemonic == op)
            {
                found = true;
                break;
            }
        }

        vector<string> operands;
        string operand;

        while (ss >> operand)
        {
            operand.erase(
                remove(operand.begin(), operand.end(), ','),
                operand.end()
            );

            operands.push_back(operand);
        }

        cout << "\nMnemonic: " << mnemonic << endl;

        cout << "Exists in opcode table: "
             << (found ? "YES" : "NO") << endl;

        if (operands.empty())
        {
            cout << "Operand used: None" << endl;
            cout << "Type of operand: None" << endl;
        }
        else
        {
            cout << "Operand used: ";

            for (int i = 0; i < operands.size(); i++)
            {
                cout << operands[i];

                if (i < operands.size() - 1)
                    cout << ", ";
            }

            cout << endl;

            cout << "Type of operand: ";

            for (int i = 0; i < operands.size(); i++)
            {
                cout << operandType(operands[i]);

                if (i < operands.size() - 1)
                    cout << ", ";
            }

            cout << endl;
        }

        cout << "----------------------------------------" << endl;
    }

    opcodeFile.close();
    asmFile.close();

    return 0;
}
