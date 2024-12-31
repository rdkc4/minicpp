#ifndef DEFS_HPP
#define DEFS_HPP

#include <iostream>
#include <vector>
#include <unordered_map>

enum class TokenType{_ID, _LITERAL, _AROP, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON, _COLON,
    _ASSIGN, _TYPE, _IF, _ELSE, _WHILE, _DO, _FOR, _SWITCH, _CASE, _DEFAULT, _BREAK, _RETURN, _COMMA, _EOF, _INVALID, _NODE
};

enum class Types{INT, UNSIGNED, VOID, NO_TYPE};

enum class Kinds{NO_KIND, REG, LIT, FUN, VAR, PAR};

enum class ArithmeticOperators{ADD, SUB, MUL, DIV, AROP_NUMBER};

enum class RelationalOperator{LT, GT, LE, GE, EQ, NE, RELOP_NUMBER};

enum class ASTNodeType{PROGRAM, FUNCTION_LIST, FUNCTION, PARAMETER, BODY, VARIABLE_LIST, VARIABLE, STATEMENT_LIST, STATEMENT,
    COMPOUND_STATEMENT, ASSIGNMENT_STATEMENT, RETURN_STATEMENT, IF_STATEMENT, WHILE_STATEMENT, FOR_STATEMENT, DO_WHILE_STATEMENT, 
    SWITCH_STATEMENT, NUMERICAL_EXPRESSION, EXPRESSION, RELATIONAL_EXPRESSION, FUNCTION_CALL, ARGUMENT, LITERAL, ID, CASE, DEFAULT, BREAK
};

enum class IRNodeType{PROGRAM, FUNCTION, PARAMETER, VARIABLE, ARGUMENT, ID, LITERAL, IF, WHILE, FOR, DO_WHILE, SWITCH, CASE, DEFAULT, BREAK,
    ASSIGN, COMPOUND, CALL, RETURN, ADD, SUB, DIV, MUL, CMP
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
    {TokenType::_COLON, "COLON"},
    {TokenType::_ASSIGN, "ASSIGN"},
    {TokenType::_TYPE, "TYPE"},
    {TokenType::_IF, "IF"},
    {TokenType::_ELSE, "ELSE"},
    {TokenType::_WHILE, "WHILE"},
    {TokenType::_FOR, "FOR"},
    {TokenType::_DO, "DO"},
    {TokenType::_SWITCH, "SWITCH"},
    {TokenType::_CASE, "CASE"},
    {TokenType::_DEFAULT, "DEFAULT"},
    {TokenType::_BREAK, "BREAK"},
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
    {ASTNodeType::WHILE_STATEMENT, "WHILE_STATEMENT"},
    {ASTNodeType::FOR_STATEMENT, "FOR_STATEMENT"},
    {ASTNodeType::DO_WHILE_STATEMENT, "DO_WHILE_STATEMENT"},
    {ASTNodeType::SWITCH_STATEMENT, "SWITCH_STATEMENT"},
    {ASTNodeType::CASE, "CASE"},
    {ASTNodeType::DEFAULT, "DEFAULT"},
    {ASTNodeType::BREAK, "BREAK"},
    {ASTNodeType::NUMERICAL_EXPRESSION, "NUMERICAL_EXPRESSION"},
    {ASTNodeType::EXPRESSION, "EXPRESSION"},
    {ASTNodeType::RELATIONAL_EXPRESSION, "RELATIONAL_EXPRESSION"},
    {ASTNodeType::FUNCTION_CALL, "FUNCTION_CALL"},
    {ASTNodeType::ARGUMENT, "ARGUMENT"},
    {ASTNodeType::LITERAL, "LITERAL"},
    {ASTNodeType::ID, "ID"},
    {ASTNodeType::CASE, "CASE"},
    {ASTNodeType::DEFAULT, "DEFAULT"}

};

const std::unordered_map<IRNodeType, std::string> iNodeToString = {
    {IRNodeType::PROGRAM, "PROGRAM"},
    {IRNodeType::FUNCTION, "FUNCTION"},
    {IRNodeType::PARAMETER, "PARAMETER"},
    {IRNodeType::VARIABLE, "VARIABLE"},
    {IRNodeType::ARGUMENT, "ARGUMENT"},
    {IRNodeType::ID, "ID"},
    {IRNodeType::LITERAL, "LITERAL"},
    {IRNodeType::IF, "IF"},
    {IRNodeType::WHILE, "WHILE"},
    {IRNodeType::FOR, "FOR"},
    {IRNodeType::DO_WHILE, "DO_WHILE"},
    {IRNodeType::SWITCH, "SWITCH"},
    {IRNodeType::CASE, "CASE"},
    {IRNodeType::DEFAULT, "DEFAULT"},
    {IRNodeType::BREAK, "BREAK"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::COMPOUND, "COMPOUND"},
    {IRNodeType::CALL, "CALL"},
    {IRNodeType::RETURN, "RETURN"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::ADD, "add"},
    {IRNodeType::SUB, "sub"},
    {IRNodeType::MUL, "mul"},
    {IRNodeType::DIV, "div"},
    {IRNodeType::CMP, "cmp"}
};

const std::unordered_map<std::string, IRNodeType> stringToArop = {
    {"+", IRNodeType::ADD},
    {"-", IRNodeType::SUB},
    {"*", IRNodeType::MUL},
    {"/", IRNodeType::DIV}
};

const std::unordered_map<std::string, std::vector<std::string>> stringToJMP = {
    {">", {"jg", "ja"}},
    {">=", {"jge", "jae"}},
    {"<", {"jl", "jb"}},
    {"<=", {"jle", "jbe"}},
    {"==", {"je", "je"}},
    {"!=", {"jne", "jne"}}
};

const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP = {
    {">", {"jle", "jbe"}},
    {">=", {"jl", "jb"}},
    {"<", {"jge", "jae"}},
    {"<=", {"jg", "ja"}},
    {"==", {"jne", "jne"}},
    {"!=", {"je", "je"}}
};

#endif
