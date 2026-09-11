#pragma once
#include <vector>
#include <stdexcept>
#include "token.h"

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

enum class AngleMode { Radians, Degrees };

class Parser {
public:
    explicit Parser(std::vector<Token> tokens, double ansValue = 0.0,
                     AngleMode angleMode = AngleMode::Radians);

    double parse();

private:
    std::vector<Token> tokens;
    size_t pos = 0;
    double ans = 0.0;
    AngleMode angleMode;

    const Token& peek() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& expect(TokenType type, const std::string& msg);
    [[noreturn]] void error(const std::string& msg) const;

    double checkFinite(double value, const std::string& context) const;

    double applyFunction(TokenType func, double arg) const;

    double applyTwoArgFunction(TokenType func, double a, double b) const;

    double parseExpression();
    double parseTerm();
    double parseUnary();
    double parseExponent();
    double parsePostfix();
    double parsePrimary();
};
