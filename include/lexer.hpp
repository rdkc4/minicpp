#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include "defs.hpp"

class Lexer{
    public:
        Lexer(const std::string& input);

        void tokenize();

        Token nextToken();

        Token peekAtNext();
        
        size_t getNextTokenPosition() const;
        void setNextTokenPosition(const size_t nt);

        void printTokens(const std::vector<Token>& tokens) const;

    private:
        std::string input;
        size_t position;

        size_t lineNumber;
        size_t prevLineLen;

        std::vector<Token> tokens;
        size_t _nextToken = 0;

        const std::unordered_map<std::string, TokenType> keywords = {
            {"return", TokenType::_RETURN},
            {"if", TokenType::_IF},
            {"else", TokenType::_ELSE},
            {"int", TokenType::_TYPE},
            {"unsigned", TokenType::_TYPE},
            {"void", TokenType::_TYPE},
            {"while", TokenType::_WHILE},
            {"for", TokenType::_FOR}
        };

        const std::unordered_map<std::string, RelationalOperator> relOperators = {
            {"<", RelationalOperator::LT},
            {">", RelationalOperator::GT},
            {"<=", RelationalOperator::LE},
            {">=", RelationalOperator::GE},
            {"==", RelationalOperator::EQ},
            {"!=", RelationalOperator::NE}
        };

        const std::unordered_map<std::string, ArithmeticOperators> aritOperators ={
            {"+", ArithmeticOperators::ADD},
            {"-", ArithmeticOperators::SUB},
            {"*", ArithmeticOperators::MUL},
            {"/", ArithmeticOperators::DIV}
        };

        std::string getID();

        std::string getLiteral();

        bool isKeyword(const std::string& value) const;

        bool isAritOperator(char curr) const;

        bool isRelOperator(char curr) const;

        std::string getRelOperator();

};

#endif