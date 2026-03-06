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

    virtual void visit(ASTIncludeDir* statement) = 0;

    virtual void visit(ASTFunction* function) = 0;

    virtual void visit(ASTParameter* statement) = 0;

    virtual void visit(ASTVariableDeclStmt* statement) = 0;

    virtual void visit(ASTAssignStmt* statement) = 0;

    virtual void visit(ASTCompoundStmt* statement) = 0;

    virtual void visit(ASTForStmt* statement) = 0;

    virtual void visit(ASTFunctionCallStmt* statement) = 0;

    virtual void visit(ASTIfStmt* statement) = 0;

    virtual void visit(ASTReturnStmt* statement) = 0;

    virtual void visit(ASTWhileStmt* statement) = 0;

    virtual void visit(ASTDoWhileStmt* statement) = 0;
    
    virtual void visit(ASTSwitchStmt* statement) = 0;

    virtual void visit(ASTCaseStmt* statement) = 0;

    virtual void visit(ASTDefaultStmt* statement) = 0;
    
    virtual void visit(ASTSwitchBlockStmt* statement) = 0;

    virtual void visit(ASTBinaryExpr* statement) = 0;

    virtual void visit(ASTFunctionCallExpr* statement) = 0;

    virtual void visit(ASTIdExpr* statement) = 0;

    virtual void visit(ASTLiteralExpr* statement) = 0;


};

#endif