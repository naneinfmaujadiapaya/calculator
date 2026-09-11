#pragma once

enum class TokenType {
    Number,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Caret,
    LParen,
    RParen,
    Comma,
    Bang,
    Sqrt,
    Cbrt,
    Sin,
    Cos,
    Tan,
    Asin,
    Acos,
    Atan,
    Sinh,
    Cosh,
    Tanh,
    Ln,
    Log,
    Abs,
    NCr,
    NPr,
    Gcd,
    Lcm,
    Pi,
    Euler,
    Ans,
    End
};

struct Token {
    TokenType type;
    double value = 0.0;
    int position = 0;
};
