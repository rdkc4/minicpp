#ifndef DEFS_HPP
#define DEFS_HPP

#include <iostream>
#include <unordered_map>

enum class TokenType{_ID, _LITERAL, _AROP, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON,
    _ASSIGN, _TYPE, _IF, _ELSE, _RETURN, _COMMA, _EOF, _INVALID, _NODE
};

enum class Types{INT, UNSIGNED, VOID, NO_TYPE};

enum class Kinds{NO_KIND = 0x1, REG = 0x2, LIT = 0x4, FUN = 0x8, VAR = 0x10, PAR = 0x20};

enum class ArithmeticOperators{ADD, SUB, MUL, DIV, AROP_NUMBER};

enum class RelationalOperator{LT, GT, LE, GE, EQ, NE, RELOP_NUMBER};

enum class ASTNodeType{PROGRAM, FUNCTION_LIST, FUNCTION, PARAMETER, BODY, VARIABLE_LIST, VARIABLE, STATEMENT_LIST, STATEMENT,
    COMPOUND_STATEMENT, ASSIGNMENT_STATEMENT, RETURN_STATEMENT, IF_STATEMENT, NUMERICAL_EXPRESSION, EXPRESSION,
    RELATIONAL_EXPRESSION, FUNCTION_CALL, ARGUMENT
};

struct Token{
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token() : type(TokenType::_NODE), value(""), line(0), column(0){}
    Token(TokenType type, const std::string& value, size_t line, size_t column): type(type), value(value), line(line), column(column){}
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

const std::unordered_map<Types, std::string> typeToString = {
    {Types::INT, "int"},
    {Types::UNSIGNED, "unsigned"},
    {Types::VOID, "void"},
    {Types::NO_TYPE, "no_type"}
};

const std::unordered_map<Kinds, std::string> kindToString = {
    {Kinds::NO_KIND, "NO_KIND"},
    {Kinds::REG, "REG"},
    {Kinds::LIT, "LIT"},
    {Kinds::FUN, "FUN"},
    {Kinds::VAR, "VAR"},
    {Kinds::PAR, "PAR"}
};

#endif
