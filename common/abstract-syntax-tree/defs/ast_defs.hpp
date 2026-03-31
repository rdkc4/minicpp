#ifndef AST_DEFS_HPP
#define AST_DEFS_HPP

#include <unordered_map>
#include <string>

/** 
 * @enum ASTNodeType
 * @brief types of the ast nodes
*/
enum class ASTNodeType {
    /// program structure
    PROGRAM,                //< root node of the ast 
    FUNCTION,               //< function definition
    PARAMETER,              //< function parameter

    /// preprocessor / directives
    DIR,                    //< generic directive
    INCLUDE_DIR,            //< #include directive
    
    /// statements
    STMT,                   //< generic statement
    VARIABLE_DECL_STMT,     //< variable declaration
    COMPOUND_STMT,          //< block { ... }
    ASSIGN_STMT,            //< assignment statement
    RETURN_STMT,            //< return statement
    FUNCTION_CALL_STMT,     //< function call as statement

    /// control flow statements
    IF_STMT, 
    WHILE_STMT, 
    FOR_STMT, 
    DOWHILE_STMT, 
    SWITCH_STMT,
    CASE_STMT, 
    DEFAULT_STMT,
    SWITCH_BLOCK_STMT, 
    BREAK_STMT, 

    /// expressions
    EXPR,                   //< generic expression
    BINARY_EXPR,            //< binary expression
    FUNCTION_CALL_EXPR,     //< function call returning a value
    LITERAL_EXPR,           //< literal value
    ID_EXPR                 //< identifier reference
};

/// maps ast node type to a string
extern const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString;

#endif
