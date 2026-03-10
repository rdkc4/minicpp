#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

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
    virtual void visit(ASTProgram* program) = 0;

    /**
     * @brief abstract method for visiting include directive
     * @param lib - pointer to a include directive
    */
    virtual void visit(ASTIncludeDir* lib) = 0;

    /**
     * @brief abstract method for visiting function
     * @param function - pointer to a function
    */
    virtual void visit(ASTFunction* function) = 0;

    /**
     * @brief abstract method for visiting parameter
     * @param parameter - pointer to a parameter
    */
    virtual void visit(ASTParameter* parameter) = 0;

    /**
     * @brief abstract method for visiting variable declaration
     * @param variableDecl - pointer to a variable declaration
    */
    virtual void visit(ASTVariableDeclStmt* variableDecl) = 0;

    /**
     * @brief abstract method for visiting assign statement
     * @param assignStmt - pointer to a assign statement
    */
    virtual void visit(ASTAssignStmt* assignStmt) = 0;

    /**
     * @brief abstract method for visiting compound statement
     * @param compoundStmt - pointer to a compound statement
    */
    virtual void visit(ASTCompoundStmt* compoundStmt) = 0;

    /**
     * @brief abstract method for visiting for statement
     * @param forStmt - pointer to a for statement
    */
    virtual void visit(ASTForStmt* forStmt) = 0;

    /**
     * @brief abstract method for visiting function call statement
     * @param callStmt - pointer to a function call statement
    */
    virtual void visit(ASTFunctionCallStmt* callStmt) = 0;

    /**
     * @brief abstract method for visiting if statement
     * @param ifStmt - pointer to a if statement
    */
    virtual void visit(ASTIfStmt* ifStmt) = 0;

    /**
     * @brief abstract method for visiting return statement
     * @param returnStmt - pointer to a return statement
    */
    virtual void visit(ASTReturnStmt* returnStmt) = 0;

    /**
     * @brief abstract method for visiting while statement
     * @param whileStmt - pointer to a while statement
    */
    virtual void visit(ASTWhileStmt* whileStmt) = 0;

    /**
     * @brief abstract method for visiting dowhile statement
     * @param dowhileStmt - pointer to a dowhile statement
    */
    virtual void visit(ASTDoWhileStmt* dowhileStmt) = 0;
    
    /**
     * @brief abstract method for visiting switch statement
     * @param switchStmt - pointer to a switch statement
    */
    virtual void visit(ASTSwitchStmt* switchStmt) = 0;

    /**
     * @brief abstract method for visiting case statement
     * @param caseStmt - pointer to a case statement
    */
    virtual void visit(ASTCaseStmt* caseStmt) = 0;

    /**
     * @brief abstract method for visiting default statement
     * @param defaultStmt - pointer to a default statement
    */
    virtual void visit(ASTDefaultStmt* defaultStmt) = 0;
    
    /**
     * @brief abstract method for visiting switch block statement
     * @param switchBlockStmt - pointer to a switch block statement
    */
    virtual void visit(ASTSwitchBlockStmt* switchBlockStmt) = 0;

    /**
     * @brief abstract method for visiting binary expression
     * @param binaryExpr - pointer to a binary expression
    */
    virtual void visit(ASTBinaryExpr* binaryExpr) = 0;

    /**
     * @brief abstract method for visiting function call expression
     * @param callExpr - pointer to a function call expression
    */
    virtual void visit(ASTFunctionCallExpr* callExpr) = 0;

    /**
     * @brief abstract method for visiting id expression
     * @param idExpr - pointer to a id expression
    */
    virtual void visit(ASTIdExpr* idExpr) = 0;

    /**
     * @brief abstract method for visiting literal expression
     * @param literalExpr - pointer to a literal expression
    */
    virtual void visit(ASTLiteralExpr* literalExpr) = 0;

};

#endif