#include <iostream>
#include <cstdlib>

using namespace std;

int main() {
    int choice;
    cout << "Choose a steganographic technique:\n";
    cout << "1. F5 Technique\n";
    cout << "2. LSB Technique\n";
    cout << "3. PVD Technique\n";
    cout << "Enter your choice (1-3): ";
    cin >> choice;

    switch (choice) {
        case 1:
            system("f5");
            break;
        case 2:
            system("lsb");
            break;
        case 3:
            system("pvd");
            break;
        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
    }

    return 0;
}
