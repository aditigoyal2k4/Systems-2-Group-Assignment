#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>
#include <iomanip>

using namespace std;

string toUpper(string str)
{
    transform(str.begin(), str.end(), str.begin(),
              [](unsigned char c) {
                  return toupper(c);
              });

    return str;
}

string trim(string str)
{
    size_t start = str.find_first_not_of(" \t\r\n");

    if (start == string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");

    return str.substr(start, end - start + 1);
}

bool isRegister(string operand)
{
    operand = toUpper(trim(operand));

    set<string> registers = {
        "EAX", "EBX", "ECX", "EDX",
        "ESI", "EDI", "ESP", "EBP",
        "AX", "BX", "CX", "DX",
        "AL", "BL", "CL", "DL"
    };

    return registers.find(operand) != registers.end();
}

bool isConstant(string operand)
{
    operand = trim(operand);

    if (operand.empty())
        return false;

    if (operand.size() > 2 &&
        operand[0] == '0' &&
        (operand[1] == 'x' || operand[1] == 'X'))
    {
        for (size_t i = 2; i < operand.size(); i++)
        {
            if (!isxdigit(operand[i]))
                return false;
        }

        return true;
    }

    size_t start = 0;

    if (operand[0] == '-')
        start = 1;

    if (start == operand.size())
        return false;

    for (size_t i = start; i < operand.size(); i++)
    {
        if (!isdigit(operand[i]))
            return false;
    }

    return true;
}

bool isMemoryOperand(string operand)
{
    operand = trim(operand);

    if (operand.find('[') != string::npos &&
        operand.find(']') != string::npos)
    {
        return true;
    }

    return false;
}

string getOperandType(string operand)
{
    operand = trim(operand);

    if (operand.empty())
        return "None";

    if (isRegister(operand))
        return "Register";

    if (isConstant(operand))
        return "Constant";

    if (isMemoryOperand(operand))
        return "Memory";

    return "Symbol";
}

vector<string> splitOperands(string operandString)
{
    vector<string> operands;

    stringstream ss(operandString);

    string operand;

    while (getline(ss, operand, ','))
    {
        operand = trim(operand);

        if (!operand.empty())
            operands.push_back(operand);
    }

    return operands;
}

bool parseAssemblyLine(
    string line,
    string &mnemonic,
    string &operands)
{
    
    size_t commentPos = line.find(';');

    if (commentPos != string::npos)
        line = line.substr(0, commentPos);

    line = trim(line);

    if (line.empty())
        return false;

    string upperLine = toUpper(line);

    if (upperLine.find("SECTION") == 0)
        return false;

    size_t colonPos = line.find(':');

    if (colonPos != string::npos)
    {
        line = trim(line.substr(colonPos + 1));

        if (line.empty())
            return false;
    }

    stringstream ss(line);

    ss >> mnemonic;

    getline(ss, operands);

    mnemonic = toUpper(trim(mnemonic));
    operands = trim(operands);

    return !mnemonic.empty();
}

int main()
{
    string opcodeFileName;
    string assemblyFileName;

    cout << "Enter opcode file name: ";
    cin >> opcodeFileName;

    cout << "Enter assembly program file name: ";
    cin >> assemblyFileName;

    ifstream opcodeFile(opcodeFileName);

    if (!opcodeFile)
    {
        cout << "Error: Cannot open opcode file.\n";
        return 1;
    }

    set<string> opcodeTable;

    string line;

    while (getline(opcodeFile, line))
    {
        line = trim(line);

        if (line.empty())
            continue;

        size_t commentPos = line.find(';');

        if (commentPos != string::npos)
            line = line.substr(0, commentPos);

        line = trim(line);

        if (line.empty())
            continue;

        stringstream ss(line);

        string mnemonic;

        ss >> mnemonic;

        if (!mnemonic.empty())
        {
            opcodeTable.insert(toUpper(mnemonic));
        }
    }

    opcodeFile.close();

    ifstream assemblyFile(assemblyFileName);

    if (!assemblyFile)
    {
        cout << "Error: Cannot open assembly file.\n";
        return 1;
    }

    cout << "\n";
    cout << "============================================================\n";
    cout << "             ASSEMBLY PROGRAM ANALYSIS\n";
    cout << "============================================================\n";

    cout << left
         << "\n"
         << "Mnemonic"
         << "\t"
         << "Exists"
         << "\t"
         << "Operand"
         << "\t\t"
         << "Type\n";

    cout << "------------------------------------------------------------\n";


    while (getline(assemblyFile, line))
    {
        string mnemonic;
        string operandString;

        if (!parseAssemblyLine(
                line,
                mnemonic,
                operandString))
        {
            continue;
        }

        bool found =
            opcodeTable.find(mnemonic)
            != opcodeTable.end();

        string exists = found ? "YES" : "NO";

        if (operandString.empty())
        {
            cout << left
                 << setw(12)
                 << mnemonic
                 << setw(10)
                 << exists
                 << setw(20)
                 << "-"
                 << "None"
                 << endl;

            continue;
        }

        vector<string> operands =
            splitOperands(operandString);

        for (string operand : operands)
        {
            string type =
                getOperandType(operand);

            cout << left
                 << setw(12)
                 << mnemonic
                 << setw(10)
                 << exists
                 << setw(20)
                 << operand
                 << type
                 << endl;
        }
    }

    assemblyFile.close();

    cout << "\n============================================================\n";

    return 0;
}
