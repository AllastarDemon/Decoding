#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

int generateRandomSignature() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1023);

    return dis(gen);
}

std::string calculateHash(const string& data) {
    int hashValue = 0;
    for (char c : data) {
        hashValue += static_cast<int>(c);
    }
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << hashValue;

    return ss.str();
}

bool hasSignature(const std::string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    char c;
    while (file.get(c)) {
        if (c == '\0') {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

bool removeSignature(const string& filename) {
    ifstream inputFile(filename, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    vector<char> data((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    auto it = remove(data.begin(), data.end(), '\0');
    data.erase(it, data.end());

   ofstream outputFile("unsigned_" + filename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot create file unsigned_" << filename << endl;
        return false;
    }

    outputFile.write(data.data(), data.size());
    outputFile.close();

    cout << "Signature removed. Unsigned file saved as unsigned_" << filename << endl;

    return true;
}

bool addSignature(const string& filename) {
    ifstream inputFile(filename, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    vector<char> data((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    int signature = generateRandomSignature();
    data.push_back('\0');
    data.push_back(static_cast<char>(signature));

    ofstream outputFile("signed_" + filename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot create file signed_" << filename << endl;
        return false;
    }

    outputFile.write(data.data(), data.size());
    outputFile.close();

    cout << "Signature added. Signed file saved as signed_" << filename << endl;

    return true;
}

int main() {
    string filename, data, hash;

    cout << "Enter the filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    data.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    bool hasSig = hasSignature(filename);
    cout << "Signature status: " << (hasSig ? "Present" : "Absent") << endl;

    cout << "File content:\n" << data << endl;

    char choice;
    cout << "Do you want to remove (r) or add (a) signature? ";
    cin >> choice;

    if (choice == 'r') {
        removeSignature(filename);
    }
    else if (choice == 'a') {
        addSignature(filename);
    }
    else {
        cout << "Invalid choice. No action performed." << endl;
    }

    return 0;
}