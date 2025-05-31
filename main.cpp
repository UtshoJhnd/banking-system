#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <conio.h>
#include <algorithm>
#include <cstdlib>
#include <iomanip>

using namespace std;

const float FDR_INTEREST = 0.10f;
const float DPS_INTEREST = 0.06f;
const string FILENAME = "accounts.txt";
const int MAX_USERS = 10;

struct BankUser {
    string name;
    int accountNumber;
    int pin;
    float balance;
    float fdrAmount;
    float dpsAmount;

    BankUser() : accountNumber(0), pin(0), balance(0), fdrAmount(0), dpsAmount(0) {}

    string toString() const {
        ostringstream ss;
        ss << name << "|" << accountNumber << "|" << pin << "|"
           << balance << "|" << fdrAmount << "|" << dpsAmount;
        return ss.str();
    }

    void showInfo() const {
        cout << fixed << setprecision(2);
        cout << "Name: " << name
             << " | Acc No: " << accountNumber
             << " | Balance: ৳" << balance
             << " | FDR: ৳" << fdrAmount
             << " | DPS: ৳" << dpsAmount << endl;
    }
};

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

string getMaskedPIN() {
    string pin;
    char ch;
    cout << "Enter 4-digit PIN: ";
    while ((ch = _getch()) && ch != '\r') {
        if (ch >= '0' && ch <= '9' && pin.length() < 4) {
            cout << '*';
            pin += ch;
        } else if (ch == '\b' && !pin.empty()) {
            cout << "\b \b";
            pin.pop_back();
        }
    }
    cout << endl;
    return pin;
}

vector<BankUser> loadAccounts() {
    vector<BankUser> users;
    ifstream file(FILENAME);
    string line;
    while (getline(file, line)) {
        BankUser user;
        replace(line.begin(), line.end(), '|', ' ');
        istringstream iss(line);
        iss >> user.name >> user.accountNumber >> user.pin
            >> user.balance >> user.fdrAmount >> user.dpsAmount;
        users.push_back(user);
    }
    return users;
}

void saveAccounts(const vector<BankUser>& users) {
    ofstream file(FILENAME);
    for (const auto& user : users) {
        file << user.toString() << endl;
    }
}

int findUserIndexByAccount(const vector<BankUser>& users, int accNo) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i].accountNumber == accNo)
            return i;
    }
    return -1;
}

bool verifyPIN(const BankUser& user) {
    string inputPin = getMaskedPIN();
    return to_string(user.pin) == inputPin;
}

void createAccount(vector<BankUser>& users) {
    if (users.size() >= MAX_USERS) {
        cout << "Maximum account limit reached.\n";
        return;
    }

    BankUser newUser;
    cin.ignore();
    cout << "Enter full name: ";
    getline(cin, newUser.name);

    string pinStr = getMaskedPIN();
    if (pinStr.length() != 4 || !all_of(pinStr.begin(), pinStr.end(), ::isdigit)) {
        cout << "Invalid PIN. Must be 4 digits.\n";
        return;
    }
    newUser.pin = stoi(pinStr);
    newUser.accountNumber = 1001 + users.size();
    users.push_back(newUser);
    saveAccounts(users);
    cout << "Account created. Account Number: " << newUser.accountNumber << endl;
}

void deposit(BankUser& user) {
    float amount;
    cout << "Enter amount to deposit: ";
    if (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        cin.clear(); cin.ignore(10000, '\n');
        return;
    }
    user.balance += amount;
    cout << "Deposit successful. New balance: ৳" << user.balance << endl;
}

void withdraw(BankUser& user) {
    float amount;
    cout << "Enter amount to withdraw: ";
    if (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        cin.clear(); cin.ignore(10000, '\n');
        return;
    }
    if (amount > user.balance) {
        cout << "Insufficient funds.\n";
    } else {
        user.balance -= amount;
        cout << "Withdrawal successful. New balance: ৳" << user.balance << endl;
    }
}

void openFDR(BankUser& user) {
    float amount;
    cout << "Enter FDR amount: ";
    if (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        cin.clear(); cin.ignore(10000, '\n');
        return;
    }
    if (amount <= user.balance) {
        user.balance -= amount;
        user.fdrAmount += amount + (amount * FDR_INTEREST);
        cout << "FDR opened. Maturity amount: ৳" << user.fdrAmount << endl;
    } else {
        cout << "Not enough balance.\n";
    }
}

void openDPS(BankUser& user) {
    float monthly;
    int months;
    cout << "Enter monthly deposit: ";
    cin >> monthly;
    cout << "Enter number of months: ";
    cin >> months;
    if (monthly > 0 && months > 0) {
        float total = monthly * months;
        float interest = total * DPS_INTEREST;
        user.dpsAmount += total + interest;
        cout << "DPS started. Total after interest: ৳" << user.dpsAmount << endl;
    } else {
        cout << "Invalid DPS input.\n";
    }
}

int main() {
    vector<BankUser> users = loadAccounts();
    int choice;

    do {
        clearScreen();
        cout << "\n--- Bank Menu ---\n";
        cout << "1. Create Account\n";
        cout << "2. Access Account\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            createAccount(users);
            system("pause");
        } else if (choice == 2) {
            clearScreen();
            int accNo;
            cout << "Enter Account Number: ";
            cin >> accNo;
            int idx = findUserIndexByAccount(users, accNo);

            if (idx == -1) {
                cout << "Account not found.\n";
                system("pause");
                continue;
            }

            if (!verifyPIN(users[idx])) {
                cout << "Incorrect PIN.\n";
                system("pause");
                continue;
            }

            int userChoice;
            do {
                clearScreen();
                cout << "\n--- Account Menu ---\n";
                cout << "1. Deposit\n";
                cout << "2. Withdraw\n";
                cout << "3. Open FDR\n";
                cout << "4. Open DPS\n";
                cout << "5. Show Info\n";
                cout << "6. Logout\n";
                cout << "Enter your choice: ";
                cin >> userChoice;

                switch (userChoice) {
                    case 1: deposit(users[idx]); break;
                    case 2: withdraw(users[idx]); break;
                    case 3: openFDR(users[idx]); break;
                    case 4: openDPS(users[idx]); break;
                    case 5: users[idx].showInfo(); break;
                    case 6: cout << "Logged out.\n"; break;
                    default: cout << "Invalid choice.\n";
                }

                saveAccounts(users);
                if (userChoice != 6) system("pause");

            } while (userChoice != 6);

        } else if (choice == 3) {
            cout << "Exiting program.\n";
        } else {
            cout << "Invalid choice.\n";
            system("pause");
        }

    } while (choice != 3);

    return 0;
}
	
