#include "lexer.h"
#include <cctype>
#include <cstdlib>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : src(source) {}

bool Lexer::isAtEnd() const {
    return pos >= src.size();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return src[pos];
}

char Lexer::advance() {
    return src[pos++];
}

void Lexer::skipSpaces() {
    while (!isAtEnd() && std::isspace(static_cast<unsigned char>(peek()))) {
        advance();
    }
}

Token Lexer::readNumber() {
    size_t start = pos;
    int startPos = static_cast<int>(pos);
    bool sawDot = false;

    while (!isAtEnd() && (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '.')) {
        if (peek() == '.') {
            if (sawDot) {
                throw LexError("Angka tidak valid: terlalu banyak titik desimal pada posisi " +
                                std::to_string(pos));
            }
            sawDot = true;
        }
        advance();
    }

    // notasi ilmiah cuma dianggap valid kalau diikuti angka
    if (!isAtEnd() && (peek() == 'e' || peek() == 'E')) {
        size_t lookahead = pos + 1;
        if (lookahead < src.size() && (src[lookahead] == '+' || src[lookahead] == '-')) {
            lookahead++;
        }
        if (lookahead < src.size() && std::isdigit(static_cast<unsigned char>(src[lookahead]))) {
            advance();
            if (peek() == '+' || peek() == '-') advance();
            while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) advance();
        }
    }

    std::string numStr = src.substr(start, pos - start);
    if (numStr == "." || numStr.empty()) {
        throw LexError("Angka tidak valid pada posisi " + std::to_string(start));
    }

    Token t;
    t.type = TokenType::Number;
    t.value = std::strtod(numStr.c_str(), nullptr);
    t.position = startPos;
    return t;
}

Token Lexer::readIdentifier() {
    static const std::unordered_map<std::string, TokenType> kKeywords = {
        {"sqrt", TokenType::Sqrt}, {"cbrt", TokenType::Cbrt},
        {"sin", TokenType::Sin},   {"cos", TokenType::Cos}, {"tan", TokenType::Tan},
        {"asin", TokenType::Asin}, {"acos", TokenType::Acos}, {"atan", TokenType::Atan},
        {"sinh", TokenType::Sinh}, {"cosh", TokenType::Cosh}, {"tanh", TokenType::Tanh},
        {"ln", TokenType::Ln},     {"log", TokenType::Log},   {"abs", TokenType::Abs},
        {"ncr", TokenType::NCr},   {"npr", TokenType::NPr},
        {"gcd", TokenType::Gcd},   {"lcm", TokenType::Lcm},
        {"pi", TokenType::Pi},     {"e", TokenType::Euler},
        {"ans", TokenType::Ans},
    };

    size_t start = pos;
    int startPos = static_cast<int>(pos);
    while (!isAtEnd() && std::isalpha(static_cast<unsigned char>(peek()))) {
        advance();
    }
    std::string word = src.substr(start, pos - start);

    auto it = kKeywords.find(word);
    if (it == kKeywords.end()) {
        throw LexError("Nama tidak dikenal '" + word + "' pada posisi " +
                        std::to_string(startPos));
    }

    Token t;
    t.type = it->second;
    t.position = startPos;
    return t;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipSpaces();
        if (isAtEnd()) break;

        int startPos = static_cast<int>(pos);
        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(readNumber());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c))) {
            tokens.push_back(readIdentifier());
            continue;
        }

        Token t;
        t.position = startPos;
        switch (c) {
            case '+': t.type = TokenType::Plus;   advance(); break;
            case '-': t.type = TokenType::Minus;  advance(); break;
            case '*': t.type = TokenType::Star;   advance(); break;
            case '/': t.type = TokenType::Slash;  advance(); break;
            case '%': t.type = TokenType::Percent; advance(); break;
            case '^': t.type = TokenType::Caret;  advance(); break;
            case '(': t.type = TokenType::LParen; advance(); break;
            case ')': t.type = TokenType::RParen; advance(); break;
            case ',': t.type = TokenType::Comma;  advance(); break;
            case '!': t.type = TokenType::Bang;   advance(); break;
            default:
                throw LexError(std::string("Karakter tidak dikenal '") + c +
                                "' pada posisi " + std::to_string(pos));
        }
        tokens.push_back(t);
    }

    Token end;
    end.type = TokenType::End;
    end.position = static_cast<int>(pos);
    tokens.push_back(end);
    return tokens;
}
