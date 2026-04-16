#include "return_checker.hpp"

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTProgram* program){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTIncludeDir* includeDir){
    // intentionally empty
}

void ReturnChecker::visit(AST::node::ASTFunction* function){
    alwaysReturns = false;

    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->setAlwaysReturns(true);
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTParameter* parameter){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTVariableDeclStmt* variableDecl){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTAssignStmt* assignStmt){
    // intentionally empty
}

void ReturnChecker::visit(AST::node::ASTCompoundStmt* compoundStmt){
    alwaysReturns = false;
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTForStmt* forStmt){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTFunctionCallStmt* callStmt){
    // intentionally empty
}

void ReturnChecker::visit(AST::node::ASTIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTReturnStmt* returnStmt){
    alwaysReturns = true;
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTWhileStmt* whileStmt){
    // intentionally empty
}

void ReturnChecker::visit(AST::node::ASTDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void ReturnChecker::visit(AST::node::ASTSwitchStmt* switchStmt){
    bool switchStmtAlwaysReturns{ true };

    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this); 
        switchStmtAlwaysReturns = switchStmtAlwaysReturns && 
            (alwaysReturns || (!alwaysReturns && !caseStmt->hasBreakStmt()));
    }
    switchStmtAlwaysReturns = switchStmtAlwaysReturns && switchStmt->hasDefaultStmt();
    
    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
    alwaysReturns = switchStmtAlwaysReturns && alwaysReturns;
}

void ReturnChecker::visit(AST::node::ASTCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(AST::node::ASTDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(AST::node::ASTSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTBinaryExpr* binaryExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTFunctionCallExpr* callExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTIdExpr* idExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] AST::node::ASTLiteralExpr* literalExpr){
    // intentionally empty
}