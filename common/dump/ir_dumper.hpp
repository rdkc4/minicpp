#ifndef IR_DUMPER_HPP
#define IR_DUMPER_HPP

#include <ostream>
#include <string_view>

#include "../visitor/ir_visitor.hpp"
#include "../intermediate-representation-tree/ir_node.hpp"
#include "../intermediate-representation-tree/ir_program.hpp"
#include "../intermediate-representation-tree/ir_function.hpp"
#include "../intermediate-representation-tree/ir_parameter.hpp"
#include "../intermediate-representation-tree/ir_variable_decl_stmt.hpp"
#include "../intermediate-representation-tree/ir_assign_stmt.hpp"
#include "../intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../intermediate-representation-tree/ir_for_stmt.hpp"
#include "../intermediate-representation-tree/ir_function_call_stmt.hpp"
#include "../intermediate-representation-tree/ir_if_stmt.hpp"
#include "../intermediate-representation-tree/ir_while_stmt.hpp"
#include "../intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../intermediate-representation-tree/ir_return_stmt.hpp"
#include "../intermediate-representation-tree/ir_switch_stmt.hpp"
#include "../intermediate-representation-tree/ir_case_stmt.hpp"
#include "../intermediate-representation-tree/ir_default_stmt.hpp"
#include "../intermediate-representation-tree/ir_switch_block_stmt.hpp"
#include "../intermediate-representation-tree/ir_binary_expr.hpp"
#include "../intermediate-representation-tree/ir_function_call_expr.hpp"
#include "../intermediate-representation-tree/ir_id_expr.hpp"
#include "../intermediate-representation-tree/ir_literal_expr.hpp"
#include "../intermediate-representation-tree/ir_temporary_expr.hpp"

/**
 * @namespace IR::dump
 * @brief module for the dumping of the ir
*/
namespace IR::dump {
    /**
     * @class IRDumper
     * @brief dumps the structure of the intermediate representation
     * @details inherits IRVisitor
    */
    class IRDumper final : public IR::visitor::IRVisitor {
    public:
        /**
         * @brief creates the instance of the ir dumper
         * @param out - reference to output stream
        */
        IRDumper(std::ostream& out);

        /**
         * @brief deletes the instance of the ir dumper
        */
        ~IRDumper() = default;

        /**
         * @brief dumps the ir program
         * @param program - pointer to the ir program
        */
        void visit(IR::node::IRProgram* program) final override;

        /**
         * @brief dumps the ir function
         * @param function - pointer to the ir function
        */
        void visit(IR::node::IRFunction* function) final override;

        /**
         * @brief dumps the ir parameter
         * @param parameter - pointer to the ir parameter
        */
        void visit(IR::node::IRParameter* parameter) final override;

        /**
         * @brief dumps the ir variable declaration
         * @param variableDecl - pointer to the ir variable declaration
        */
        void visit(IR::node::IRVariableDeclStmt* variableDecl) final override;

        /**
         * @brief dumps the ir assign statement
         * @param assignStmt - pointer to the ir assign statement
        */
        void visit(IR::node::IRAssignStmt* assignStmt) final override;

        /**
         * @brief dumps the ir compound statement
         * @param compoundStmt - pointer to the ir compound statement
        */
        void visit(IR::node::IRCompoundStmt* compoundStmt) final override;

        /**
         * @brief dumps the ir for statement
         * @param forStmt - pointer to the ir for statement
        */
        void visit(IR::node::IRForStmt* forStmt) final override;

        /**
         * @brief dumps the ir function-call statement
         * @param callStmt - pointer to the ir function-call statement
        */
        void visit(IR::node::IRFunctionCallStmt* callStmt) final override;

        /**
         * @brief dumps the ir if statement
         * @param ifStmt - pointer to the ir if statement
        */
        void visit(IR::node::IRIfStmt* ifStmt) final override;

        /**
         * @brief dumps the ir return statement
         * @param returnStmt - pointer to the ir return statement
        */
        void visit(IR::node::IRReturnStmt* returnStmt) final override;

        /**
         * @brief dumps the ir while statement
         * @param whileStmt - pointer to the ir while statement
        */
        void visit(IR::node::IRWhileStmt* whileStmt) final override;

        /**
         * @brief dumps the ir dowhile statement
         * @param dowhileStmt - pointer to the ir dowhile statement
        */
        void visit(IR::node::IRDoWhileStmt* dowhileStmt) final override;

        /**
         * @brief dumps the ir switch statement
         * @param switchStmt - pointer to the ir switch statement
        */
        void visit(IR::node::IRSwitchStmt* switchStmt) final override;

        /**
         * @brief dumps the ir case statement
         * @param caseStmt - pointer to the ir case statement
        */
        void visit(IR::node::IRCaseStmt* caseStmt) final override;

        /**
         * @brief dumps the ir default statement
         * @param defaultStmt - pointer to the ir default statement
        */
        void visit(IR::node::IRDefaultStmt* defaultStmt) final override;

        /**
         * @brief dumps the ir switch block statement
         * @param switchBlockStmt - pointer to the ir switch block statement
        */
        void visit(IR::node::IRSwitchBlockStmt* switchBlockStmt) final override;

        /**
         * @brief dumps the ir binary expression
         * @param binaryExpr - pointer to the ir binary expression
        */
        void visit(IR::node::IRBinaryExpr* binaryExpr) final override;

        /**
         * @brief dumps the ir function-call expression
         * @param callExpr - pointer to the ir function-call expression
        */
        void visit(IR::node::IRFunctionCallExpr* callExpr) final override;

        /**
         * @brief dumps the ir id expression
         * @param idExpr - pointer to the ir id expression
        */
        void visit(IR::node::IRIdExpr* idExpr) final override;

        /**
         * @brief dumps the ir literal expression
         * @param literalExpr - pointer to the ir literal expression
        */
        void visit(IR::node::IRLiteralExpr* literalExpr) final override;
        
        /**
         * @brief dumps the ir temporary expression
         * @param tempExpr - pointer to the ir temporary expression
        */
        void visit(IR::node::IRTemporaryExpr* tempExpr) final override;

    private:
        /// reference to output stream
        std::ostream& out;

        /// size of indentation
        int indent;

        /**
         * @brief dumps the indentation
        */
        void dumpIndent();

        /**
         * @brief dumps the ir node
         * @param node - const pointer to a node
         * @param details - additional details about the node, defaults to empty string
        */
        void dumpNode(const IR::node::IRNode* node, std::string_view details = "");

        /**
         * @brief dumps the label for the node
         * @param nodeLabel - label of the node
        */ 
        void dumpNode(std::string_view nodeLabel);

        /**
         * @brief dumps all included libs
         * @param program - const pointer to a program
        */
        void dumpLibs(const IR::node::IRProgram* program);

    };

}

#endif