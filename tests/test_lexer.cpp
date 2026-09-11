#include "lexer.h"
#include "test_framework.h"

void run_lexer_tests() {
    testing::suite("Lexer");

    {
        Lexer lexer("12 + 3.5");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 4u, "tokenizes '12 + 3.5' into 4 tokens (incl. End)");
        EXPECT_EQ(tokens[0].type, TokenType::Number, "first token is a Number");
        EXPECT_NEAR(tokens[0].value, 12.0, 1e-9, "first number value is 12");
        EXPECT_EQ(tokens[1].type, TokenType::Plus, "second token is Plus");
        EXPECT_EQ(tokens[2].type, TokenType::Number, "third token is a Number");
        EXPECT_NEAR(tokens[2].value, 3.5, 1e-9, "third number value is 3.5");
        EXPECT_EQ(tokens[3].type, TokenType::End, "stream ends with End token");
    }

    {
        Lexer lexer("(1-2)*3/4%5^6");
        auto tokens = lexer.tokenize();
        std::vector<TokenType> expected = {
            TokenType::LParen, TokenType::Number, TokenType::Minus, TokenType::Number,
            TokenType::RParen, TokenType::Star, TokenType::Number, TokenType::Slash,
            TokenType::Number, TokenType::Percent, TokenType::Number, TokenType::Caret,
            TokenType::Number, TokenType::End};
        bool matches = tokens.size() == expected.size();
        if (matches) {
            for (size_t i = 0; i < expected.size(); i++) {
                if (tokens[i].type != expected[i]) {
                    matches = false;
                    break;
                }
            }
        }
        EXPECT_TRUE(matches, "recognizes every operator and paren symbol");
    }

    {
        Lexer lexer("");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 1u, "empty input yields just an End token");
        EXPECT_EQ(tokens[0].type, TokenType::End, "the only token is End");
    }

    {
        Lexer lexer("   7   ");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 2u, "whitespace around a number is skipped");
        EXPECT_NEAR(tokens[0].value, 7.0, 1e-9, "value parses to 7");
    }

    EXPECT_THROWS(Lexer("1.2.3").tokenize(), LexError,
                  "two decimal points in one number throws LexError");
    EXPECT_THROWS(Lexer("5 & 3").tokenize(), LexError,
                  "unrecognized character '&' throws LexError");

    {
        Lexer lexer("1e10");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 2u, "'1e10' tokenizes as a single Number + End");
        EXPECT_NEAR(tokens[0].value, 1e10, 1e-3, "'1e10' parses to 1e10");
    }
    {
        Lexer lexer("2.5E-3");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 2u, "'2.5E-3' tokenizes as a single Number + End");
        EXPECT_NEAR(tokens[0].value, 2.5e-3, 1e-9, "'2.5E-3' parses to 0.0025");
    }
    {
        // "5e" gak lengkap notasi ilmiahnya, jadi 'e' dibaca sebagai konstanta euler
        Lexer lexer("5e");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 3u, "'5e' tokenizes as Number, Euler, End");
        EXPECT_EQ(tokens[0].type, TokenType::Number, "first token is the Number 5");
        EXPECT_EQ(tokens[1].type, TokenType::Euler, "second token is the Euler constant");
    }

    {
        Lexer lexer("sqrt(9) + pi * e - ans");
        auto tokens = lexer.tokenize();
        std::vector<TokenType> expected = {
            TokenType::Sqrt, TokenType::LParen, TokenType::Number, TokenType::RParen,
            TokenType::Plus, TokenType::Pi, TokenType::Star, TokenType::Euler,
            TokenType::Minus, TokenType::Ans, TokenType::End};
        bool matches = tokens.size() == expected.size();
        if (matches) {
            for (size_t i = 0; i < expected.size(); i++) {
                if (tokens[i].type != expected[i]) {
                    matches = false;
                    break;
                }
            }
        }
        EXPECT_TRUE(matches, "recognizes function names and constants");
    }
    EXPECT_THROWS(Lexer("foo(1)").tokenize(), LexError,
                  "unrecognized identifier throws LexError");
    {
        Lexer lexer("5!");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 3u, "'5!' tokenizes as Number, Bang, End");
        EXPECT_EQ(tokens[1].type, TokenType::Bang, "second token is Bang");
    }
    {
        Lexer lexer("ncr(5, 2)");
        auto tokens = lexer.tokenize();
        EXPECT_EQ(tokens.size(), 7u, "'ncr(5, 2)' tokenizes into 7 tokens (incl. End)");
        EXPECT_EQ(tokens[0].type, TokenType::NCr, "first token is NCr");
        EXPECT_EQ(tokens[2].type, TokenType::Number, "third token is a Number");
        EXPECT_EQ(tokens[3].type, TokenType::Comma, "fourth token is Comma");
    }
}
