#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>
#include <string>
#include <vector>

#include "lexer_test.hpp"

TEST(LexerTest, TokenizationOfAssignmentStatement){
    LexerTest lexer{ "int x = -1 + 2 << 3u;" };
    lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expectedTokens{ {TokenType::_INT, "int"}, {TokenType::_ID, "x"}, 
        {TokenType::_ASSIGN, "="}, {TokenType::_LITERAL, "-1"}, {TokenType::_AROP, "+"}, {TokenType::_LITERAL, "2"}, 
        {TokenType::_BITWISE, "<<"}, {TokenType::_LITERAL, "3u"}, {TokenType::_SEMICOLON, ";"}, {TokenType::_EOF, ""}
    };
    const size_t expectedSize = expectedTokens.size();

    ASSERT_EQ(lexer.tokensSize(), expectedSize);
    for(size_t i = 0; i < expectedSize; ++i){
        SCOPED_TRACE("Token index: " + std::to_string(i));
        ASSERT_EQ(lexer.at(i).type, expectedTokens[i].first);
        ASSERT_EQ(lexer.at(i).value, expectedTokens[i].second);
    }
}

TEST(LexerTest, TokenizationOfIfStatement){
    LexerTest lexer{ "if(a > b){return a;}" };
    lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expectedTokens{ {TokenType::_IF, "if"}, {TokenType::_LPAREN, "("}, 
        {TokenType::_ID, "a"}, {TokenType::_RELOP, ">"}, {TokenType::_ID, "b"}, {TokenType::_RPAREN, ")"}, 
        {TokenType::_LBRACKET, "{"}, {TokenType::_RETURN, "return"}, {TokenType::_ID, "a"}, {TokenType::_SEMICOLON, ";"},
        {TokenType::_RBRACKET, "}"}, {TokenType::_EOF, ""}
    };
    const size_t expectedSize = expectedTokens.size();

    ASSERT_EQ(lexer.tokensSize(), expectedSize);
    for(size_t i = 0; i < expectedSize; ++i){
        SCOPED_TRACE("Token index: " + std::to_string(i));
        ASSERT_EQ(lexer.at(i).type, expectedTokens[i].first);
        ASSERT_EQ(lexer.at(i).value, expectedTokens[i].second);
    }
}

TEST(LexerTest, EmptyInput){
    LexerTest lexer{""};
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, OnlySingleLineComment){
    LexerTest lexer{"//this is a comment"};
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, OnlyMultiLineComment){
    LexerTest lexer{"/*this is a comment\n a multi line comment\n */"};
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, ThrowsOnInvalidTokens){
    LexerTest lexer{ "int x. = 123;"};
    EXPECT_THROW(lexer.tokenize(), std::runtime_error);
}

TEST(LexerTest, ThrowsOnInvalidMultiLineComment){
    LexerTest lexer{ "/* int x = 3;"};
    EXPECT_THROW(lexer.tokenize(), std::runtime_error);
}