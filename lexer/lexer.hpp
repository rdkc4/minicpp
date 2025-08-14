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

        // token handling
        void next() noexcept;
        const Token& peek() const noexcept;
        const Token& current() const noexcept;
        bool completedTokenization() const noexcept;
        
        void printTokens() const noexcept;

    protected:
    
        // input - raw text
        std::string input;
        
        // current position of the input
        size_t position;
        size_t lineNumber;
        size_t prevLineLen; // (position - prevLineLen) -> column in the current line

        size_t nextTokenIdx;
        std::vector<Token> tokens;
        std::vector<std::string> exceptions;

        void checkLexicalErrors() const;

        // maintaining lines / columns
        void updatePosition() noexcept;
        void updatePosition(size_t n) noexcept;
        void updateLine() noexcept;

        // token readers
        void getID();
        void getLiteral(bool sign = false);
        void getAssignOperator();
        void getBitwiseOperator();
        void getAritOperator();
        void getRelOperator();

        // checking for token
        bool isAssignOperator() const noexcept;
        bool isKeyword(const std::string& value) const noexcept;
        bool isSignedLiteral() const noexcept;
        bool isAritOperator() const;
        bool isBitwiseOperator() const;
        bool isRelOperator() const;

        // comments (not tokenized, ignored)
        void singleLineComment();
        void multiLineComment();

};

#endif
