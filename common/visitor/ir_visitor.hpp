#ifndef IR_VISITOR_HPP
#define IR_VISITOR_HPP

namespace IR::node {
    class IRProgram;
    class IRFunction;
    class IRParameter;
    class IRVariableDeclStmt;
    class IRAssignStmt; 
    class IRCompoundStmt;
    class IRForStmt;
    class IRFunctionCallStmt;
    class IRIfStmt;
    class IRReturnStmt;
    class IRWhileStmt;
    class IRDoWhileStmt;
    class IRSwitchStmt;
    class IRCaseStmt;
    class IRDefaultStmt;
    class IRSwitchBlockStmt;
    class IRBinaryExpr;
    class IRFunctionCallExpr;
    class IRIdExpr;
    class IRLiteralExpr;
    class IRTemporaryExpr;
}

/**
 * @namespace IR::visitor
 * @brief module defining the visitor of the ir
*/
namespace IR::visitor {
    /**
     * @class IRVisitor
     * @brief defines the interface for classes that need irt traversal
    */
    class IRVisitor {
    public:
        /**
         * @brief default constructor
        */
        IRVisitor() = default;

        /**
         * @brief default destructor
        */
        virtual ~IRVisitor() = default;

        /**
         * @brief abstract method for visiting program
         * @param program - pointer to a program
        */
        virtual void visit(IR::node::IRProgram* program) = 0;

        /**
         * @brief abstract method for visiting function
         * @param function - pointer to a function
        */
        virtual void visit(IR::node::IRFunction* function) = 0;

        /**
         * @brief abstract method for visiting parameter
         * @param parameter - pointer to a parameter
        */
        virtual void visit(IR::node::IRParameter* parameter) = 0;

        /**
         * @brief abstract method for visiting variable declaration
         * @param variableDecl - pointer to a variable declaration
        */
        virtual void visit(IR::node::IRVariableDeclStmt* variableDecl) = 0;

        /**
         * @brief abstract method for visiting assign statement
         * @param assignStmt - pointer to a assign statement
        */
        virtual void visit(IR::node::IRAssignStmt* assignStmt) = 0;

        /**
         * @brief abstract method for visiting compound statement
         * @param compoundStmt - pointer to a compound statement
        */
        virtual void visit(IR::node::IRCompoundStmt* compoundStmt) = 0;

        /**
         * @brief abstract method for visiting for statement
         * @param forStmt - pointer to a for statement
        */
        virtual void visit(IR::node::IRForStmt* forStmt) = 0;

        /**
         * @brief abstract method for visiting function call statement
         * @param callStmt - pointer to a function call statement
        */
        virtual void visit(IR::node::IRFunctionCallStmt* callStmt) = 0;

        /**
         * @brief abstract method for visiting if statement
         * @param ifStmt - pointer to a if statement
        */
        virtual void visit(IR::node::IRIfStmt* ifStmt) = 0;

        /**
         * @brief abstract method for visiting return statement
         * @param returnStmt - pointer to a return statement
        */
        virtual void visit(IR::node::IRReturnStmt* returnStmt) = 0;

        /**
         * @brief abstract method for visiting while statement
         * @param whileStmt - pointer to a while statement
        */
        virtual void visit(IR::node::IRWhileStmt* whileStmt) = 0;

        /**
         * @brief abstract method for visiting dowhile statement
         * @param dowhileStmt - pointer to a dowhile statement
        */
        virtual void visit(IR::node::IRDoWhileStmt* dowhileStmt) = 0;

        /**
         * @brief abstract method for visiting switch statement
         * @param switchStmt - pointer to a switch statement
        */
        virtual void visit(IR::node::IRSwitchStmt* switchStmt) = 0;

        /**
         * @brief abstract method for visiting case statement
         * @param caseStmt - pointer to a case statement
        */
        virtual void visit(IR::node::IRCaseStmt* caseStmt) = 0;

        /**
         * @brief abstract method for visiting default statement
         * @param defaultStmt - pointer to a default statement
        */
        virtual void visit(IR::node::IRDefaultStmt* defaultStmt) = 0;

        /**
         * @brief abstract method for visiting switch block statement
         * @param switchBlockStmt - pointer to a switch block statement
        */
        virtual void visit(IR::node::IRSwitchBlockStmt* switchBlockStmt) = 0;

        /**
         * @brief abstract method for visiting binary expression
         * @param binaryExpr - pointer to a binary expression
        */
        virtual void visit(IR::node::IRBinaryExpr* binaryExpr) = 0;

        /**
         * @brief abstract method for visiting function call expression
         * @param callExpr - pointer to a function call expression
        */
        virtual void visit(IR::node::IRFunctionCallExpr* callExpr) = 0;

        /**
         * @brief abstract method for visiting id expression
         * @param idExpr - pointer to a id expression
        */
        virtual void visit(IR::node::IRIdExpr* idExpr) = 0;

        /**
         * @brief abstract method for visiting literal expression
         * @param literalExpr - pointer to a literal expression
        */
        virtual void visit(IR::node::IRLiteralExpr* literalExpr) = 0;
        
        /**
         * @brief abstract method for visiting temporary expression
         * @param tempExpr - pointer to a temporary expression
        */
        virtual void visit(IR::node::IRTemporaryExpr* tempExpr) = 0;

    };

}

#endif