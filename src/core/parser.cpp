#include "parser.h"
#include <cmath>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kDegToRad = kPi / 180.0;
constexpr double kRadToDeg = 180.0 / kPi;
}

Parser::Parser(std::vector<Token> t, double ansValue, AngleMode mode)
    : tokens(std::move(t)), ans(ansValue), angleMode(mode) {}

const Token& Parser::peek() const {
    return tokens[pos];
}

const Token& Parser::advance() {
    const Token& t = tokens[pos];
    if (pos + 1 < tokens.size()) pos++;
    return t;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::expect(TokenType type, const std::string& msg) {
    if (!check(type)) error(msg);
    return advance();
}

void Parser::error(const std::string& msg) const {
    throw ParseError(msg + " (posisi " + std::to_string(peek().position) + ")");
}

// nan/inf gak dicek otomatis, jadi divalidasi manual di sini
double Parser::checkFinite(double value, const std::string& context) const {
    if (std::isnan(value)) {
        error("Hasil tidak terdefinisi (NaN) pada " + context);
    }
    if (std::isinf(value)) {
        error("Hasil terlalu besar (di luar jangkauan) pada " + context);
    }
    return value;
}

double Parser::applyFunction(TokenType func, double arg) const {
    switch (func) {
        case TokenType::Sqrt:
            if (arg < 0.0) error("sqrt() dari bilangan negatif tidak terdefinisi");
            return std::sqrt(arg);
        case TokenType::Cbrt:
            return std::cbrt(arg);
        case TokenType::Sin:
            return std::sin(angleMode == AngleMode::Degrees ? arg * kDegToRad : arg);
        case TokenType::Cos:
            return std::cos(angleMode == AngleMode::Degrees ? arg * kDegToRad : arg);
        case TokenType::Tan:
            return std::tan(angleMode == AngleMode::Degrees ? arg * kDegToRad : arg);
        case TokenType::Sinh:
            return std::sinh(arg);
        case TokenType::Cosh:
            return std::cosh(arg);
        case TokenType::Tanh:
            return std::tanh(arg);
        case TokenType::Asin: {
            if (arg < -1.0 || arg > 1.0) error("asin() hanya terdefinisi untuk [-1, 1]");
            double r = std::asin(arg);
            return angleMode == AngleMode::Degrees ? r * kRadToDeg : r;
        }
        case TokenType::Acos: {
            if (arg < -1.0 || arg > 1.0) error("acos() hanya terdefinisi untuk [-1, 1]");
            double r = std::acos(arg);
            return angleMode == AngleMode::Degrees ? r * kRadToDeg : r;
        }
        case TokenType::Atan: {
            double r = std::atan(arg);
            return angleMode == AngleMode::Degrees ? r * kRadToDeg : r;
        }
        case TokenType::Ln:
            if (arg <= 0.0) error("ln() hanya terdefinisi untuk bilangan positif");
            return std::log(arg);
        case TokenType::Log:
            if (arg <= 0.0) error("log() hanya terdefinisi untuk bilangan positif");
            return std::log10(arg);
        case TokenType::Abs:
            return std::fabs(arg);
        default:
            error("Fungsi tidak dikenal");
    }
}

namespace {
bool isSafeInteger(double v) {
    return std::floor(v) == v && std::fabs(v) < 1e15;
}
}

double Parser::applyTwoArgFunction(TokenType func, double a, double b) const {
    switch (func) {
        case TokenType::Gcd: {
            if (!isSafeInteger(a) || !isSafeInteger(b)) {
                error("gcd() hanya berlaku untuk bilangan bulat");
            }
            long long x = static_cast<long long>(std::fabs(a));
            long long y = static_cast<long long>(std::fabs(b));
            while (y != 0) {
                long long t = y;
                y = x % y;
                x = t;
            }
            return static_cast<double>(x);
        }
        case TokenType::Lcm: {
            if (!isSafeInteger(a) || !isSafeInteger(b)) {
                error("lcm() hanya berlaku untuk bilangan bulat");
            }
            long long x = static_cast<long long>(std::fabs(a));
            long long y = static_cast<long long>(std::fabs(b));
            if (x == 0 || y == 0) return 0.0;
            long long g = x, r = y;
            while (r != 0) { long long t = r; r = g % r; g = t; }
            return static_cast<double>(x / g) * static_cast<double>(y);
        }
        case TokenType::NCr:
        case TokenType::NPr: {
            if (!isSafeInteger(a) || !isSafeInteger(b) || a < 0 || b < 0 || b > a) {
                error("nCr()/nPr() membutuhkan 0 <= r <= n, keduanya bilangan bulat");
            }
            // pakai rumus perkalian bertahap biar gak overflow kayak n!/(n-r)!
            double result = 1.0;
            for (int i = 0; i < static_cast<int>(b); i++) {
                result *= (a - i);
                if (func == TokenType::NCr) result /= (i + 1);
                checkFinite(result, func == TokenType::NCr ? "nCr" : "nPr");
            }
            // dibulatkan karena hasil bisa meleset dikit akibat pembagian float
            if (func == TokenType::NCr) result = std::round(result);
            return result;
        }
        default:
            error("Fungsi dua-argumen tidak dikenal");
    }
}

double Parser::parse() {
    double result = parseExpression();
    if (!check(TokenType::End)) {
        error("Token tak terduga setelah ekspresi");
    }
    return result;
}

double Parser::parseExpression() {
    double value = parseTerm();
    while (check(TokenType::Plus) || check(TokenType::Minus)) {
        bool isPlus = check(TokenType::Plus);
        advance();
        double rhs = parseTerm();
        value = isPlus ? value + rhs : value - rhs;
        checkFinite(value, isPlus ? "penjumlahan" : "pengurangan");
    }
    return value;
}

double Parser::parseTerm() {
    double value = parseUnary();
    while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent)) {
        TokenType op = peek().type;
        advance();
        double rhs = parseUnary();
        if (op == TokenType::Star) {
            value *= rhs;
            checkFinite(value, "perkalian");
        } else if (op == TokenType::Slash) {
            if (rhs == 0.0) error("Tidak bisa membagi dengan nol");
            value /= rhs;
            checkFinite(value, "pembagian");
        } else {
            if (rhs == 0.0) error("Tidak bisa modulo dengan nol");
            value = std::fmod(value, rhs);
            checkFinite(value, "modulo");
        }
    }
    return value;
}

double Parser::parseUnary() {
    if (match(TokenType::Minus)) {
        return -parseUnary();
    }
    if (match(TokenType::Plus)) {
        return parseUnary();
    }
    return parseExponent();
}

double Parser::parseExponent() {
    double base = parsePostfix();
    if (match(TokenType::Caret)) {
        // right-associative, biar 2^-2 kebaca bener
        double exponent = parseUnary();
        base = std::pow(base, exponent);
        checkFinite(base, "pemangkatan");
    }
    return base;
}

double Parser::parsePostfix() {
    double value = parsePrimary();
    while (match(TokenType::Bang)) {
        if (value < 0.0 || std::floor(value) != value) {
            error("Faktorial hanya berlaku untuk bilangan bulat non-negatif");
        }
        if (value > 170.0) {
            error("Faktorial terlalu besar (di luar jangkauan)");
        }
        double result = 1.0;
        for (int i = 2; i <= static_cast<int>(value); i++) {
            result *= i;
        }
        value = checkFinite(result, "faktorial");
    }
    return value;
}

double Parser::parsePrimary() {
    if (check(TokenType::Number)) {
        return advance().value;
    }
    if (match(TokenType::Pi)) {
        return kPi;
    }
    if (match(TokenType::Euler)) {
        return 2.71828182845904523536;
    }
    if (match(TokenType::Ans)) {
        return ans;
    }
    if (check(TokenType::Sqrt) || check(TokenType::Cbrt) || check(TokenType::Sin) ||
        check(TokenType::Cos) || check(TokenType::Tan) || check(TokenType::Asin) ||
        check(TokenType::Acos) || check(TokenType::Atan) || check(TokenType::Sinh) ||
        check(TokenType::Cosh) || check(TokenType::Tanh) || check(TokenType::Ln) ||
        check(TokenType::Log) || check(TokenType::Abs)) {
        TokenType func = advance().type;
        expect(TokenType::LParen, "Diharapkan '(' setelah nama fungsi");
        double arg = parseExpression();
        expect(TokenType::RParen, "Kurung tidak seimbang pada pemanggilan fungsi");
        return checkFinite(applyFunction(func, arg), "fungsi");
    }
    if (check(TokenType::NCr) || check(TokenType::NPr) || check(TokenType::Gcd) ||
        check(TokenType::Lcm)) {
        TokenType func = advance().type;
        expect(TokenType::LParen, "Diharapkan '(' setelah nama fungsi");
        double a = parseExpression();
        expect(TokenType::Comma, "Fungsi ini butuh dua argumen, dipisah koma");
        double b = parseExpression();
        expect(TokenType::RParen, "Kurung tidak seimbang pada pemanggilan fungsi");
        return checkFinite(applyTwoArgFunction(func, a, b), "fungsi");
    }
    if (match(TokenType::LParen)) {
        double value = parseExpression();
        expect(TokenType::RParen, "Kurung tidak seimbang, '(' tanpa pasangan ')'");
        return value;
    }
    if (check(TokenType::End)) {
        error("Ekspresi tidak lengkap");
    }
    error("Token tak terduga, diharapkan angka, fungsi, atau '('");
}
