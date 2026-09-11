#include "lexer.h"
#include "parser.h"
#include "test_framework.h"

namespace {
double eval(const std::string& expr, double ans = 0.0,
            AngleMode mode = AngleMode::Radians) {
    Lexer lexer(expr);
    Parser parser(lexer.tokenize(), ans, mode);
    return parser.parse();
}
}

void run_parser_tests() {
    testing::suite("Parser");

    EXPECT_NEAR(eval("2+3"), 5.0, 1e-9, "2+3 = 5");
    EXPECT_NEAR(eval("10-4"), 6.0, 1e-9, "10-4 = 6");
    EXPECT_NEAR(eval("6*7"), 42.0, 1e-9, "6*7 = 42");
    EXPECT_NEAR(eval("9/2"), 4.5, 1e-9, "9/2 = 4.5");
    EXPECT_NEAR(eval("9%4"), 1.0, 1e-9, "9%4 = 1");

    EXPECT_NEAR(eval("2+3*4"), 14.0, 1e-9, "respects * before +: 2+3*4 = 14");
    EXPECT_NEAR(eval("(2+3)*4"), 20.0, 1e-9, "parentheses override precedence: (2+3)*4 = 20");
    EXPECT_NEAR(eval("2*3+4*5"), 26.0, 1e-9, "2*3+4*5 = 26");
    EXPECT_NEAR(eval("2-3-4"), -5.0, 1e-9, "left-associative: 2-3-4 = -5");

    EXPECT_NEAR(eval("-5+3"), -2.0, 1e-9, "leading unary minus: -5+3 = -2");
    EXPECT_NEAR(eval("-(2+3)"), -5.0, 1e-9, "unary minus over a group: -(2+3) = -5");
    EXPECT_NEAR(eval("+5"), 5.0, 1e-9, "leading unary plus is a no-op");

    EXPECT_NEAR(eval("2^3"), 8.0, 1e-9, "2^3 = 8");
    EXPECT_NEAR(eval("2^3^2"), 512.0, 1e-9, "right-associative: 2^3^2 = 2^(3^2) = 512");
    EXPECT_NEAR(eval("2^-2"), 0.25, 1e-9, "negative exponent: 2^-2 = 0.25");

    EXPECT_NEAR(eval("((1+2)*(3+4))"), 21.0, 1e-9, "nested parens: ((1+2)*(3+4)) = 21");
    EXPECT_NEAR(eval("  3   +   4  "), 7.0, 1e-9, "tolerates surrounding whitespace");

    EXPECT_THROWS(eval("5/0"), ParseError, "division by zero throws ParseError");
    EXPECT_THROWS(eval("5%0"), ParseError, "modulo by zero throws ParseError");
    EXPECT_THROWS(eval("(1+2"), ParseError, "unbalanced '(' throws ParseError");
    EXPECT_THROWS(eval("1+2)"), ParseError, "unexpected trailing ')' throws ParseError");
    EXPECT_THROWS(eval(""), ParseError, "empty expression throws ParseError");
    EXPECT_THROWS(eval("1 2"), ParseError, "two numbers with no operator throws ParseError");
    EXPECT_THROWS(eval("*3"), ParseError, "expression starting with an operator throws ParseError");

    EXPECT_THROWS(eval("(-8)^0.5"), ParseError,
                  "negative base with fractional exponent (NaN) throws ParseError");
    EXPECT_THROWS(eval("(-1)^0.5"), ParseError,
                  "another negative base / fractional exponent case throws ParseError");
    EXPECT_THROWS(eval("10^400"), ParseError,
                  "exponent overflowing to infinity throws ParseError");
    EXPECT_THROWS(eval("0^-1"), ParseError,
                  "0 raised to a negative power (Inf) throws ParseError");
    EXPECT_NEAR(eval("0^0"), 1.0, 1e-9, "0^0 is well-defined as 1, not rejected");
    EXPECT_NEAR(eval("2^10"), 1024.0, 1e-9, "ordinary large-ish exponent still works fine");

    EXPECT_NEAR(eval("pi"), 3.14159265358979323846, 1e-9, "pi constant");
    EXPECT_NEAR(eval("e"), 2.71828182845904523536, 1e-9, "e constant");
    EXPECT_NEAR(eval("2*pi"), 6.283185307179586, 1e-9, "constants combine with operators");

    EXPECT_NEAR(eval("ans+1", 41.0), 42.0, 1e-9, "ans refers to the injected previous result");
    EXPECT_NEAR(eval("ans", 0.0), 0.0, 1e-9, "ans defaults to 0 when nothing came before");

    EXPECT_NEAR(eval("sqrt(9)"), 3.0, 1e-9, "sqrt(9) = 3");
    EXPECT_NEAR(eval("sqrt(2)*sqrt(2)"), 2.0, 1e-6, "sqrt(2)*sqrt(2) = 2");
    EXPECT_THROWS(eval("sqrt(-1)"), ParseError, "sqrt of a negative number throws ParseError");
    EXPECT_NEAR(eval("abs(-5)"), 5.0, 1e-9, "abs(-5) = 5");
    EXPECT_NEAR(eval("ln(e)"), 1.0, 1e-9, "ln(e) = 1");
    EXPECT_NEAR(eval("log(100)"), 2.0, 1e-9, "log(100) = 2 (base 10)");
    EXPECT_THROWS(eval("ln(0)"), ParseError, "ln(0) throws ParseError");
    EXPECT_THROWS(eval("log(-5)"), ParseError, "log of a negative number throws ParseError");
    EXPECT_NEAR(eval("sqrt(4+5)"), 3.0, 1e-9, "function argument can be a full expression");
    EXPECT_THROWS(eval("sqrt(4"), ParseError, "unclosed function call throws ParseError");
    EXPECT_THROWS(eval("foo(1)"), LexError, "unknown function name throws LexError");

    EXPECT_NEAR(eval("sin(0)"), 0.0, 1e-9, "sin(0) = 0 in radians");
    EXPECT_NEAR(eval("cos(pi)"), -1.0, 1e-9, "cos(pi) = -1 in radians");
    EXPECT_NEAR(eval("sin(90)", 0.0, AngleMode::Degrees), 1.0, 1e-9, "sin(90deg) = 1 in degree mode");
    EXPECT_NEAR(eval("cos(180)", 0.0, AngleMode::Degrees), -1.0, 1e-9, "cos(180deg) = -1 in degree mode");
    EXPECT_NEAR(eval("asin(1)", 0.0, AngleMode::Degrees), 90.0, 1e-9, "asin(1) = 90deg in degree mode");
    EXPECT_THROWS(eval("asin(2)"), ParseError, "asin outside [-1,1] throws ParseError");

    EXPECT_NEAR(eval("5!"), 120.0, 1e-9, "5! = 120");
    EXPECT_NEAR(eval("0!"), 1.0, 1e-9, "0! = 1");
    EXPECT_NEAR(eval("3!+1"), 7.0, 1e-9, "3!+1 = 7 (factorial binds tighter than +)");
    EXPECT_NEAR(eval("(2+3)!"), 120.0, 1e-9, "factorial applies to a parenthesized group");
    EXPECT_THROWS(eval("(-1)!"), ParseError, "factorial of a negative number throws ParseError");
    EXPECT_THROWS(eval("2.5!"), ParseError, "factorial of a non-integer throws ParseError");

    EXPECT_NEAR(eval("cbrt(27)"), 3.0, 1e-9, "cbrt(27) = 3");
    EXPECT_NEAR(eval("cbrt(-27)"), -3.0, 1e-9, "cbrt(-27) = -3 (real cube root of a negative)");
    EXPECT_NEAR(eval("sinh(0)"), 0.0, 1e-9, "sinh(0) = 0");
    EXPECT_NEAR(eval("cosh(0)"), 1.0, 1e-9, "cosh(0) = 1");
    EXPECT_NEAR(eval("tanh(0)"), 0.0, 1e-9, "tanh(0) = 0");

    EXPECT_NEAR(eval("gcd(12, 18)"), 6.0, 1e-9, "gcd(12, 18) = 6");
    EXPECT_NEAR(eval("gcd(0, 5)"), 5.0, 1e-9, "gcd(0, 5) = 5");
    EXPECT_NEAR(eval("lcm(4, 6)"), 12.0, 1e-9, "lcm(4, 6) = 12");
    EXPECT_NEAR(eval("lcm(0, 6)"), 0.0, 1e-9, "lcm(0, 6) = 0");
    EXPECT_NEAR(eval("ncr(5, 2)"), 10.0, 1e-9, "ncr(5, 2) = 10");
    EXPECT_NEAR(eval("ncr(6, 0)"), 1.0, 1e-9, "ncr(6, 0) = 1");
    EXPECT_NEAR(eval("ncr(6, 6)"), 1.0, 1e-9, "ncr(6, 6) = 1");
    EXPECT_NEAR(eval("npr(5, 2)"), 20.0, 1e-9, "npr(5, 2) = 20");
    EXPECT_THROWS(eval("ncr(5, 8)"), ParseError, "ncr with r > n throws ParseError");
    EXPECT_THROWS(eval("ncr(5, -1)"), ParseError, "ncr with negative r throws ParseError");
    EXPECT_THROWS(eval("gcd(1.5, 2)"), ParseError, "gcd of a non-integer throws ParseError");
    EXPECT_THROWS(eval("ncr(5)"), ParseError, "two-argument function called with one argument throws ParseError");
    EXPECT_NEAR(eval("ncr(4+1, 2*1)"), 10.0, 1e-9, "two-argument function arguments can be expressions");
}
