#include <iostream>
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
        "SECTION","SEGMENT","GLOBAL","EXTERN",
        "DB","DW","DD","EQU","BITS","ORG"
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

    while (getline(f, line)) {
        string m = mnemonic(line);
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
}
