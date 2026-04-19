#include "return_checker.hpp"

void semantic::ReturnChecker::visit(syntax::ast::ASTFunction* function){
    alwaysReturns = false;

    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->setAlwaysReturns(true);
            return;
        }
    }
}

void semantic::ReturnChecker::visit(syntax::ast::ASTCompoundStmt* compoundStmt){
    alwaysReturns = false;
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void semantic::ReturnChecker::visit(syntax::ast::ASTIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void semantic::ReturnChecker::visit([[maybe_unused]] syntax::ast::ASTReturnStmt* returnStmt){
    alwaysReturns = true;
}

void semantic::ReturnChecker::visit(syntax::ast::ASTDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void semantic::ReturnChecker::visit(syntax::ast::ASTSwitchStmt* switchStmt){
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

void semantic::ReturnChecker::visit(syntax::ast::ASTCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void semantic::ReturnChecker::visit(syntax::ast::ASTDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void semantic::ReturnChecker::visit(syntax::ast::ASTSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}