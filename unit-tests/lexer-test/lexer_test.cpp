#include <gtest/gtest.h>
#include <utility>
#include <array>
#include <string_view>

#include "lexer_fixture.hpp"

TEST_F(LexerFixture, TokenizationOfAssignmentStatement){
    input = {"int x = -1 + 2 << 3u;"};
    initLexer();

    constexpr std::array<std::pair<syntax::TokenType, std::string_view>, 10> expectedTokens {{
        {syntax::TokenType::INT, "int"}, 
        {syntax::TokenType::ID, "x"}, 
        {syntax::TokenType::ASSIGN, "="}, 
        {syntax::TokenType::LITERAL, "-1"}, 
        {syntax::TokenType::PLUS, "+"}, 
        {syntax::TokenType::LITERAL, "2"}, 
        {syntax::TokenType::LSHIFT, "<<"}, 
        {syntax::TokenType::LITERAL, "3u"}, 
        {syntax::TokenType::SEMICOLON, ";"}, 
        {syntax::TokenType::_EOF, ""}
    }};

    constexpr size_t expectedSize{ expectedTokens.size() };
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

    constexpr std::array<std::pair<syntax::TokenType, std::string_view>, 12> expectedTokens {{
        {syntax::TokenType::IF, "if"}, 
        {syntax::TokenType::LPAREN, "("}, 
        {syntax::TokenType::ID, "a"}, 
        {syntax::TokenType::GREATER, ">"}, 
        {syntax::TokenType::ID, "b"}, 
        {syntax::TokenType::RPAREN, ")"}, 
        {syntax::TokenType::LBRACE, "{"}, 
        {syntax::TokenType::RETURN, "return"}, 
        {syntax::TokenType::ID, "a"}, 
        {syntax::TokenType::SEMICOLON, ";"},
        {syntax::TokenType::RBRACE, "}"}, 
        {syntax::TokenType::_EOF, ""}
    }};
    constexpr size_t expectedSize{ expectedTokens.size() };
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
    ASSERT_EQ(lexer->at(0).type, syntax::TokenType::_EOF);
}

TEST_F(LexerFixture, OnlySingleLineComment){
    input = {"//this is a comment"};
    initLexer();

    ASSERT_EQ(lexer->tokensSize(), 1);
    ASSERT_EQ(lexer->at(0).type, syntax::TokenType::_EOF);
}

TEST_F(LexerFixture, OnlyMultiLineComment){
    input = {"/*this is a comment\n a multi line comment\n */"};
    initLexer();

    ASSERT_EQ(lexer->tokensSize(), 1);
    ASSERT_EQ(lexer->at(0).type, syntax::TokenType::_EOF);
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