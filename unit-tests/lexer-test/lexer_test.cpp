#include <gtest/gtest.h>
#include <utility>
#include <string>
#include <vector>

#include "lexer_test.hpp"

TEST(LexerTest, TokenizationOfAssignmentStatement){
    std::vector<std::string> input{"int x = -1 + 2 << 3u;"};
    LexerTest lexer{ input };
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
    std::vector<std::string> input{"if(a > b){return a;}"};
    LexerTest lexer{ input };
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