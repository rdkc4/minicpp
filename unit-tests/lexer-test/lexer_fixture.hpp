#ifndef LEXER_FIXTURE_HPP
#define LEXER_FIXTURE_HPP

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "lexer_test.hpp"

class LexerFixture : public ::testing::Test {
protected:
    std::vector<std::string> input;
    std::unique_ptr<LexerTest> lexer;

    void initLexer() {
        lexer = std::make_unique<LexerTest>(input);
        lexer->tokenize();
    }

};

#endif