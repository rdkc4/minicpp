#include "../dead_code_eliminator.hpp"

#include <latch>

Optimization::dce::DeadCodeEliminator::DeadCodeEliminator(ThreadPool& threadPool) : threadPool{threadPool} {}

thread_local bool Optimization::dce::DeadCodeEliminator::alwaysReturns{ false };

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRProgram* program){
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& function : program->getFunctions()){
        threadPool.enqueue(
            [this, function=function.get(), &doneLatch] -> void {
                function->accept(*this);
                doneLatch.count_down();
            }
        );
    }

    doneLatch.wait();
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRFunction* function){
    alwaysReturns = false;

    size_t stmtIdx{ 0 };
    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
        if(alwaysReturns){
            function->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRParameter* parameter){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRVariableDeclStmt* variableDecl){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRAssignStmt* assignStmt){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRCompoundStmt* compoundStmt){
    alwaysReturns = false;

    size_t stmtIdx{ 0 };
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            compoundStmt->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRForStmt* forStmt){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRFunctionCallStmt* callStmt){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRReturnStmt* returnStmt){
    alwaysReturns = true;
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRWhileStmt* whileStmt){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRSwitchStmt* switchStmt){
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

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void Optimization::dce::DeadCodeEliminator::visit(IR::node::IRSwitchBlockStmt* switchBlockStmt){
    alwaysReturns = false;

    size_t stmtIdx{ 0 };
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
        if(alwaysReturns){
            switchBlockStmt->eliminateDeadStmts(stmtIdx + 1);
            return;
        }
        ++stmtIdx;
    }
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRBinaryExpr* binaryExpr){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRFunctionCallExpr* callExpr){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRIdExpr* idExpr){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRLiteralExpr* literalExpr){
    // intentionally empty
}

void Optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] IR::node::IRTemporaryExpr* tempExpr){
    // intentionally empty
}
