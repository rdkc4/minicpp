#ifndef ASTREE_DEFS_HPP
#define ASTREE_DEFS_HPP

#include <unordered_map>
#include <string>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// definitions related to abstract syntax tree
//-----------------------------------------------------------------------------------------------------------------------------------------------------

enum class ASTNodeType{
    PROGRAM, FUNCTION_LIST, FUNCTION, PARAMETER, BODY, VARIABLE_LIST, VARIABLE, STATEMENT_LIST, STATEMENT, COMPOUND_STATEMENT, 
    ASSIGNMENT_STATEMENT, RETURN_STATEMENT, IF_STATEMENT, WHILE_STATEMENT, FOR_STATEMENT, DO_WHILE_STATEMENT, SWITCH_STATEMENT, 
    NUMERICAL_EXPRESSION, EXPRESSION, RELATIONAL_EXPRESSION, FUNCTION_CALL, ARGUMENT, LITERAL, ID, CASE, DEFAULT, BREAK
};

extern const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString;

#endif
