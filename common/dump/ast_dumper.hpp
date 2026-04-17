#ifndef AST_DUMPER_HPP
#define AST_DUMPER_HPP

#include <ostream>
#include <string_view>

#include "../visitor/ast_visitor.hpp"
#include "../abstract-syntax-tree/ast_node.hpp"
#include "../abstract-syntax-tree/ast_program.hpp"
#include "../abstract-syntax-tree/ast_include_dir.hpp"
#include "../abstract-syntax-tree/ast_function.hpp"
#include "../abstract-syntax-tree/ast_parameter.hpp"
#include "../abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../abstract-syntax-tree/ast_for_stmt.hpp"
#include "../abstract-syntax-tree/ast_function_call_stmt.hpp"
#include "../abstract-syntax-tree/ast_if_stmt.hpp"
#include "../abstract-syntax-tree/ast_return_stmt.hpp"
#include "../abstract-syntax-tree/ast_while_stmt.hpp"
#include "../abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../abstract-syntax-tree/ast_case_stmt.hpp"
#include "../abstract-syntax-tree/ast_default_stmt.hpp"
#include "../abstract-syntax-tree/ast_switch_block_stmt.hpp"
#include "../abstract-syntax-tree/ast_binary_expr.hpp"
#include "../abstract-syntax-tree/ast_function_call_expr.hpp"
#include "../abstract-syntax-tree/ast_id_expr.hpp"
#include "../abstract-syntax-tree/ast_literal_expr.hpp"

namespace syntax::ast {
    /**
    * @class ASTDumper
    * @brief dumps the structure of the abstract syntax tree
    * @details inherits ASTVisitor
    */
    class ASTDumper final : public ASTVisitor {
    public:
        /**
        * @brief creates the ast dumper instance
        * @param out - reference to an output stream
        */
        ASTDumper(std::ostream& out);
        
        /**
        * @brief deletes the instance of the ast dumper
        */
        ~ASTDumper() = default;

        /**
        * @brief dumps the ast program
        * @param program - pointer to the ast program
        */
        void visit(ASTProgram* program) final override;

        /**
        * @brief dumps the ast include directive
        * @param lib - pointer to the ast include directive
        */
        void visit(ASTIncludeDir* lib) final override;

        /**
        * @brief dumps the ast function
        * @param function - pointer to the ast function
        */
        void visit(ASTFunction* function) final override;

        /**
        * @brief dumps the ast parameter
        * @param parameter - pointer to the ast parameter
        */
        void visit(ASTParameter* parameter) final override;

        /**
        * @brief dumps the ast variable declaration
        * @param variableDecl - pointer to the ast variable declaration
        */
        void visit(ASTVariableDeclStmt* variableDecl) final override;

        /**
        * @brief dumps the ast assign statement
        * @param assignStmt - pointer to the ast assign statement
        */
        void visit(ASTAssignStmt* assignStmt) final override;

        /**
        * @brief dumps the ast compound statement
        * @param compoundStmt - pointer to the ast compound statement
        */
        void visit(ASTCompoundStmt* compoundStmt) final override;

        /**
        * @brief dumps the ast for statement
        * @param forStmt - pointer to the ast for statement
        */
        void visit(ASTForStmt* forStmt) final override;

        /**
        * @brief dumps the ast function-call statement
        * @param callStmt - pointer to the ast function-call statement
        */
        void visit(ASTFunctionCallStmt* callStmt) final override;

        /**
        * @brief dumps the ast if statement
        * @param ifStmt - pointer to the ast if statement
        */
        void visit(ASTIfStmt* ifStmt) final override;

        /**
        * @brief dumps the ast return statement
        * @param returnStmt - pointer to the ast return statement
        */
        void visit(ASTReturnStmt* returnStmt) final override;

        /**
        * @brief dumps the ast while statement
        * @param whileStmt - pointer to the ast while statement
        */
        void visit(ASTWhileStmt* whileStmt) final override;

        /**
        * @brief dumps the ast dowhile statement
        * @param dowhileStmt - pointer to the ast dowhile statement
        */
        void visit(ASTDoWhileStmt* dowhileStmt) final override;
        
        /**
        * @brief dumps the ast switch statement
        * @param switchStmt - pointer to the ast switch statement
        */
        void visit(ASTSwitchStmt* switchStmt) final override;

        /**
        * @brief dumps the ast case statement
        * @param caseStmt - pointer to the ast case statement
        */
        void visit(ASTCaseStmt* caseStmt) final override;

        /**
        * @brief dumps the ast default statement
        * @param defaultStmt - pointer to the ast default statement
        */
        void visit(ASTDefaultStmt* defaultStmt) final override;
        
        /**
        * @brief dumps the ast switch block statement
        * @param switchBlockStmt - pointer to the ast switch block statement
        */
        void visit(ASTSwitchBlockStmt* switchBlockStmt) final override;

        /**
        * @brief dumps the ast binary expression
        * @param binaryExpr - pointer to the ast binary expression
        */
        void visit(ASTBinaryExpr* binaryExpr) final override;

        /**
        * @brief dumps the ast function-call expression
        * @param callExpr - pointer to the ast function-call expression
        */
        void visit(ASTFunctionCallExpr* callExpr) final override;

        /**
        * @brief dumps the ast id expression
        * @param idExpr - pointer to the ast id expression
        */
        void visit(ASTIdExpr* idExpr) final override;

        /**
        * @brief dumps the ast literal expression
        * @param literalExpr - pointer to the ast literal expression
        */
        void visit(ASTLiteralExpr* literalExpr) final override;

    private:
        /// output stream
        std::ostream& out;

        /// size of indentation
        int indent;

        /**
        * @brief dumps the indentation
        */
        void dumpIndent();

        /**
        * @brief dumps the ast node
        * @param node - const pointer to a node
        */
        void dumpNode(const ASTNode* node);

        /**
        * @brief dumps the label for the node
        * @param nodeLabel - label of the node
        */ 
        void dumpNode(std::string_view nodeLabel);

    };

}

#endif