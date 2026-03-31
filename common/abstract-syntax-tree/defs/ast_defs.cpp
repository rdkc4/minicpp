#include "ast_defs.hpp"

const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString {
    /// program structure
    {ASTNodeType::PROGRAM, "PROGRAM"},
    {ASTNodeType::FUNCTION, "FUNCTION"},
    {ASTNodeType::PARAMETER, "PARAMETER"},

    /// preprocessor / directive
    {ASTNodeType::DIR, "DIR"},
    {ASTNodeType::INCLUDE_DIR, "INCLUDE_DIR"},

    /// statements
    {ASTNodeType::STMT, "STMT"},
    {ASTNodeType::VARIABLE_DECL_STMT, "VARIABLE_DECL_STMT"},
    {ASTNodeType::COMPOUND_STMT, "COMPOUND_STMT"},
    {ASTNodeType::ASSIGN_STMT, "ASSIGN_STMT"},

    /// control flow statements
    {ASTNodeType::IF_STMT, "IF_STMT"},
    {ASTNodeType::WHILE_STMT, "WHILE_STMT"},
    {ASTNodeType::FOR_STMT, "FOR_STMT"},
    {ASTNodeType::DOWHILE_STMT, "DO_WHILE_STMT"},
    {ASTNodeType::FUNCTION_CALL_STMT, "FUNCTION_CALL_STMT"},
    {ASTNodeType::RETURN_STMT, "RETURN_STMT"},
    {ASTNodeType::SWITCH_STMT, "SWITCH_STMT"},
    {ASTNodeType::CASE_STMT, "CASE_STMT"},
    {ASTNodeType::DEFAULT_STMT, "DEFAULT_STMT"},
    {ASTNodeType::SWITCH_BLOCK_STMT, "SWITCH_BLOCK_STMT"},
    {ASTNodeType::BREAK_STMT, "BREAK"},

    /// expressions
    {ASTNodeType::EXPR, "EXPR"},
    {ASTNodeType::ID_EXPR, "ID_EXPR"},
    {ASTNodeType::LITERAL_EXPR, "LITERAL_EXPR"},
    {ASTNodeType::BINARY_EXPR, "BINARY_EXPR"},
    {ASTNodeType::FUNCTION_CALL_EXPR, "FUNCTION_CALL_EXPR"}
};