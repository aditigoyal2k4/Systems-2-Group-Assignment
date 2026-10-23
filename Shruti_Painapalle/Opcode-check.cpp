#include <iostream>
#include <iomanip>
#include <climits>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
using namespace std;
namespace fs = std::filesystem;

string trim(string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

string upper(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return toupper(c); });
    return s;
}

string clean(string s) {
    size_t p = s.find_first_of(";#");
    if (p != string::npos) s = s.substr(0, p);
    return trim(s);
}

bool pdfToText(string pdf, string txt) {
    string cmd = "pdftotext -layout \"" + pdf + "\" \"" + txt + "\"";
    return system(cmd.c_str()) == 0;
}

struct Opcode {
    string name, operands;
};

struct Symbol {
    int id;
    string name;
    int size;
    int address;
    vector<int> values;
    string stringValue;
    string section;
    string status;
};

vector<Symbol> symbolTable;
bool symbolExists(string name) {
    for (auto &s : symbolTable) {
        if (s.name == name) {
            return true;
        }
    }

    return false;
}
bool addSymbol(int id, string name, int size, int address,
               vector<int>values, string stringValue,
               string section, string status)
{
    if (symbolExists(name)) {
        cout << "Duplicate symbol: " << name << '\n';
        return false;
    }

    Symbol s;
    s.id = id;
    s.name = name;
    s.size = size;
    s.address = address;
    s.values = values;
    s.stringValue = stringValue;
    s.section = section;
    s.status = status;

    symbolTable.push_back(s);
    return true;
}

void displaySymbolTable()
{
    cout << "\n================ SYMBOL TABLE ================\n";

    cout << left
         << setw(15)  << "ID"
         << setw(24) << "Name"
         << setw(19)  << "Size"
         << setw(19) << "Address"
         << setw(24) << "Value"
         << setw(10) << "Section"
         << setw(10) << "Status"
         << '\n';

    for (const auto &s : symbolTable)
    {
        cout << left
             << setw(15)  << s.id
             << setw(24) << s.name
             << setw(19)  << s.size
             << setw(19) << s.address;

        string valuesText;

        if (!s.stringValue.empty())
        {
            valuesText = s.stringValue;
        }
        else if (!s.values.empty())
        {
            for (size_t i = 0; i < s.values.size(); i++)
            {
                if (i > 0)
                    valuesText += ", ";

                valuesText += to_string(s.values[i]);
            }
        }
        else
        {
            valuesText = "-";
        }

        cout << setw(24) << valuesText
             << setw(10) << s.section
             << setw(10) << s.status
             << '\n';
    }

    cout << "==============================================\n";
}


string extractStringValue(string declaration) {
    string result;
    bool insideQuotes = false;

    for (size_t i = 0; i < declaration.size(); i++) {
        char ch = declaration[i];

        if (ch == '"') {
            insideQuotes = !insideQuotes;
            continue;
        }

        if (insideQuotes) {
            result += ch;
        }
    }

    // Join multiple quoted strings with a comma and space.
    // Example: "Hello", "World", 0 -> Hello, World
    string formatted;
    size_t quoteCount = 0;

    for (size_t i = 0; i < declaration.size(); i++) {
        if (declaration[i] == '"') {
            quoteCount++;

            if (quoteCount == 2 && i + 1 < declaration.size()) {
                string remaining = declaration.substr(i + 1);

                if (remaining.find('"') != string::npos) {
                    formatted += ", ";
                    quoteCount = 0;
                }
            }
        }
    }

    if (!formatted.empty()) {
        result.clear();
        insideQuotes = false;
        string current;

        for (char ch : declaration) {
            if (ch == '"') {
                if (insideQuotes) {
                    if (!result.empty()) result += ", ";
                    result += current;
                    current.clear();
                }
                insideQuotes = !insideQuotes;
            } else if (insideQuotes) {
                current += ch;
            }
        }
    }

    return result;
}

void buildSymbolTable(string asmFile) {
    ifstream file(asmFile);

    if (!file.is_open()) {
        cerr << "Error: Cannot open assembly file.\n";
        return;
    }
    string line;
    string section = "";
    int id = 1;
    int dataAddress = 0;
    int bssAddress = 0;

    while (getline(file, line)) {
        line = clean(line);
    
        if (line.empty()) {
            continue;
        }

        string upperLine = upper(line);

        if (upperLine.find("SECTION .DATA") == 0) {
            section = "DATA";
            continue;
        }

        if (upperLine.find("SECTION .BSS") == 0) {
            section = "BSS";
            continue;
        }

        if (upperLine.find("SECTION .TEXT") == 0) {
            section = "TEXT";
            continue;
        }

        if (section == "DATA")
        {
            string name, directive;
            stringstream ss(line);

            ss >> name >> directive;
            directive = upper(directive);

            int elementSize = 0;

            if (directive == "DB")
                elementSize = 1;
            else if (directive == "DW")
                elementSize = 2;
            else if (directive == "DD")
                elementSize = 4;
            else if (directive == "DQ")
                elementSize = 8;

            if (elementSize > 0)
            {
                string declaration;
                getline(ss, declaration);
                declaration = trim(declaration);

                int totalSize = 0;
                vector<int> values;
                string stringValue;

                vector<string> elements;
                string token;
                bool insideQuotes = false;

                // Split elements at commas outside quoted strings.
                for (char ch : declaration)
                {
                    if (ch == '"')
                    {
                        insideQuotes = !insideQuotes;
                        token += ch;
                    }
                    else if (ch == ',' && !insideQuotes)
                    {
                        elements.push_back(trim(token));
                        token.clear();
                    }
                    else
                    {
                        token += ch;
                    }
                }

                if (!token.empty())
                    elements.push_back(trim(token));

                bool valid = !elements.empty();

                for (const string &element : elements)
                {
                    if (element.empty())
                    {
                        valid = false;
                        break;
                    }

                    // Handle quoted strings.
                    if (element.size() >= 2 &&
                        element.front() == '"' &&
                        element.back() == '"')
                    {
                        string text = element.substr(
                            1, element.size() - 2);

                        totalSize += static_cast<int>(text.size());

                        if (!stringValue.empty())
                            stringValue += ", ";

                        stringValue += text;
                    }
                    else
                    {
                        // Handle numeric values.
                        try
                        {
                            long long number = stoll(element, nullptr, 0);

                            if (number < INT_MIN || number > INT_MAX)
                            {
                                valid = false;
                                break;
                            }

                            values.push_back(static_cast<int>(number));
                            totalSize += elementSize;
                        }
                        catch (...)
                        {
                            cerr << "Invalid data value for "
                                 << name << ": " << element << '\n';

                            valid = false;
                            break;
                        }
                    }
                }

                if (valid)
                {
                    int address = dataAddress;

                    if (addSymbol(id, name, totalSize, address,
                                  values, stringValue, "DATA", "DEFINED"))
                    {
                        id++;
                        dataAddress += totalSize;
                    }
                }
            }
        }

        else if (section =="BSS") {
            string name, directive;
            stringstream ss(line);

            ss >> name >> directive;
            directive = upper(directive);

            int size = 0;

            if (directive == "RESB")
                size = 1;
            else if (directive == "RESW")
                size = 2;
            else if (directive == "RESD")
                size = 4;
            else if (directive == "RESQ")
                size = 8;

            if (size > 0) {
                int count = 1;
                ss >> count;

                int totalSize = size * count;
                int address = bssAddress;

                if (addSymbol(id, name, totalSize, address, {}, "",
                                 "BSS", "DEFINED")) {
                    id++;
                bssAddress += totalSize;
                }                
            }
        }

        // Process labels in the TEXT section
        else if (section == "TEXT") {
            size_t pos = line.find(':');

            if (pos != string::npos) {
                string name = trim(line.substr(0, pos));

                if (!name.empty()) {
                    int address = 0;  // Temporary; calculate code addresses later.

                    if (addSymbol(id, name, 0, address, {}, "",
                                  "TEXT", "DEFINED")) {
                        id++;
                    }
                }
            }
        }
    }

    file.close();
}
vector<Opcode> readOpcodes(string file) {
    ifstream f(file);
    vector<Opcode> table;
    string line;

    vector<string> names = {
        "MOV","ADD","SUB","AND","OR","XOR","MUL",
        "CMP","JMP","JNE","CALL","INC","DEC","RET"
    };

    while (getline(f, line)) {
        line = trim(line);
        string u = upper(line);

        for (auto &name : names) {
            if (u.rfind(name, 0) != 0) continue;
            if (u.size() > name.size() &&
                !isspace((unsigned char)u[name.size()])) continue;

            string rest = trim(line.substr(name.size()));
            stringstream ss(rest);
            vector<string> t;
            string x;

            while (ss >> x) t.push_back(x);
            if (t.size() < 2) continue;

            int op = -1;
            regex hex("^(0F|[0-9A-F]{2})$", regex::icase);

            for (int i = 0; i < (int)t.size(); i++) {
                string z = t[i];
                while (!z.empty() && (z.back() == ',' || z.back() == ':'))
                    z.pop_back();

                if (regex_match(z, hex) || upper(z) == "B8+RD") {
                    op = i;
                    break;
                }
            }

            if (op >= 0) {
                string operands;
                for (int i = 0; i < op; i++)
                    operands += (i ? " " : "") + t[i];

                table.push_back({name, trim(operands)});
                break;
            }
        }
    }
    return table;
}

bool exists(const vector<Opcode>& table, string m) {
    for (auto &x : table)
        if (x.name == upper(m)) return true;
    return false;
}

string mnemonic(string line) {
    line = clean(line);
    if (line.empty()) return "";

    size_t p = line.find(':');
    if (p != string::npos) line = trim(line.substr(p + 1));

    string m;
    stringstream(line) >> m;
    m = upper(m);

    vector<string> ignore = {
        "SECTION","SEGMENT","GLOBAL","EXTERN","LABEL","EQU","BITS","ORG"
        ,"EQU","BITS","ORG","END","IF","ELSE","ENDIF","MACRO","ENDM"
    };

    for (auto &x : ignore)
        if (m == x) return "";

    return m;
}

string operands(string line) {
    line = clean(line);

    size_t p = line.find(':');
    if (p != string::npos) line = trim(line.substr(p + 1));

    string m, op;
    stringstream ss(line);
    ss >> m;
    getline(ss, op);
    return trim(op);
}

string type(string op) {
    string u = upper(trim(op));

    vector<string> regs = {
        "EAX","EBX","ECX","EDX","ESI","EDI","EBP","ESP",
        "AX","BX","CX","DX","SI","DI","BP","SP",
        "AL","AH","BL","BH","CL","CH","DL","DH"
    };

    for (auto &r : regs)
        if (u == r) return "REGISTER";

    if (u.find('[') != string::npos) return "MEMORY";

    if (regex_match(u, regex("^-?(0X[0-9A-F]+|[0-9]+)$")))
        return "IMMEDIATE";

    return "LABEL";
}

string operandTypes(string op) {
    if (op.empty()) return "NONE";

    string result, part;
    stringstream ss(op);

    while (getline(ss, part, ',')) {
        if (!result.empty()) result += ", ";
        result += type(part);
    }
    return result;
}

void analyze(string file, const vector<Opcode>& table) {
    ifstream f(file);
    string line;
    string section = "";
    while (getline(f, line)) {
        line = clean(line);
        string upperLine = upper(line);

        if (upperLine.find("SECTION .DATA") == 0) {
            section = "DATA";
            continue;
        }

        if (upperLine.find("SECTION .BSS") == 0) {
            section = "BSS";
            continue;
        }
    

        if (upperLine.find("SECTION .TEXT") == 0) {
            section = "TEXT";
            continue;
        }
        if (section != "TEXT") continue;
        string m =mnemonic(line);
        if (m.empty()) continue;

        string op = operands(line);
        cout << "Mnemonic: " << m << '\n';
        cout << "Exists in opcode table: "
             << (exists(table, m) ? "YES" : "NO") << '\n';
        cout << "Operand used: " << (op.empty() ? "NONE" : op) << '\n';
        cout << "Type of operand: " << operandTypes(op) << '\n';
        cout << "----------------------------------\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./analyzer opcode.pdf program.asm\n";
        return 1;
    }

    string pdf = argv[1], asmFile = argv[2], txt = "temp.txt";

    if (!fs::exists(pdf) || !fs::exists(asmFile)) {
        cerr << "Error: Input file not found.\n";
        return 1;
    }

    if (!pdfToText(pdf, txt)) {
        cerr << "Error: Install pdftotext (poppler-utils).\n";
        return 1;
    }

    auto table = readOpcodes(txt);

    if (table.empty()) {
        cerr << "Error: No opcode found.\n";
        remove(txt.c_str());
        return 1;
    }

    analyze(asmFile, table);
    remove(txt.c_str());

    buildSymbolTable(asmFile);

    displaySymbolTable();
    
    return 0;
}
