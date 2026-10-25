#include <iostream>
#include <cstdint>
using namespace std;

int main() {
    uint8_t a, b;
    char operation;

    cout << "Choose operation (A for ADD, S for SUB): ";
    cin >> operation;
    cout << "Enter first 8-bit number (0-255): ";
    unsigned int x;
    cin >> x;

    cout << "Enter second 8-bit number (0-255): ";
    unsigned int y;
    cin >> y;

    if (x > 255 || y > 255) {
        cout << "Invalid input! Enter numbers between 0 and 255.\n";
        return 1;
    }

    a = static_cast<uint8_t>(x);
    b = static_cast<uint8_t>(y);

    unsigned int result;
    uint8_t answer;

    int CF, ZF, SF, OF;

    if (operation == 'A' || operation == 'a')
    {
        result = static_cast<unsigned int>(a) + static_cast<unsigned int>(b);
        answer = static_cast<uint8_t>(result);

        CF = (result > 255) ? 1 : 0;
        OF = ((~(a ^ b) & (a ^ answer) & 0x80) != 0) ? 1 : 0;
    }
    else if (operation == 'S' || operation == 's')
    {
        result = static_cast<unsigned int>(a) - static_cast<unsigned int>(b);
        answer = static_cast<uint8_t>(result);

        CF = (a < b) ? 1 : 0;
        OF = (((a ^ b) & (a ^ answer) & 0x80) != 0) ? 1 : 0;
    }
    else
    {
        cout << "Invalid operation!\n";
        return 1;
    }

    ZF = (answer == 0) ? 1 : 0;
    SF = (answer & 0x80) ? 1 : 0;

    cout << "\nResult (8-bit): " << static_cast<unsigned int>(answer) << '\n';
    cout << "Carry Flag (CF): " << CF << '\n';
    cout << "Zero Flag (ZF): " << ZF << '\n';
    cout << "Sign Flag (SF): " << SF << '\n';
    cout << "Overflow Flag (OF): " << OF << '\n';
    CF = (result > 255) ? 1 : 0;

    ZF = (answer == 0) ? 1 : 0;

    SF = (answer & 0x80) ? 1 : 0;

    OF = ((~(a ^ b) & (a ^ answer) & 0x80) != 0) ? 1 : 0;
    cout << "\nResult (8-bit): " << static_cast<unsigned int>(answer) << '\n';
    cout << "Carry Flag (CF): " << CF << '\n';
    cout << "Zero Flag (ZF): " << ZF << '\n';
    cout << "Sign Flag (SF): " << SF << '\n';
    cout << "Overflow Flag (OF): " << OF << '\n';

        return 0;
}
