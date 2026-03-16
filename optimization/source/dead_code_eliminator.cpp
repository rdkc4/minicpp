#include "../dead_code_eliminator.hpp"

#include <latch>

DeadCodeEliminator::DeadCodeEliminator(ThreadPool& threadPool) : threadPool{threadPool} {}

thread_local bool DeadCodeEliminator::alwaysReturns{ false };

void DeadCodeEliminator::visit(IRProgram* program){
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& function : program->getFunctions()){
        threadPool.enqueue([this, function=function.get(), &doneLatch]{
            function->accept(*this);
            doneLatch.count_down();
        });
    }

    doneLatch.wait();
}

void DeadCodeEliminator::visit(IRFunction* function){
    alwaysReturns = false;

    size_t stmtIdx = 0;
    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void DeadCodeEliminator::visit([[maybe_unused]] IRParameter* parameter){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRVariableDeclStmt* variableDecl){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRAssignStmt* assignStmt){
    // intentionally empty
}

void DeadCodeEliminator::visit(IRCompoundStmt* compoundStmt){
    alwaysReturns = false;

    size_t stmtIdx = 0;
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            compoundStmt->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void DeadCodeEliminator::visit([[maybe_unused]] IRForStmt* forStmt){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRFunctionCallStmt* callStmt){
    // intentionally empty
}

void DeadCodeEliminator::visit(IRIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void DeadCodeEliminator::visit([[maybe_unused]] IRReturnStmt* returnStmt){
    alwaysReturns = true;
}

void DeadCodeEliminator::visit([[maybe_unused]] IRWhileStmt* whileStmt){
    // intentionally empty
}

void DeadCodeEliminator::visit(IRDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void DeadCodeEliminator::visit(IRSwitchStmt* switchStmt){
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

void DeadCodeEliminator::visit(IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void DeadCodeEliminator::visit(IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void DeadCodeEliminator::visit(IRSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;

    size_t stmtIdx = 0;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            switchBlockStmt->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void DeadCodeEliminator::visit([[maybe_unused]] IRBinaryExpr* binaryExpr){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRFunctionCallExpr* callExpr){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRIdExpr* idExpr){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRLiteralExpr* literalExpr){
    // intentionally empty
}

void DeadCodeEliminator::visit([[maybe_unused]] IRTemporaryExpr* tempExpr){
    // intentionally empty
}
