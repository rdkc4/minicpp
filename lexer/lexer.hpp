#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>
#include <vector>

#include "../common/token/token.hpp"

class Lexer{
    public:

        Lexer(std::string_view input);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for lexical check and tokenization
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void tokenize();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token retrieval (used by parser)
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        const Token& nextToken() noexcept;

        const Token& peekAtNext() const noexcept;
        
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void printTokens() const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // input - text read from standard input
        // position - current position in input
        // line number - current line number
        // previous line length - position of last "\n"
        // tokens - tokens detected in input
        // next token index - "pointer" to a token that should be processed next
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string input;
        size_t position;
        size_t lineNumber;
        size_t prevLineLen;
        size_t nextTokenIdx;
        std::vector<Token> tokens;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // maintaining position, line number and previous line length values
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void updatePosition() noexcept;

        void updatePosition(size_t n) noexcept;

        void updateLine() noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token readers
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void getID();

        void getLiteral(bool sign = false);

        void getAssignOperator(char curr);

        void getBitwiseOperator();

        void getAritOperator(char curr);

        void getRelOperator();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token checking
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        bool isAssignOperator(char curr) const noexcept;

        bool isKeyword(const std::string& value) const noexcept;

        bool isSignedLiteral(char curr) const noexcept;

        bool isAritOperator(char curr) const;

        bool isBitwiseOperator(char curr) const;

        bool isRelOperator(char curr) const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // comments
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void singleLineComment();

        void multiLineComment();

};

#endif
