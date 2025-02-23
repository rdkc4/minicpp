#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "../common/token/token.hpp"

class Lexer{
    public:

        Lexer(const std::string& input);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for lexical check and tokenization
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void tokenize();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token retrieval (used by parser)
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        Token nextToken();

        Token peekAtNext();
        
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void printTokens(const std::vector<Token>& tokens) const;

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
        std::vector<Token> tokens;
        size_t nextTokenIdx;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // maintaining position, line number and previous line length values
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void updatePosition(char curr);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token readers
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void getID();

        void getLiteral(bool sign = false);

        void getAssignOperator(char curr);

        void getBitwiseOperator(char curr);

        void getAritOperator(char curr);

        void getRelOperator();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token checking
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        bool isAssignOperator(char curr) const;

        bool isKeyword(const std::string& value) const;

        bool isSignedLiteral(char curr) const;

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
