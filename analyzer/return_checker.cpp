#include "return_checker.hpp"

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTProgram* program){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTIncludeDir* includeDir){
    // intentionally empty
}

void ReturnChecker::visit(syntax::ast::ASTFunction* function){
    alwaysReturns = false;

    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->setAlwaysReturns(true);
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTParameter* parameter){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTVariableDeclStmt* variableDecl){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTAssignStmt* assignStmt){
    // intentionally empty
}

void ReturnChecker::visit(syntax::ast::ASTCompoundStmt* compoundStmt){
    alwaysReturns = false;
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTForStmt* forStmt){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTFunctionCallStmt* callStmt){
    // intentionally empty
}

void ReturnChecker::visit(syntax::ast::ASTIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTReturnStmt* returnStmt){
    alwaysReturns = true;
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTWhileStmt* whileStmt){
    // intentionally empty
}

void ReturnChecker::visit(syntax::ast::ASTDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void ReturnChecker::visit(syntax::ast::ASTSwitchStmt* switchStmt){
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

void ReturnChecker::visit(syntax::ast::ASTCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(syntax::ast::ASTDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(syntax::ast::ASTSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTBinaryExpr* binaryExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTFunctionCallExpr* callExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTIdExpr* idExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTLiteralExpr* literalExpr){
    // intentionally empty
}