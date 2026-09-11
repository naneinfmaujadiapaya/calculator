#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "token.h"

class LexError : public std::runtime_error {
public:
    explicit LexError(const std::string& msg) : std::runtime_error(msg) {}
};

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos = 0;

    char peek() const;
    char advance();
    bool isAtEnd() const;
    void skipSpaces();
    Token readNumber();
    Token readIdentifier();
};
