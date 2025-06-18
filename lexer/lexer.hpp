#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>
#include <vector>

#include "../common/token/token.hpp"

/*
    Lexical analysis of the input
    Translating input into tokens
*/
class Lexer{
    public:

        Lexer(std::string_view input);

        // tokenization of the input string
        // throws at the end if the number of invalid tokens is greater than 0
        void tokenize();

        const Token& nextToken() noexcept;
        const Token& peekAtNext() const noexcept;
        bool completedTokenization() const noexcept;
        
        void printTokens() const noexcept;

    private:
    
        // input - raw text
        std::string input;
        
        // current position of the input
        size_t position;
        size_t lineNumber;
        size_t prevLineLen; // (position - prevLineLen) -> column in the current line

        size_t nextTokenIdx;
        std::vector<Token> tokens;

        // maintaining lines / columns
        void updatePosition() noexcept;
        void updatePosition(size_t n) noexcept;
        void updateLine() noexcept;

        // token readers
        void getID();
        void getLiteral(bool sign = false);
        void getAssignOperator(char curr);
        void getBitwiseOperator(char curr);
        void getAritOperator(char curr);
        void getRelOperator();

        // checking for token
        bool isAssignOperator(char curr) const noexcept;
        bool isKeyword(const std::string& value) const noexcept;
        bool isSignedLiteral(char curr) const noexcept;
        bool isAritOperator(char curr) const;
        bool isBitwiseOperator(char curr) const;
        bool isRelOperator(char curr) const;

        // comments (not tokenized, ignored)
        void singleLineComment();
        void multiLineComment();

};

#endif
