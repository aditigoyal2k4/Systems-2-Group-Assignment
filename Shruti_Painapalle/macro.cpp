#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>
using namespace std;
struct MNT {
    int index, operands, start, end;
    string name;
};
struct MDT {
    int index;
    string definition;
};
vector<MNT> mnt;
vector<MDT> mdt;
string trim(string s) {
    size_t p = s.find_first_not_of(" \t");
    if (p == string::npos) return "";
    return s.substr(p, s.find_last_not_of(" \t") - p + 1);
}
string replaceParameters(string s) {
    for (int i = 0; i + 1 < s.size(); i++)
        if (s[i] == '%' && isdigit(s[i + 1])) s[i] = '#';
    return s;
}
void readAssembly(string filename) {
    ifstream file(filename);
    if (!file) {
        cout << "File not found!\n";
        return;
    }
    string line;
    bool inside = false;
    MNT current;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.rfind("%macro", 0) == 0) {
            if (inside) {
                cout << "Nested macro not allowed!\n";
                return;
            }
            string directive;
            stringstream ss(line);
            ss >> directive >> current.name >> current.operands;

            for (auto m : mnt)
                if (m.name == current.name) {
                    cout << "Duplicate macro name!\n";
                    return;
                }
            current.index = mnt.size() + 1;
            current.start = mdt.size() + 1;
            inside = true;
        }
        else if (line == "%endmacro") {
            if (!inside) {
                cout << "Unexpected %endmacro!\n";
                return;
            }
            mdt.push_back({(int)mdt.size() + 1, "MEND"});
            current.end = mdt.size();
            mnt.push_back(current);
            inside = false;
        }
        else if (inside)
            mdt.push_back({(int)mdt.size() + 1, replaceParameters(line)});
    }
    if (inside) cout << "Missing %endmacro!\n";
}
void displayMNT() {
    cout << "\n========== MACRO NAME TABLE (MNT) ==========\n";
    cout << left << setw(10) << "Index" << setw(18) << "Macro Name"
         << setw(18) << "Operands" << setw(15) << "Start Index"
         << setw(15) << "End Index" << "\n";
    cout << string(76, '-') << "\n";
    for (auto m : mnt)
        cout << left << setw(10) << m.index << setw(18) << m.name
             << setw(18) << m.operands << setw(15) << m.start
             << setw(15) << m.end << "\n"<<endl;
}
void displayMDT() {
    cout << "\n========== MACRO DEFINITION TABLE (MDT) ==========\n";
    cout << left << setw(10) << "Index" << "Definition\n";
    cout << string(40, '-') << "\n";
    for (auto m : mdt)
        cout << left << setw(10) << m.index << m.definition << "\n";
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: ./macro <assembly_file.asm>\n";
        return 1;
    }
    readAssembly(argv[1]);
    if (!mnt.empty()) {
        displayMNT();
        displayMDT();
    } else cout << "No macro definitions found.\n";
    return 0;
}
