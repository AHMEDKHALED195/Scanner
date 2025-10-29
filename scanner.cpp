#include <iostream>
#include <regex>
#include <string>
using namespace std;

class TokenData {
public:
    string category;
    string text;
    int lineNum;

    TokenData() {}
    TokenData(string c, string t, int l) {
        category = c;
        text = t;
        lineNum = l;
    }
};

string reservedWords[] = {
    "go", "build", "num", "word", "flip", "drift", "show", "grab",
    "check", "otherwise", "loop", "repeat", "give", "plug", "zone"
};
int reservedCount = sizeof(reservedWords) / sizeof(reservedWords[0]);

bool isReserved(const string& word) {
    for (int i = 0; i < reservedCount; i++) {
        if (word == reservedWords[i])
            return true;
    }
    return false;
}

int analyzeCode(const string& source, TokenData tokens[], int maxTokens) {
    regex pattern(
        "(//.*)|\"([^\"\\n]*)\"|(\\d+\\.\\d+)|(\\d+)|([a-zA-Z_][a-zA-Z0-9_]*)|"
        "(==|!=|<=|>=|=|\\+|\\-|\\*|/|<|>)|([(){};,])|(\\s+)|(.)"
    );

    int currentLine = 1;
    int tokenCount = 0;
    auto start = sregex_iterator(source.begin(), source.end(), pattern);
    auto end = sregex_iterator();

    for (auto it = start; it != end && tokenCount < maxTokens; ++it) {
        smatch result = *it;
        string value = result.str();

        if (result[1].matched) {
            // comment
        }
        else if (result[2].matched) {
            tokens[tokenCount++] = TokenData("STRING", "\"" + result[2].str() + "\"", currentLine);
        }
        else if (result[3].matched) {
            tokens[tokenCount++] = TokenData("DOUBLE", value, currentLine);
        }
        else if (result[4].matched) {
            tokens[tokenCount++] = TokenData("INTEGER", value, currentLine);
        }
        else if (result[5].matched) {
            if (isReserved(value))
                tokens[tokenCount++] = TokenData("KEYWORD", value, currentLine);
            else if (value == "true" || value == "false")
                tokens[tokenCount++] = TokenData("BOOLEAN", value, currentLine);
            else
                tokens[tokenCount++] = TokenData("IDENTIFIER", value, currentLine);
        }
        else if (result[6].matched) {
            tokens[tokenCount++] = TokenData("OPERATOR", value, currentLine);
        }
        else if (result[7].matched) {
            tokens[tokenCount++] = TokenData("SYMBOL", value, currentLine);
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
    return tokenCount;
}

int main() {
    string inputCode = R"(go
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

    const int MAX = 200;
    TokenData tokenArray[MAX];
    int count = analyzeCode(inputCode, tokenArray, MAX);

    for (int i = 0; i < count; i++) {
        cout << "Line " << tokenArray[i].lineNum
            << " | " << tokenArray[i].category
            << " -> " << tokenArray[i].text << endl;
    }

    cout << "\nLexical analysis finished.\n";
    return 0;
}
