#include <gtest/gtest.h>
#include <utility>
#include <string>
#include <vector>

#include "lexer_fixture.hpp"

TEST_F(LexerFixture, TokenizationOfAssignmentStatement){
    input = {"int x = -1 + 2 << 3u;"};
    initLexer();

    std::vector<std::pair<TokenType, std::string>> expectedTokens { 
        {TokenType::INT, "int"}, 
        {TokenType::ID, "x"}, 
        {TokenType::ASSIGN, "="}, 
        {TokenType::LITERAL, "-1"}, 
        {TokenType::PLUS, "+"}, 
        {TokenType::LITERAL, "2"}, 
        {TokenType::LSHIFT, "<<"}, 
        {TokenType::LITERAL, "3u"}, 
        {TokenType::SEMICOLON, ";"}, 
        {TokenType::_EOF, ""}
    };

    size_t expectedSize{ expectedTokens.size() };
    ASSERT_EQ(lexer->tokensSize(), expectedSize);

    for(size_t i = 0; i < expectedSize; ++i){
        SCOPED_TRACE("Token index: " + std::to_string(i));
        EXPECT_EQ(lexer->at(i).type, expectedTokens[i].first);
        EXPECT_EQ(lexer->at(i).value, expectedTokens[i].second);
    }
}

TEST_F(LexerFixture, TokenizationOfIfStatement){
    input = {"if(a > b){return a;}"};
    initLexer();

    std::vector<std::pair<TokenType, std::string>> expectedTokens { 
        {TokenType::IF, "if"}, 
        {TokenType::LPAREN, "("}, 
        {TokenType::ID, "a"}, 
        {TokenType::GREATER, ">"}, 
        {TokenType::ID, "b"}, 
        {TokenType::RPAREN, ")"}, 
        {TokenType::LBRACE, "{"}, 
        {TokenType::RETURN, "return"}, 
        {TokenType::ID, "a"}, 
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"}, 
        {TokenType::_EOF, ""}
    };
    const size_t expectedSize{ expectedTokens.size() };
    ASSERT_EQ(lexer->tokensSize(), expectedSize);

    for(size_t i = 0; i < expectedSize; ++i){
        SCOPED_TRACE("Token index: " + std::to_string(i));
        EXPECT_EQ(lexer->at(i).type, expectedTokens[i].first);
        EXPECT_EQ(lexer->at(i).value, expectedTokens[i].second);
    }
}

TEST_F(LexerFixture, EmptyInput){
    input = {""};
    initLexer();

    ASSERT_EQ(lexer->tokensSize(), 1);
    ASSERT_EQ(lexer->at(0).type, TokenType::_EOF);
}

TEST_F(LexerFixture, OnlySingleLineComment){
    input = {"//this is a comment"};
    initLexer();

    ASSERT_EQ(lexer->tokensSize(), 1);
    ASSERT_EQ(lexer->at(0).type, TokenType::_EOF);
}

TEST_F(LexerFixture, OnlyMultiLineComment){
    input = {"/*this is a comment\n a multi line comment\n */"};
    initLexer();

    ASSERT_EQ(lexer->tokensSize(), 1);
    ASSERT_EQ(lexer->at(0).type, TokenType::_EOF);
}

TEST_F(LexerFixture, ThrowsOnInvalidTokens){
    input = {"int x. = 123;"};
    initLexer();

    ASSERT_TRUE(lexer->hasErrors());
}

TEST_F(LexerFixture, ThrowsOnInvalidMultiLineComment){
    input = {"/* int x = 3;"};
    initLexer();

    ASSERT_TRUE(lexer->hasErrors());
}