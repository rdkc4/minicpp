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

class IRVisitor {
public:
    IRVisitor() = default;

    virtual ~IRVisitor() = default;

    virtual void visit(IRProgram* program) = 0;

    virtual void visit(IRFunction* function) = 0;

    virtual void visit(IRParameter* parameter) = 0;

    virtual void visit(IRVariableDeclStmt* variableDecl) = 0;

    virtual void visit(IRAssignStmt* assignStmt) = 0;

    virtual void visit(IRCompoundStmt* compoundStmt) = 0;

    virtual void visit(IRForStmt* forStmt) = 0;

    virtual void visit(IRFunctionCallStmt* callStmt) = 0;

    virtual void visit(IRIfStmt* ifStmt) = 0;

    virtual void visit(IRReturnStmt* returnStmt) = 0;

    virtual void visit(IRWhileStmt* whileStmt) = 0;

    virtual void visit(IRDoWhileStmt* dowhileStmt) = 0;

    virtual void visit(IRSwitchStmt* switchStmt) = 0;

    virtual void visit(IRCaseStmt* caseStmt) = 0;

    virtual void visit(IRDefaultStmt* defaultStmt) = 0;

    virtual void visit(IRSwitchBlockStmt* switchBlockStmt) = 0;

    virtual void visit(IRBinaryExpr* binaryExpr) = 0;

    virtual void visit(IRFunctionCallExpr* callExpr) = 0;

    virtual void visit(IRIdExpr* idExpr) = 0;

    virtual void visit(IRLiteralExpr* literalExpr) = 0;
    
    virtual void visit(IRTemporaryExpr* tempExpr) = 0;

};

#endif