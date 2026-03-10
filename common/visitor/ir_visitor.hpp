#ifndef IR_VISITOR_HPP
#define IR_VISITOR_HPP

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
    virtual void visit(IRProgram* program) = 0;

    /**
     * @brief abstract method for visiting function
     * @param function - pointer to a function
    */
    virtual void visit(IRFunction* function) = 0;

    /**
     * @brief abstract method for visiting parameter
     * @param parameter - pointer to a parameter
    */
    virtual void visit(IRParameter* parameter) = 0;

    /**
     * @brief abstract method for visiting variable declaration
     * @param variableDecl - pointer to a variable declaration
    */
    virtual void visit(IRVariableDeclStmt* variableDecl) = 0;

    /**
     * @brief abstract method for visiting assign statement
     * @param assignStmt - pointer to a assign statement
    */
    virtual void visit(IRAssignStmt* assignStmt) = 0;

    /**
     * @brief abstract method for visiting compound statement
     * @param compoundStmt - pointer to a compound statement
    */
    virtual void visit(IRCompoundStmt* compoundStmt) = 0;

    /**
     * @brief abstract method for visiting for statement
     * @param forStmt - pointer to a for statement
    */
    virtual void visit(IRForStmt* forStmt) = 0;

    /**
     * @brief abstract method for visiting function call statement
     * @param callStmt - pointer to a function call statement
    */
    virtual void visit(IRFunctionCallStmt* callStmt) = 0;

    /**
     * @brief abstract method for visiting if statement
     * @param ifStmt - pointer to a if statement
    */
    virtual void visit(IRIfStmt* ifStmt) = 0;

    /**
     * @brief abstract method for visiting return statement
     * @param returnStmt - pointer to a return statement
    */
    virtual void visit(IRReturnStmt* returnStmt) = 0;

    /**
     * @brief abstract method for visiting while statement
     * @param whileStmt - pointer to a while statement
    */
    virtual void visit(IRWhileStmt* whileStmt) = 0;

    /**
     * @brief abstract method for visiting dowhile statement
     * @param dowhileStmt - pointer to a dowhile statement
    */
    virtual void visit(IRDoWhileStmt* dowhileStmt) = 0;

    /**
     * @brief abstract method for visiting switch statement
     * @param switchStmt - pointer to a switch statement
    */
    virtual void visit(IRSwitchStmt* switchStmt) = 0;

    /**
     * @brief abstract method for visiting case statement
     * @param caseStmt - pointer to a case statement
    */
    virtual void visit(IRCaseStmt* caseStmt) = 0;

    /**
     * @brief abstract method for visiting default statement
     * @param defaultStmt - pointer to a default statement
    */
    virtual void visit(IRDefaultStmt* defaultStmt) = 0;

    /**
     * @brief abstract method for visiting switch block statement
     * @param switchBlockStmt - pointer to a switch block statement
    */
    virtual void visit(IRSwitchBlockStmt* switchBlockStmt) = 0;

    /**
     * @brief abstract method for visiting binary expression
     * @param binaryExpr - pointer to a binary expression
    */
    virtual void visit(IRBinaryExpr* binaryExpr) = 0;

    /**
     * @brief abstract method for visiting function call expression
     * @param callExpr - pointer to a function call expression
    */
    virtual void visit(IRFunctionCallExpr* callExpr) = 0;

    /**
     * @brief abstract method for visiting id expression
     * @param idExpr - pointer to a id expression
    */
    virtual void visit(IRIdExpr* idExpr) = 0;

    /**
     * @brief abstract method for visiting literal expression
     * @param literalExpr - pointer to a literal expression
    */
    virtual void visit(IRLiteralExpr* literalExpr) = 0;
    
    /**
     * @brief abstract method for visiting temporary expression
     * @param tempExpr - pointer to a temporary expression
    */
    virtual void visit(IRTemporaryExpr* tempExpr) = 0;

};

#endif