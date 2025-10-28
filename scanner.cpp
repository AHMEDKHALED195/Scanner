#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

struct TokenData {
    string category;
    string text;
    int lineNum;
};

vector<string> reservedWords = {
    "go", "build", "num", "word", "flip", "drift", "show", "grab",
    "check", "otherwise", "loop", "repeat", "give", "plug", "zone"
};

bool isReserved(const string& word) {
    for (auto& r : reservedWords)
        if (word == r)
            return true;
    return false;
}

vector<TokenData> analyzeCode(const string& source) {
    vector<TokenData> tokenList;

    regex pattern(
        "(//.*)|\"([^\"\\n]*)\"|(\\d+\\.\\d+)|(\\d+)|([a-zA-Z_][a-zA-Z0-9_]*)|"
        "(==|!=|<=|>=|=|\\+|\\-|\\*|/|<|>)|([(){};,])|(\\s+)|(.)"
    );

    int currentLine = 1;
    auto start = sregex_iterator(source.begin(), source.end(), pattern);
    auto end = sregex_iterator();

    for (auto it = start; it != end; ++it) {
        smatch result = *it;
        string value = result.str();

        if (result[1].matched) {
            // comment, ignore
        }
        else if (result[2].matched) {
            tokenList.push_back({ "STRING", "\"" + result[2].str() + "\"", currentLine });
        }
        else if (result[3].matched) {
            tokenList.push_back({ "DOUBLE", value, currentLine });
        }
        else if (result[4].matched) {
            tokenList.push_back({ "INTEGER", value, currentLine });
        }
        else if (result[5].matched) {
            if (isReserved(value))
                tokenList.push_back({ "KEYWORD", value, currentLine });
            else if (value == "true" || value == "false")
                tokenList.push_back({ "BOOLEAN", value, currentLine });
            else
                tokenList.push_back({ "IDENTIFIER", value, currentLine });
        }
        else if (result[6].matched) {
            tokenList.push_back({ "OPERATOR", value, currentLine });
        }
        else if (result[7].matched) {
            tokenList.push_back({ "SYMBOL", value, currentLine });
        }
        else if (result[8].matched) {
            for (char c : value)
                if (c == '\n') currentLine++;
            continue;
        }
        else if (result[9].matched) {
            cerr << "Error at line " << currentLine << ": invalid character '" << value << "'" << endl;
        }
    }

    return tokenList;
}

int main() {
    string inputCode = R"(
        go
        plug "game.lib";
        num score = 0;
        flip gameOn = true;
        show score;
        word playerName = "LeBron";
        drift score = score + 1.5;
    )";

    cout << "Input Code:\n" << inputCode << endl;
    cout << "-----------------------------\n";
    cout << "Tokens:\n\n";

    vector<TokenData> tokens = analyzeCode(inputCode);

    for (auto& tk : tokens) {
        cout << "Line " << tk.lineNum << " | "
            << tk.category << " -> " << tk.text << endl;
    }

    cout << "\nLexical analysis finished.\n";
    return 0;
}
