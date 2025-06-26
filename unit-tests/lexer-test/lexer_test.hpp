#ifndef LEXER_TEST_HPP
#define LEXER_TEST_HPP

#include "../../lexer/lexer.hpp"

class LexerTest : public Lexer {
    public:
        LexerTest(std::string_view input) : Lexer{ input } {}
        const Token& at(size_t i) const{
            return tokens[i];
        }

        size_t tokensSize() const noexcept {
            return tokens.size();
        }

        void setTokens(std::vector<Token>& _tokens){
            tokens = std::move(_tokens);
        }
};

#endif