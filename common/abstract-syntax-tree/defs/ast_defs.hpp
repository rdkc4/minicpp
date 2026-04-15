#ifndef AST_DEFS_HPP
#define AST_DEFS_HPP

#include <array>
#include <string_view>

/**
 * @namespace AST::defs
 * @brief module defining the definitions of abstract syntax tree
*/
namespace AST::defs {
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

    /// number of ast node types
    constexpr size_t AST_NODE_TYPE_COUNT{ static_cast<size_t>(ASTNodeType::ID_EXPR) + 1 };

    /// maps ast node types to their string representations
    constexpr std::array<std::string_view, AST_NODE_TYPE_COUNT> astNodeTypeStringRepresentations {
        [] {
            /**
             * @brief converts ast node type enum to array index
             * @param type - element of the ast node type enum
             * @returns index in the array for provided node type
            */
            constexpr auto idx {
                [](ASTNodeType type) noexcept -> size_t {
                    return static_cast<size_t>(type);
                }
            };
            std::array<std::string_view, AST_NODE_TYPE_COUNT> nodes{};

            nodes[idx(ASTNodeType::PROGRAM)] = "PROGRAM";
            nodes[idx(ASTNodeType::FUNCTION)] = "FUNCTION";
            nodes[idx(ASTNodeType::PARAMETER)] = "PARAMETER";

            nodes[idx(ASTNodeType::DIR)] = "DIR";
            nodes[idx(ASTNodeType::INCLUDE_DIR)] = "INCLUDE_DIR";

            nodes[idx(ASTNodeType::STMT)] = "STMT";
            nodes[idx(ASTNodeType::VARIABLE_DECL_STMT)] = "VARIABLE_DECL_STMT";
            nodes[idx(ASTNodeType::COMPOUND_STMT)] = "COMPOUND_STMT";
            nodes[idx(ASTNodeType::ASSIGN_STMT)] = "ASSIGN_STMT";
            nodes[idx(ASTNodeType::RETURN_STMT)] = "RETURN_STMT";
            nodes[idx(ASTNodeType::FUNCTION_CALL_STMT)] = "FUNCTION_CALL_STMT";
            nodes[idx(ASTNodeType::IF_STMT)] = "IF_STMT";
            nodes[idx(ASTNodeType::WHILE_STMT)] = "WHILE_STMT";
            nodes[idx(ASTNodeType::FOR_STMT)] = "FOR_STMT";
            nodes[idx(ASTNodeType::DOWHILE_STMT)] = "DOWHILE_STMT";
            nodes[idx(ASTNodeType::SWITCH_STMT)] = "SWITCH_STMT";
            nodes[idx(ASTNodeType::CASE_STMT)] = "CASE_STMT";
            nodes[idx(ASTNodeType::DEFAULT_STMT)] = "DEFAULT_STMT";
            nodes[idx(ASTNodeType::SWITCH_BLOCK_STMT)] = "SWITCH_BLOCK_STMT";
            nodes[idx(ASTNodeType::BREAK_STMT)] = "BREAK_STMT";

            nodes[idx(ASTNodeType::EXPR)] = "EXPR";
            nodes[idx(ASTNodeType::BINARY_EXPR)] = "BINARY_EXPR";
            nodes[idx(ASTNodeType::FUNCTION_CALL_EXPR)] = "FUNCTION_CALL_EXPR";
            nodes[idx(ASTNodeType::LITERAL_EXPR)] = "LITERAL_EXPR";
            nodes[idx(ASTNodeType::ID_EXPR)] = "ID_EXPR";

            return nodes;
        }()
    };

    /**
     * @brief getter for the string representation of the string node
     * @param type - type of the ast node
     * @returns string representation of the ast node
    */
    constexpr std::string_view astNodeTypeToStr(ASTNodeType type){
        return astNodeTypeStringRepresentations[static_cast<size_t>(type)];
    }

    static_assert(astNodeTypeStringRepresentations.size() == AST_NODE_TYPE_COUNT);

}

#endif