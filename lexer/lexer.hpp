#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>
#include <stdexcept>

#include "../common/defs.hpp"
#include "../common/token.hpp"

extern const std::unordered_map<std::string, TokenType> keywords;

class Lexer{
    public:
        Lexer(const std::string& input);

        void tokenize();

        Token nextToken();

        Token peekAtNext();
        
        void printTokens(const std::vector<Token>& tokens) const;

    private:
        std::string input;
        size_t position;

        size_t lineNumber;
        size_t prevLineLen;

        std::vector<Token> tokens;
        size_t _nextToken = 0;

        void updatePosition(char curr);

        void getID();

        void getLiteral(bool sign = false);

        void getAssignOperator(char curr);

        void getBitwiseOperator(char curr);

        void getAritOperator(char curr);

        void getRelOperator();

        bool isAssignOperator(char curr) const;

        bool isKeyword(const std::string& value) const;

        bool isSignedLiteral(char curr) const;

        bool isAritOperator(char curr) const;

        bool isBitwiseOperator(char curr) const;

        bool isRelOperator(char curr) const;

        void singleLineComment();

        void multiLineComment();

};

#endif