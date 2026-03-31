#include <gtest/gtest.h>
#include <utility>
#include <string>
#include <vector>

#include "lexer_test.hpp"

TEST(LexerTest, TokenizationOfAssignmentStatement){
    std::vector<std::string> input{"int x = -1 + 2 << 3u;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expectedTokens{ {TokenType::INT, "int"}, {TokenType::ID, "x"}, 
        {TokenType::ASSIGN, "="}, {TokenType::LITERAL, "-1"}, {TokenType::ARITHMETIC, "+"}, {TokenType::LITERAL, "2"}, 
        {TokenType::BITWISE, "<<"}, {TokenType::LITERAL, "3u"}, {TokenType::SEMICOLON, ";"}, {TokenType::_EOF, ""}
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
    std::vector<std::string> input{"if(a > b){return a;}"};
    LexerTest lexer{ input };
    lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expectedTokens{ {TokenType::IF, "if"}, {TokenType::LPAREN, "("}, 
        {TokenType::ID, "a"}, {TokenType::RELATIONAL, ">"}, {TokenType::ID, "b"}, {TokenType::RPAREN, ")"}, 
        {TokenType::LBRACKET, "{"}, {TokenType::RETURN, "return"}, {TokenType::ID, "a"}, {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACKET, "}"}, {TokenType::_EOF, ""}
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
    std::vector<std::string> input{""};
    LexerTest lexer{ input };
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, OnlySingleLineComment){
    std::vector<std::string> input{"//this is a comment"};
    LexerTest lexer{ input };
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, OnlyMultiLineComment){
    std::vector<std::string> input{"/*this is a comment\n a multi line comment\n */"};
    LexerTest lexer{ input };
    lexer.tokenize();

    ASSERT_EQ(lexer.tokensSize(), 1);
    ASSERT_EQ(lexer.at(0).type, TokenType::_EOF);
}

TEST(LexerTest, ThrowsOnInvalidTokens){
    std::vector<std::string> input{"int x. = 123;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    ASSERT_TRUE(lexer.hasErrors());
}

TEST(LexerTest, ThrowsOnInvalidMultiLineComment){
    std::vector<std::string> input{"/* int x = 3;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    ASSERT_TRUE(lexer.hasErrors());
}