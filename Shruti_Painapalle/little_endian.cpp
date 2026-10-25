#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

string trim(string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string lower(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return tolower(c); });
    return s;
}

vector<string> split(string s) {
    vector<string> v;
    string t;
    bool quote = false;
    for (char c : s) {
        if (c == '"') quote = !quote;
        if (c == ',' && !quote) {
            v.push_back(trim(t));
            t.clear();
        } else t += c;
    }
    if (!trim(t).empty()) v.push_back(trim(t));
    return v;
}

unsigned long value(string s) {
    s = trim(s);

    if (s.empty())
        return 0;

    try {
        size_t pos = 0;
        int base = 10;

        if (s.back() == 'h' || s.back() == 'H') {
            s.pop_back();
            base = 16;
        } else if (s.size() > 2 &&
                   s[0] == '0' &&
                   (s[1] == 'x' || s[1] == 'X')) {
            base = 16;
        }

        unsigned long result = stoul(s, &pos, base);

        if (pos != s.size()) {
            cerr << "Invalid numeric value: " << s << '\n';
            return 0;
        }

        return result;
    } catch (const exception&) {
        cerr << "Invalid numeric value: " << s << '\n';
        return 0;
    }
}
void addLE(vector<unsigned char>& b, unsigned long n, int size) {
    for (int i = 0; i < size; i++)
        b.push_back((n >> (8 * i)) & 255);
}

void show(int addr, vector<unsigned char> b, string text) {
    cout << hex << uppercase << setfill('0')
         << setw(4) << addr << "    ";
    for (auto x : b)
        cout << setw(2) << (int)x << ' ';
    cout << "    " << text << '\n' << dec;
}

int main(int argc, char* argv[]) {
    ifstream f(argc > 1 ? argv[1] : "input.asm");
    if (!f) {
        cout << "Cannot open input.asm\n";
        return 1;
    }

    map<string, int> symbols, labels;
    vector<string> data, bss, code;
    string line, section;

    while (getline(f, line)) {
        size_t c = line.find(';');
        if (c != string::npos) line.erase(c);
        line = trim(line);
        if (line.empty()) continue;

        string l = lower(line);
        if (l.find("section .data") == 0) { section = "data"; continue; }
        if (l.find("section .bss") == 0)  { section = "bss"; continue; }
        if (l.find("section .text") == 0) { section = "text"; continue; }
        if (l.find("global ") == 0) continue;

        if (section == "data") data.push_back(line);
        else if (section == "bss") bss.push_back(line);
        else if (section == "text") code.push_back(line);
    }

    int da = 0, ba = 0, ca = 0;

    // Pass 1: calculate data and BSS addresses.
    for (string s : data) {
        string name, dir, rest;
        stringstream ss(s);
        ss >> name >> dir;
        getline(ss, rest);
        dir = lower(dir);
        rest = trim(rest);
        symbols[name] = da;

        if (dir == "db") {
            if (!rest.empty() && rest[0] == '"')
                da += (int)rest.size() - 2;
            else da += (int)split(rest).size();
        } else if (dir == "dw") da += 2 * split(rest).size();
        else if (dir == "dd") da += 4 * split(rest).size();
        else if (dir == "dq") da += 8 * split(rest).size();
    }

    for (string s : bss) {
        string name, dir, rest;
        stringstream ss(s);
        ss >> name >> dir >> rest;
        dir = lower(dir);
        symbols[name] = ba;
        int n = stoi(rest);
        if (dir == "resb") ba += n;
        else if (dir == "resw") ba += 2 * n;
        else if (dir == "resd") ba += 4 * n;
        else if (dir == "resq") ba += 8 * n;
    }

    // Pass 1: calculate instruction addresses and labels.
    for (string s : code) {
        size_t colon = s.find(':');
        if (colon != string::npos) {
            labels[trim(s.substr(0, colon))] = ca;
            s = trim(s.substr(colon + 1));
            if (s.empty()) continue;
        }

        string op, args;
        stringstream ss(s);
        ss >> op;
        getline(ss, args);
        op = lower(op);
        args = lower(trim(args));
        int n = 0;

        if (op == "mov" &&
            (args == "ecx, 5" || args == "eax, 0" ||
             args.find("esi,") == 0)) n = 5;
        else if (op == "add" || op == "xor" ||
                 op == "and" || op == "or") n = 2;
        else if (op == "ret") n = 1;

        ca += n;
    }

    cout << "\n========== DATA SECTION ==========\n";
    cout << "Address    Machine Code       Declaration\n";
    da = 0;

    for (string s : data) {
        string name, dir, rest;
        stringstream ss(s);
        ss >> name >> dir;
        getline(ss, rest);
        dir = lower(dir);
        rest = trim(rest);
        vector<unsigned char> bytes;

        if (dir == "db") {
            for (string x : split(rest)) {
                if (x.size() >= 2 && x.front() == '"' && x.back() == '"') {
                    string str = x.substr(1, x.size() - 2);
                    for (size_t i = 0; i < str.size(); i++) {
                        if (str[i] == '\\' && i + 1 < str.size()) {
                            char next = str[++i];
                            bytes.push_back(next == 'n' ? 10 :
                                            next == 't' ? 9 : next);
                        } else bytes.push_back(str[i]);
                    }
                } else addLE(bytes, value(x), 1);
            }
        } else {
            int size = dir == "dw" ? 2 : dir == "dd" ? 4 : 8;
            for (string x : split(rest))
                addLE(bytes, value(x), size);
        }

        show(da, bytes, s);
        da += bytes.size();
    }

    cout << "\n========== BSS SECTION ==========\n";
    cout << "Address    Size(bytes)    Declaration\n";
    ba = 0;

    for (string s : bss) {
        string name, dir, rest;
        stringstream ss(s);
        ss >> name >> dir >> rest;
        dir = lower(dir);
        int n = stoi(rest);
        int size = dir == "resb" ? n : dir == "resw" ? n * 2 :
                   dir == "resd" ? n * 4 : n * 8;

        cout << hex << uppercase << setw(4) << setfill('0') << ba
             << dec << "       " << size << "             " << s << '\n';
        ba += size;
    }

    cout << "\n========== TEXT SECTION ==========\n";
    cout << "Address    Machine Code       Instruction\n";
    ca = 0;

    for (string s : code) {
        size_t colon = s.find(':');
        if (colon != string::npos) {
            s = trim(s.substr(colon + 1));
            if (s.empty()) continue;
        }

        string op, args;
        stringstream ss(s);
        ss >> op;
        getline(ss, args);
        op = lower(op);
        args = lower(trim(args));
        vector<unsigned char> bytes;

        if (op == "mov" && args == "ecx, 5")
            bytes = {0xB9, 5, 0, 0, 0};
        else if (op == "mov" && args == "eax, 0")
            bytes = {0xB8, 0, 0, 0, 0};
        else if (op == "mov" && args.find("esi,") == 0) {
            string name = trim(args.substr(4));
            if (symbols.count(name)) {
                bytes.push_back(0xBE);
                addLE(bytes, symbols[name], 4);
            }
        } else if (op == "add" && args == "edi,ebx")
            bytes = {0x01, 0xDF};
        else if (op == "add" && args == "eax,ebx")
            bytes = {0x01, 0xD8};
        else if (op == "xor" && args == "ecx,ecx")
            bytes = {0x31, 0xC9};
        else if (op == "and" && args == "eax,ebx")
            bytes = {0x21, 0xD8};
        else if (op == "or" && args == "esi,ebx")
            bytes = {0x09, 0xDE};
        else if (op == "ret")
            bytes = {0xC3};

        if (!bytes.empty()) {
            show(ca, bytes, s);
            ca += bytes.size();
        } else {
            cout << "Unsupported instruction: " << s << '\n';
        }
    }

    cout << "\nConversion completed.\n";
    return 0;
}
