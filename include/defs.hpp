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
    RELATIONAL_EXPRESSION, FUNCTION_CALL, ARGUMENT, LITERAL, ID
};

struct Token{
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token() : type(TokenType::_NODE), value(""), line(0), column(0){}
    Token(std::string value, size_t line, size_t column) : type(TokenType::_NODE), value(value), line(line), column(column){}
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

const std::unordered_map<ASTNodeType, std::string> nodeTypeToString = {
    {ASTNodeType::PROGRAM, "PROGRAM"},
    {ASTNodeType::FUNCTION_LIST, "FUNCTION_LIST"},
    {ASTNodeType::FUNCTION, "FUNCTION"},
    {ASTNodeType::PARAMETER, "PARAMETER"},
    {ASTNodeType::BODY, "BODY"},
    {ASTNodeType::VARIABLE_LIST, "VARIABLE_LIST"},
    {ASTNodeType::VARIABLE, "VARIABLE"},
    {ASTNodeType::STATEMENT_LIST, "STATEMENT_LIST"},
    {ASTNodeType::STATEMENT, "STATEMENT"},
    {ASTNodeType::COMPOUND_STATEMENT, "COMPOUND_STATEMENT"},
    {ASTNodeType::ASSIGNMENT_STATEMENT, "ASSIGNMENT_STATEMENT"},
    {ASTNodeType::RETURN_STATEMENT, "RETURN_STATEMENT"},
    {ASTNodeType::IF_STATEMENT, "IF_STATEMENT"},
    {ASTNodeType::NUMERICAL_EXPRESSION, "NUMERICAL_EXPRESSION"},
    {ASTNodeType::EXPRESSION, "EXPRESSION"},
    {ASTNodeType::RELATIONAL_EXPRESSION, "RELATIONAL_EXPRESSION"},
    {ASTNodeType::FUNCTION_CALL, "FUNCTION_CALL"},
    {ASTNodeType::ARGUMENT, "ARGUMENT"},
    {ASTNodeType::LITERAL, "LITERAL"},
    {ASTNodeType::ID, "ID"}

};

#endif
