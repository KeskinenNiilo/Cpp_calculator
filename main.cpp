#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <cstdlib>

using namespace std;

void clearScreen() { // Checks system for console clear
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

bool validString(const string& s) { // Check input
    static const unordered_set<char> validChars = {
        '0','1','2','3','4','5','6','7','8','9',
        ' ', '+', '-', '*', '/', '^', '(', ')'
    };

    for (char c : s) {
        if (!validChars.count(c))
            return false;
    }
    return true;
}

bool validParanthesis(const string& s) { // Check if paranthesis are balanced and right way round
    int count = 0;
    for (char c : s) {
        if (c == '(') ++count;
        else if (c == ')') {
            --count;
            if (count < 0) return false;
        }
    }
    return count == 0;
}

void deleteSpaces(string& s) { // delete white space
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
}

vector<string> stringToVector(const string& s) { // turn str to vector<str>
    unordered_set<char> unaryMinusPredecessor = {'+', '-', '*', '/', '^', '('};
    unordered_set<char> symbols = {'+','-','*','/','^','(',')'};

    vector<string> tokens;
    string currentNumber;
    int n = s.size();

    for (int i = 0; i < n; ++i) {
        char curr = s[i];
        char prev = (i > 0) ? s[i - 1] : '\0';

        bool isUnaryMinus = curr == '-' && (i == 0 || unaryMinusPredecessor.count(prev)); // Check if minus is given to number or is a unary

        if (isUnaryMinus) {
            if (!currentNumber.empty()) {
                tokens.push_back(currentNumber);
                currentNumber.clear();
            }
            tokens.push_back("u-");
        }
        else if (symbols.count(curr)) {
            if (!currentNumber.empty()) {
                tokens.push_back(currentNumber);
                currentNumber.clear();
            }
            tokens.push_back(string(1, curr));
        }
        else {
            currentNumber += curr;
            if (i == n - 1 || symbols.count(s[i + 1])) {
                tokens.push_back(currentNumber);
                currentNumber.clear();
            }
        }
    }

    return tokens;
}

double calculate(vector<string>& v) { // The part that calculates things
    size_t i = 0;
    while (i < v.size()) {
        if (v[i] == "(") {
            size_t start = i;
            int count = 1;
            size_t j = i + 1;
            vector<string> subExpr(v.begin() + start + 1, v.begin() + j - 1);
            double subResult = calculate(subExpr); // recursion

            v.erase(v.begin() + start, v.begin() + j);
            v.insert(v.begin() + start, to_string(subResult));

            i = 0;
        } else {
            i++;
        }
    }

    vector<unordered_set<string>> pemdas = { // Operatins in right order
        {"^"},
        {"u-"},
        {"*", "/"},
        {"+", "-"}
    };

    map<string, function<double(double)>> unaryOperation;
    map<string, function<double(double, double)>> binaryOperation;

    unaryOperation["u-"] = [](double a) { return -a; }; // map chars to funcs
    binaryOperation["^"] = [](double a, double b) { return pow(a, b); };
    binaryOperation["*"] = [](double a, double b) { return a * b; };
    binaryOperation["/"] = [](double a, double b) { return a / b; };
    binaryOperation["+"] = [](double a, double b) { return a + b; };
    binaryOperation["-"] = [](double a, double b) { return a - b; };

    for (auto& ops : pemdas) {
    
        if (ops.count("^")) { // Exponent handling so exponents are calculated from right to left
            for (int i = (int)v.size() - 1; i >= 0; --i) {
                if (v[i] == "^") {
                    double left = stod(v[i - 1]);
                    double right = stod(v[i + 1]);
                    double result = binaryOperation["^"](left, right);

                    v.erase(v.begin() + i - 1, v.begin() + i + 2);
                    v.insert(v.begin() + i - 1, to_string(result));
                }
            }
            continue; 
        }

        size_t i = 0;
        while (i < v.size()) {
            if (ops.count(v[i])) {
                if (v[i] == "u-") { // unary handling so -3^2 doesnt become (-3)^2
                    double value = unaryOperation["u-"](stod(v[i + 1]));
                    v.erase(v.begin() + i, v.begin() + i + 2);
                    v.insert(v.begin() + i, to_string(value));
                } else {
                    double left = stod(v[i - 1]);
                    double right = stod(v[i + 1]);
                    double result = binaryOperation[v[i]](left, right);

                    v.erase(v.begin() + i - 1, v.begin() + i + 2);
                    v.insert(v.begin() + i - 1, to_string(result));
                    if (i > 0) i--;
                }
            } else {
                i++;
            }
        }
    }


    return stod(v[0]);
}

int main() {
    clearScreen();
    cout << "+----------------+" << endl;
    cout << "| C++ Calculator |" << endl;
    cout << "| Niilo Keskinen |" << endl;
    cout << "+----------------+" << endl;

    cin.get();
    clearScreen();

    string input;

    while (true) {
        cout << "INPUT: ";
        getline(cin, input);

        if (input == "exit") {
            clearScreen();
            break;
        }
        else if (input == "clear" || input == "cls") {
            clearScreen();
            continue;
        }
        else if (!validString(input) || !validParanthesis(input)) {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }

        deleteSpaces(input);
        vector<string> tokens = stringToVector(input);

        try {
            double result = calculate(tokens);
            double rounded = round(result * 1e6) / 1e6;
            if (floor(rounded) == rounded) cout << "= " << static_cast<long long>(rounded) << endl;
            else cout << "= " << fixed << setprecision(6) << rounded << endl;
            cout << endl;
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}
