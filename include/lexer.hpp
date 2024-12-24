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

        std::vector<Token> tokens;
        size_t _nextToken = 0;

        const std::unordered_map<std::string, TokenType> keywords = {
            {"return", TokenType::_RETURN},
            {"if", TokenType::_IF},
            {"else", TokenType::_ELSE},
            {"int", TokenType::_TYPE},
            {"unsigned", TokenType::_TYPE},
            {"void", TokenType::_TYPE}
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

        const std::unordered_map<TokenType, std::string> tokenTypeToString = {
            {TokenType::_ID, "ID"},
            {TokenType::_LITERAL, "LITERAL"},
            {TokenType::_AROP, "AROP"},
            {TokenType::_RELOP, "RELOP"},
            {TokenType::_LPAREN, "LPAREN"},
            {TokenType::_RPAREN, "RPAREN"},
            {TokenType::_LBRACKET, "LBRACKET"},
            {TokenType::_RBRACKET, "RBRACKET"},
            {TokenType::_SEMICOLON, "SEMICOLON"},
            {TokenType::_ASSIGN, "ASSIGN"},
            {TokenType::_TYPE, "TYPE"},
            {TokenType::_IF, "IF"},
            {TokenType::_ELSE, "ELSE"},
            {TokenType::_RETURN, "RETURN"},
            {TokenType::_COMMA, "COMMA"},
            {TokenType::_EOF, "EOF"},
            {TokenType::_INVALID, "INVALID"}
        };

        std::string getID();

        std::string getLiteral();

        bool isKeyword(const std::string& value) const;

        bool isAritOperator(char curr) const;

        bool isRelOperator(char curr) const;

        std::string getRelOperator();

};

#endif