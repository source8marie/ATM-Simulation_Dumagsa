#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <cmath>

using namespace std;
const int MAX_ACCOUNTS = 100;

struct ATMCard {
    int accountNumber;
    string accountName;
    string birthday;
    string contactNumber;
    double balance;
    string pinCode;
};

class ATMCardList {
private:
    vector<ATMCard> accounts;
    char drive = 'E';

public:
    ATMCardList(char drive) : drive(drive) {};

    vector<ATMCard>& getAccounts() {
        return accounts;
    }

    size_t getAccountCount() const {
        return accounts.size();
    }

    void addAccount(const ATMCard& account) {
        accounts.push_back(account);
    }

    void insertAccount(const ATMCard& newAccount) {
        accounts.push_back(newAccount);
        saveAccounts();
    }

    ATMCard* retrieveAccount(int accountNumber) {
        for (ATMCard& account : accounts) {
            if (account.accountNumber == accountNumber) {
                return &account;
            }
        }
        return nullptr;
    }

 void saveAccounts() const {
    string pinFilePath = string(1, drive) + ":\\pinCode.txt";

    ofstream file("accounts.txt");
    ofstream pinFile(pinFilePath);

    if (file.is_open() && pinFile.is_open()) {
        for (const ATMCard& account : accounts) {
            file << "Account Number: " << account.accountNumber << endl;
            file << "Account Name: " << account.accountName << endl;
            file << "Birthday: " << account.birthday << endl;
            file << "Contact Number: " << account.contactNumber << endl;
            file << "Balance: " << account.balance << endl;
            file << "PIN Code: " << account.pinCode << endl;
            file << "-------------------" << endl;
        }


        file.close();


        ofstream pinFile(pinFilePath, std::ios_base::app);

        for (const ATMCard& account : accounts) {
            pinFile << account.accountNumber << " " << account.pinCode << endl;
        }

        pinFile.close();
    } else {
        cout << "Error: Unable to save the accounts or create pinCode.txt." << endl;
    }
}


    void loadAccounts() {
        ifstream file("accounts.txt");
        if (file.is_open()) {
            while (!file.eof()) {
                ATMCard account;
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file >> account.accountNumber;
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file.ignore();
                getline(file, account.accountName);
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file.ignore();
                getline(file, account.birthday);
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file.ignore();
                getline(file, account.contactNumber);
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file >> account.balance;
                file.ignore(std::numeric_limits<std::streamsize>::max(), ':');
                file >> account.pinCode;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '-');
                accounts.push_back(account);
            }
            file.close();
        } else {
            cout << "Error: Unable to load accounts." << endl;
        }
    }

    bool checkUser() const {
        return !accounts.empty();
    }
};

class Bank {
private:
    ATMCardList accountList{'E'};

public:
    Bank() {
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    vector<ATMCard>& getAccounts() {
        return accountList.getAccounts();
    }

    size_t getAccountCount() const {
        return accountList.getAccountCount();
    }

    char checkFlashdrive() const {
        DWORD drives = GetLogicalDrives();
        for (char drive = 'A'; drive <= 'Z'; ++drive) {
            if (drives & 1) {
                char rootPath[] = {drive, ':', '\\', '\0'};
                if (GetDriveType(rootPath) == DRIVE_REMOVABLE) {
                    return drive;
                }
            }
            drives >>= 1;
        }
        return false;
    }

    string enterPin() const {
        string pin;
        while (true) {
            system ("cls");
            cout << "\n\n\n\n\n\t\t\t#########################################################################" <<endl;
            cout << "\t\t\t#                                                                       #"<<endl;
            cout << "\t\t\t#                ENTER PERSONAL IDENTIFICATION NUMBER                   #"<<endl;
            cout << "\t\t\t#                                                                       #"<<endl;
            cout << "\t\t\t#########################################################################" <<endl <<endl<<endl;
            cout << "\t\t\t\t\t\tPIN code (4 digits): ";

            pin.clear();


            while (true) {
                char digit = _getch();
                if (isdigit(digit)) {
                    pin += digit;
                    cout << "*";

                } else if (digit == 13) {
                    if (pin.length() == 4 || pin.length() == 6) {
                        cout << endl;
                        return pin;
                    } else {
                        cout << endl << "\t\t\t\t\t\tPlease enter 4 digits only." << endl;
                        break;
                    }
                }
            }
        }
    }

    void saveAccounts() const {
        accountList.saveAccounts();
    }

    void loadAccounts() {
        accountList.loadAccounts();
    }

    bool checkUser() const {
        return accountList.checkUser();
    }

    void logIn() {
        int accountNumber;
        string pinCode;
        pinCode = enterPin();

        for (ATMCard& account : accountList.getAccounts()) {
            if (account.pinCode == pinCode) {
                cout << ".";
                transactionMenu(account.accountNumber, pinCode, *this);
                return;
            }
        }

        cout << "\t\t\t\t\t\tLogin failed. Invalid PIN code." << endl;
    }

    void registerAccount() {
    while (true) {
        system("cls");
        bool registrationSuccess = false;

        if (accountList.getAccountCount() < MAX_ACCOUNTS) {
            cout << endl << endl;
            cout << "\t     ATM SIMULATION | REGISTRATION MODULE" << endl << endl;
            cout << "\t-----------------------------------------------" << endl;
            cout << "\t-----------------------------------------------" << endl << endl;

            ATMCard newAccount;

            cout << "\tAccount Name: ";
            cin.ignore();
            getline(cin, newAccount.accountName);


            cout << "\n\n\tInput date of birth (MM/DD/YYYY): ";
            cin >> newAccount.birthday;


            if (newAccount.birthday.length() != 10 || newAccount.birthday[2] != '/' || newAccount.birthday[5] != '/') {
                cout << "\tError. Please enter the date in MM/DD/YYYY format." << endl << endl;
                cout << "\tDo you want to continue with the registration? (Y/N): ";
                char choice;
                cin >> choice;
                if (toupper(choice) != 'Y') {
                    break;
                }
                continue;
            }


            int currentYear = 2023;
            int year = stoi(newAccount.birthday.substr(6, 4));
            int age = currentYear - year;


            if (age < 18 || age > 120) {
                cout << "Error. Ages [18 and 120] only." << endl;
                cout << "\tDo you want to continue with the registration? (Y/N): ";
                char choice;
                cin >> choice;
                if (toupper(choice) != 'Y') {
                    break;
                }
                continue;
            }

            cout << "\n\n\tContact number (11 digits): +63 ";
            cin.ignore();
            long long contactNum = 0;
            int digitCount = 0;
            char ch;


            while ((ch = _getch()) != '\r') {
                if (isdigit(ch) && digitCount < 10) {
                    contactNum = contactNum * 10 + (ch - '0');
                    cout << ch;
                    digitCount++;
                } else if (ch == '\b' && digitCount > 0) {
                    contactNum = contactNum / 10;
                    cout << "\b \b";
                    digitCount--;
                }
            }

            if (contactNum >= 1000000000 && contactNum <= 9999999999) {
                newAccount.contactNumber = "+63" + to_string(contactNum);
            } else {
                cout << "\n\tError. Please enter a valid contact number." << endl << endl;
                cout << "\tDo you want to continue with the registration? (Y/N): ";
                char choice;
                cin >> choice;
                if (toupper(choice) != 'Y') {
                    break;
                }
                continue;
            }

            cout << "\n\n\tInitial Deposit/Balance (min 5000): ";
            cin >> newAccount.balance;
            while (newAccount.balance < 5000) {
                cout << "\t*Balance must be at least 5000." << endl;
                cout << "\tBalance (min 5000): ";
                cin >> newAccount.balance;
            }

            string newPin;
            cout << "\n\n\tPIN Code (4 digits): ";
            cin >> newPin;
            int pinAsInt = stoi(newPin);
            while (pinAsInt < 1000 || pinAsInt > 9999) {
                cout << "\t*Invalid. Please try again." << endl;
                cout << "\tPIN Code (4 digits): ";
                cin >> newPin;
                pinAsInt = stoi(newPin);
            }
            newAccount.pinCode = newPin;

            newAccount.accountNumber = rand() % 90000 + 10000;

            accountList.addAccount(newAccount);
            system("cls");

            cout << endl << endl << endl << endl;
            cout << "\t\t\t\t###############################################" << endl;
            cout << "\t\t\t\t           SUCCESSFULLY REGISTERED              " << endl;
            cout << "\t\t\t\t###############################################" << endl;
            cout << "\t\t\t\t _____________________________________________" << endl;
            cout << "\t\t\t\t Account Name: " << newAccount.accountName << endl;
            cout << "\t\t\t\t Birthday: " << newAccount.birthday << endl;
            cout << "\t\t\t\t Contact Number: " << newAccount.contactNumber << endl;
            cout << "\t\t\t\t Initial Deposit: " << newAccount.balance << endl;
            cout << "\t\t\t\t PIN Code: ****" << endl;
            cout << "\t\t\t\t _____________________________________________ " << endl;
            cout << "\t\t\t\t###############################################" << endl;
            cout << "\t\t\t\t           ACCOUNT NUMBER: " << newAccount.accountNumber << "              " << endl;
            cout << "\t\t\t\t###############################################" << endl << endl << endl << endl;

            registrationSuccess = true;
        } else {
            cout << "\n\n\t\t\t\t*Account limit reached!" << endl;
            cout << "\tDo you want to continue with the registration? (Y/N): ";
            char choice;
            cin >> choice;
            if (toupper(choice) != 'Y') {
                break;
            }
        }

        if (registrationSuccess) {
            break;
        }
    }
}




    void balanceInquiry(int accountNumber, const string& pinCode) {
        for (const ATMCard& account : accountList.getAccounts()) {
            if (account.accountNumber == accountNumber && account.pinCode == pinCode) {
                cout  << "\n\n\n\n\t\t\t\tATM SIMULATION | TRANSACTION MODULE | BALANCE INQUIRY";
                cout << "\n\n\t\t\t\tAccount Number: " << account.accountNumber << endl;
                cout << "\t\t\t\tAccount Name: " << account.accountName << endl;
                cout << "\t\t\t\tBalance: " << account.balance << endl << endl << endl;
                return;
            }
        }
        cout << "\n\t\t\t\tAuthentication failed. Access denied." << endl<< endl << endl;
    }


void withdraw(int accountNumber, const string& pinCode, double amount) {
    for (ATMCard& account : accountList.getAccounts()) {
        if (account.accountNumber == accountNumber && account.pinCode == pinCode) {
            if (amount >= 100 && fmod(amount, 100.0) == 0.0) {
                if (amount <= account.balance) {
                    account.balance -= amount;
                    cout << "\t\t\t\tWithdrawal successful. New balance: " << account.balance << endl;
                    saveAccounts();
                } else {
                    cout << "\t\t\t\tInsufficient funds for withdrawal." << endl;
                }
            } else {
                cout << "\t\t\t\tWithdrawal amount must be 100 minimum." << endl;
            }
            return;
        }
    }
    cout << "\t\t\t\tAuthentication failed. Access denied." << endl;
}


void deposit(int accountNumber, const string& pinCode, double amount) {
    for (ATMCard& account : accountList.getAccounts()) {
        if (account.accountNumber == accountNumber && account.pinCode == pinCode) {
            if (amount >= 100 && fmod(amount, 100.0) == 0.0) {
                account.balance += amount;
                cout << "\t\t\t\tDeposit successful. New balance: " << account.balance << endl;
                saveAccounts();
            } else {
                cout << "\t\t\t\tDeposit amount must be 100 minimum." << endl;
            }
            return;
        }
    }
    cout << "\t\t\t\tAuthentication failed. Access denied." << endl;
}


void fundTransfer(int senderAccountNumber, const string& senderPinCode, int receiverAccountNumber, double amount) {

    ATMCard* senderAccount = nullptr;
    for (ATMCard& account : accountList.getAccounts()) {
        if (account.accountNumber == senderAccountNumber && account.pinCode == senderPinCode) {
            senderAccount = &account;
            break;
        }
    }


    ATMCard* receiverAccount = nullptr;
    for (ATMCard& account : accountList.getAccounts()) {
        if (account.accountNumber == receiverAccountNumber) {
            receiverAccount = &account;
            break;
        }
    }


    if (senderAccount && receiverAccount) {
        if (amount >= 100 && fmod(amount, 100.0) == 0.0) {
            if (amount <= senderAccount->balance) {
                senderAccount->balance -= amount;
                receiverAccount->balance += amount;
                cout << "\n\t\t\t\t-------------------------" << endl;
                cout << "\t\t\t\tFund transfer successful!" << endl;
                cout << "\t\t\t\t-------------------------" << endl << endl;
                cout << "\t\t\t\tNew balance of Receiver: " << receiverAccount->balance << endl;
                cout << "\t\t\t\tNew balance of Sender: " << senderAccount->balance << endl << endl;
                saveAccounts();
            } else {
                cout << "\t\t\t\tInsufficient funds for fund transfer." << endl;
            }
        } else {
            cout << "\t\t\t\tTransfer amount must be 100 minimum." << endl;
        }
    } else {
        cout << "\t\t\t\tAuthentication failed. Access denied." << endl;
    }
}



   void changePin(int accountNumber, const string& pinCode) {
        cout << "\t\t\t\t\t\t*Input NEW PIN CODE below*" << endl;
        string newPin = enterPin();
        if (newPin.length() == 4) {
            for (ATMCard& account : accountList.getAccounts()) {
                if (account.accountNumber == accountNumber && account.pinCode == pinCode) {
                    account.pinCode = newPin;
                    cout << "\t\t\t\t\t\tPIN code changed successfully." << endl;
                    saveAccounts();
                    return;
                }
            }
            cout << "\t\t\t\tAuthentication failed. Access denied." << endl;
        } else {
            cout << "\t\t\t\tNew PIN code must be 4 digits." << endl;
        }
    }



    void transactionMenu(int accountNumber, const string& pinCode, Bank& bank) {
        char transactionOption;
        do {
            system("cls");
            cout << endl << endl;
            cout << "\tTransaction Menu\n\n";
            cout << "\t\ta. Balance Inquiry\n";
            cout << "\t\tb. Withdraw\n";
            cout << "\t\tc. Deposit\n";
            cout << "\t\td. Fund Transfer [for enrolled accounts]\n";
            cout << "\t\te. Change PIN Code [Input NEW PIN upon entry]\n";
            cout << "\t\tf. Back to Main Menu\n\n" << endl << endl;
            cout << "\tEnter letter of your choice: ";
            cin >> transactionOption;

            switch (transactionOption) {
                case 'a':
                    system("cls");
                    balanceInquiry(accountNumber, pinCode);
                    break;
                case 'b':
                    system("cls");
                    double withdrawAmount;
                    cout  << "\n\n\n\n\t\t\t\tATM SIMULATION | TRANSACTION MODULE | WITHDRAW" <<endl <<endl;
                    cout << "\t\t\t\tEnter the amount to withdraw (100 min.): ";
                    cin >> withdrawAmount;
                    withdraw(accountNumber, pinCode, withdrawAmount);
                    break;
                case 'c':
                    system("cls");
                    double depositAmount;
                    cout  << "\n\n\n\n\t\t\t\tATM SIMULATION | TRANSACTION MODULE | DEPOSIT" <<endl <<endl;
                    cout << "\t\t\t\tEnter the amount to deposit (100 min.): ";
                    cin >> depositAmount;
                    deposit(accountNumber, pinCode, depositAmount);
                    break;
                case 'd':
                    system("cls");
                    int receiverAccountNumber;
                    cout  << "\n\n\n\n\t\t\t\tATM SIMULATION | TRANSACTION MODULE | FUND TRANSFER" <<endl <<endl;
                    cout << "\t\t\t\tEnter the account number to transfer funds to: ";
                    cin >> receiverAccountNumber;
                    double transferAmount;
                    cout << "\t\t\t\tEnter the amount to transfer (100 min.): ";
                    cin >> transferAmount;
                    bank.fundTransfer(accountNumber, pinCode, receiverAccountNumber, transferAmount);
                    break;

                case 'e':
                    system("cls");
                    changePin(accountNumber, pinCode);
                    break;
                case 'f':
                    {
                        return;
                    }
                default:
                    cout << "\t*Invalid transaction option." << endl;
                    cin.ignore();
                    getchar();
                    break;
            }

            char choice;
            cout << endl << endl;
            cout << "\t\t\t\tDo you want to stay in the transaction menu? (y/n): ";
            cin >> choice;
            if (choice == 'n') {
                break;
            }
        } while (true);
    }

public:
    vector<ATMCard> accounts;

};

void loading(Bank &bank) {
    system("COLOR 0D");

    char flashDriveInserted = bank.checkFlashdrive();

    cout << "\n\n\n\n\n\n";
    cout << "\n\n\t\t\t\t\tACCESSING THE ATM SIMULATION PROGRAM...\n\n";

    for (int i = 1; i <= 120; i++) {
        Sleep(8);
        cout << (char)219;
    }

    int timeoutSeconds = 30;
    auto startTime = chrono::steady_clock::now();

    while (!flashDriveInserted) {
        cout << "\n\n\t\t\t\t\t\tPlease insert flashdrive!" << endl;


        while (true) {
            char flashDriveLetter = bank.checkFlashdrive();
            if (flashDriveLetter) {
                flashDriveInserted = flashDriveLetter;
                break;
            }

            auto currentTime = chrono::steady_clock::now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();

            if (elapsedTime >= timeoutSeconds) {
                cout << "\n\n\t\t\t\t\t\tLoading timeout. Exiting the ATM system." << endl;
                exit(EXIT_FAILURE);
            }

            this_thread::sleep_for(chrono::seconds(1));
        }

        if (flashDriveInserted) {
            cout << "\n\n\t\t\t\t\t\tDetected. ";
        }
    }

    cout << "\n\n\n\n\n\n\t\t\t\t\t\tATM system is now active.";
}




int main() {
    system("color 0D");
    Bank bank;
    loading(bank);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    int choice;
    int accountNumber;
    string pinCode;


    bank.loadAccounts();

    do {
        system("cls");

        cout << endl << endl;
        cout << "\t\t\t\t\t  ________   ________   _________  "  << endl;
        cout << "\t\t\t\t\t |   __   | |        | |         | " << endl;
        cout << "\t\t\t\t\t |  |  |  | |__    __| |         | " <<endl;
        cout << "\t\t\t\t\t |  |__|  |    |  |    |  |\\_/|  | " <<endl;
        cout << "\t\t\t\t\t |  |  |  |    |  |    |  |   |  | " << endl;
        cout << "\t\t\t\t\t |__|  |__|    |__|    |__|   |__| " << endl;


        cout << endl << endl;
        cout << "\t\t\t\t*-------------------------------------------------*" << endl;
        cout << "\t\t\t\t|                                                 |" << endl;
        cout << "\t\t\t\t| ATM SIMULATION | MAIN MENU | BSIS 2A STUDENT 19 |" << endl;
        cout << "\t\t\t\t|                                                 |" << endl;
        cout << "\t\t\t\t*-------------------------------------------------*" << endl;
        cout << "\t\t\t\t|\t                                          |" << endl;
        cout << "\t\t\t\t|\t1. Register                               |" << endl;
        cout << "\t\t\t\t|\t2. Transaction                            |" << endl;
        cout << "\t\t\t\t|\t3. Exit                                   |" << endl;
        cout << "\t\t\t\t|\t                                          |" << endl;
        cout << "\t\t\t\t*-------------------------------------------------*" << endl << endl << endl;

        cout << "\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                bank.registerAccount();
                bank.saveAccounts();
                break;
            }
            case 2: {
                if (bank.checkUser()) {
                    bank.logIn();
                } else {
                    cout << "\n\t\t\t\tNo registered users. Please register an account first." << endl;
                }
                break;
            }
            case 3: {
                cout << "\n\t\t\t\tThank you for using the ATM! [Project 1 by Dumagsa, Trisha Marie R.]" << endl;
                bank.saveAccounts();
                break;
            }
            default: {
                cout << "\t\t\t\tInvalid choice. Please try again." << endl;
                cout << endl  << endl;
                getchar();
            }
        }
        if (choice != 3) {
        system("pause");
        }
    } while (choice != 3);
    return 0;
 }


