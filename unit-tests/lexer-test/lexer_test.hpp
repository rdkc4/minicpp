#ifndef LEXER_TEST_HPP
#define LEXER_TEST_HPP

#include "../../lexer/lexer.hpp"
#include <cassert>

class LexerTest : public lex::Lexer {
    public:
        LexerTest(std::vector<std::string>& input) : lex::Lexer{ input } {}

        const syntax::Token& at(size_t i) const{
            assert(i < tokens.size());
            return tokens[i];
        }

        size_t tokensSize() const noexcept {
            return tokens.size();
        }

        void setTokens(std::vector<syntax::Token>& _tokens){
            tokens = std::move(_tokens);
        }
};

#endif