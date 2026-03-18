#ifndef AST_DEFS_HPP
#define AST_DEFS_HPP

#include <unordered_map>
#include <string>

/** 
 * @enum ASTNodeType
 * @brief types of the ast nodes
*/
enum class ASTNodeType{
    PROGRAM, 
    FUNCTION, 
    PARAMETER, 
    DIR, 
    INCLUDE_DIR,
    STMT, 
    VARIABLE_DECL_STMT, 
    COMPOUND_STMT, 
    SWITCH_BLOCK_STMT, 
    ASSIGN_STMT, 
    RETURN_STMT, 
    IF_STMT, 
    WHILE_STMT, 
    FOR_STMT, 
    DOWHILE_STMT, 
    SWITCH_STMT,
    CASE_STMT, 
    DEFAULT_STMT,
    BREAK_STMT, 
    FUNCTION_CALL_STMT,
    EXPR, 
    BINARY_EXPR, 
    FUNCTION_CALL_EXPR,
    LITERAL_EXPR, 
    ID_EXPR
};

/// maps ast node type to a string
extern const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString;

#endif
