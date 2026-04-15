#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

namespace AST::node {
    class ASTProgram;
    class ASTIncludeDir;
    class ASTFunction;
    class ASTParameter;
    class ASTVariableDeclStmt;
    class ASTAssignStmt; 
    class ASTCompoundStmt;
    class ASTForStmt;
    class ASTFunctionCallStmt;
    class ASTIfStmt;
    class ASTReturnStmt;
    class ASTWhileStmt;
    class ASTDoWhileStmt;
    class ASTSwitchStmt;
    class ASTCaseStmt;
    class ASTDefaultStmt;
    class ASTSwitchBlockStmt;
    class ASTBinaryExpr;
    class ASTFunctionCallExpr;
    class ASTIdExpr;
    class ASTLiteralExpr;
}

/**
 * @namespace AST::visitor
 * @brief module defining the visitor of the abstract syntax tree
*/
namespace AST::visitor {
    /**
     * @class ASTVisitor
     * @brief defines the interface for classes that need ast traversal
    */
    class ASTVisitor {
    public:
        /**
         * @brief default constructor
        */
        ASTVisitor() = default;
        
        /**
         * @brief default destructor
        */
        virtual ~ASTVisitor() = default;

        /**
         * @brief abstract method for visiting program
         * @param program - pointer to a program
        */
        virtual void visit(AST::node::ASTProgram* program) = 0;

        /**
         * @brief abstract method for visiting include directive
         * @param lib - pointer to a include directive
        */
        virtual void visit(AST::node::ASTIncludeDir* lib) = 0;

        /**
         * @brief abstract method for visiting function
         * @param function - pointer to a function
        */
        virtual void visit(AST::node::ASTFunction* function) = 0;

        /**
         * @brief abstract method for visiting parameter
         * @param parameter - pointer to a parameter
        */
        virtual void visit(AST::node::ASTParameter* parameter) = 0;

        /**
         * @brief abstract method for visiting variable declaration
         * @param variableDecl - pointer to a variable declaration
        */
        virtual void visit(AST::node::ASTVariableDeclStmt* variableDecl) = 0;

        /**
         * @brief abstract method for visiting assign statement
         * @param assignStmt - pointer to a assign statement
        */
        virtual void visit(AST::node::ASTAssignStmt* assignStmt) = 0;

        /**
         * @brief abstract method for visiting compound statement
         * @param compoundStmt - pointer to a compound statement
        */
        virtual void visit(AST::node::ASTCompoundStmt* compoundStmt) = 0;

        /**
         * @brief abstract method for visiting for statement
         * @param forStmt - pointer to a for statement
        */
        virtual void visit(AST::node::ASTForStmt* forStmt) = 0;

        /**
         * @brief abstract method for visiting function call statement
         * @param callStmt - pointer to a function call statement
        */
        virtual void visit(AST::node::ASTFunctionCallStmt* callStmt) = 0;

        /**
         * @brief abstract method for visiting if statement
         * @param ifStmt - pointer to a if statement
        */
        virtual void visit(AST::node::ASTIfStmt* ifStmt) = 0;

        /**
         * @brief abstract method for visiting return statement
         * @param returnStmt - pointer to a return statement
        */
        virtual void visit(AST::node::ASTReturnStmt* returnStmt) = 0;

        /**
         * @brief abstract method for visiting while statement
         * @param whileStmt - pointer to a while statement
        */
        virtual void visit(AST::node::ASTWhileStmt* whileStmt) = 0;

        /**
         * @brief abstract method for visiting dowhile statement
         * @param dowhileStmt - pointer to a dowhile statement
        */
        virtual void visit(AST::node::ASTDoWhileStmt* dowhileStmt) = 0;
        
        /**
         * @brief abstract method for visiting switch statement
         * @param switchStmt - pointer to a switch statement
        */
        virtual void visit(AST::node::ASTSwitchStmt* switchStmt) = 0;

        /**
         * @brief abstract method for visiting case statement
         * @param caseStmt - pointer to a case statement
        */
        virtual void visit(AST::node::ASTCaseStmt* caseStmt) = 0;

        /**
         * @brief abstract method for visiting default statement
         * @param defaultStmt - pointer to a default statement
        */
        virtual void visit(AST::node::ASTDefaultStmt* defaultStmt) = 0;
        
        /**
         * @brief abstract method for visiting switch block statement
         * @param switchBlockStmt - pointer to a switch block statement
        */
        virtual void visit(AST::node::ASTSwitchBlockStmt* switchBlockStmt) = 0;

        /**
         * @brief abstract method for visiting binary expression
         * @param binaryExpr - pointer to a binary expression
        */
        virtual void visit(AST::node::ASTBinaryExpr* binaryExpr) = 0;

        /**
         * @brief abstract method for visiting function call expression
         * @param callExpr - pointer to a function call expression
        */
        virtual void visit(AST::node::ASTFunctionCallExpr* callExpr) = 0;

        /**
         * @brief abstract method for visiting id expression
         * @param idExpr - pointer to a id expression
        */
        virtual void visit(AST::node::ASTIdExpr* idExpr) = 0;

        /**
         * @brief abstract method for visiting literal expression
         * @param literalExpr - pointer to a literal expression
        */
        virtual void visit(AST::node::ASTLiteralExpr* literalExpr) = 0;

    };

}

#endif