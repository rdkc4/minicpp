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

class ASTVisitor {
public:
    ASTVisitor() = default;
    
    virtual ~ASTVisitor() = default;

    virtual void visit(ASTProgram* program) = 0;

    virtual void visit(ASTIncludeDir* lib) = 0;

    virtual void visit(ASTFunction* function) = 0;

    virtual void visit(ASTParameter* parameter) = 0;

    virtual void visit(ASTVariableDeclStmt* variableDecl) = 0;

    virtual void visit(ASTAssignStmt* assignStmt) = 0;

    virtual void visit(ASTCompoundStmt* compoundStmt) = 0;

    virtual void visit(ASTForStmt* forStmt) = 0;

    virtual void visit(ASTFunctionCallStmt* callStmt) = 0;

    virtual void visit(ASTIfStmt* ifStmt) = 0;

    virtual void visit(ASTReturnStmt* returnStmt) = 0;

    virtual void visit(ASTWhileStmt* whileStmt) = 0;

    virtual void visit(ASTDoWhileStmt* dowhileStmt) = 0;
    
    virtual void visit(ASTSwitchStmt* switchStmt) = 0;

    virtual void visit(ASTCaseStmt* caseStmt) = 0;

    virtual void visit(ASTDefaultStmt* defaultStmt) = 0;
    
    virtual void visit(ASTSwitchBlockStmt* switchBlockStmt) = 0;

    virtual void visit(ASTBinaryExpr* binaryExpr) = 0;

    virtual void visit(ASTFunctionCallExpr* callExpr) = 0;

    virtual void visit(ASTIdExpr* idExpr) = 0;

    virtual void visit(ASTLiteralExpr* literalExpr) = 0;

};

#endif