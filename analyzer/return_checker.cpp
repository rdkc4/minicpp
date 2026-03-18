#include "return_checker.hpp"

void ReturnChecker::visit([[maybe_unused]] ASTProgram* program){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTIncludeDir* includeDir){
    // intentionally empty
}

void ReturnChecker::visit(ASTFunction* function){
    alwaysReturns = false;

    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->setAlwaysReturns(true);
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] ASTParameter* parameter){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTVariableDeclStmt* variableDecl){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTAssignStmt* assignStmt){
    // intentionally empty
}

void ReturnChecker::visit(ASTCompoundStmt* compoundStmt){
    alwaysReturns = false;
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] ASTForStmt* forStmt){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTFunctionCallStmt* callStmt){
    // intentionally empty
}

void ReturnChecker::visit(ASTIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void ReturnChecker::visit([[maybe_unused]] ASTReturnStmt* returnStmt){
    alwaysReturns = true;
}

void ReturnChecker::visit([[maybe_unused]] ASTWhileStmt* whileStmt){
    // intentionally empty
}

void ReturnChecker::visit(ASTDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void ReturnChecker::visit(ASTSwitchStmt* switchStmt){
    bool switchStmtAlwaysReturns{ true };

    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this); 
        switchStmtAlwaysReturns = switchStmtAlwaysReturns && (alwaysReturns || (!alwaysReturns && !caseStmt->hasBreakStmt()));
    }
    switchStmtAlwaysReturns = switchStmtAlwaysReturns && switchStmt->hasDefaultStmt();
    
    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
    alwaysReturns = switchStmtAlwaysReturns && alwaysReturns;
}

void ReturnChecker::visit(ASTCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(ASTDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void ReturnChecker::visit(ASTSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            return;
        }
    }
}

void ReturnChecker::visit([[maybe_unused]] ASTBinaryExpr* binaryExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTFunctionCallExpr* callExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTIdExpr* idExpr){
    // intentionally empty
}

void ReturnChecker::visit([[maybe_unused]] ASTLiteralExpr* literalExpr){
    // intentionally empty
}