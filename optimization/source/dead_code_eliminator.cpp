#include "../dead_code_eliminator.hpp"

#include <latch>

optimization::dce::DeadCodeEliminator::DeadCodeEliminator(ThreadPool& threadPool) 
    : threadPool{threadPool} {}

thread_local bool optimization::dce::DeadCodeEliminator::alwaysReturns{ false };

void optimization::dce::DeadCodeEliminator::visit(ir::IRProgram* program){
    std::latch doneLatch{ 
        static_cast<std::ptrdiff_t>(program->getFunctionCount()) 
    };

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

void optimization::dce::DeadCodeEliminator::visit(ir::IRFunction* function){
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

void optimization::dce::DeadCodeEliminator::visit(ir::IRCompoundStmt* compoundStmt){
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

void optimization::dce::DeadCodeEliminator::visit(ir::IRIfStmt* ifStmt){
    bool ifStmtAlwaysReturns{ true };

    for(const auto& stmt : ifStmt->getStmts()){
        alwaysReturns = false;
        stmt->accept(*this);
        ifStmtAlwaysReturns = ifStmtAlwaysReturns && alwaysReturns;
    }
    alwaysReturns = ifStmtAlwaysReturns && ifStmt->hasElseStmt();
}

void optimization::dce::DeadCodeEliminator::visit([[maybe_unused]] ir::IRReturnStmt* returnStmt){
    alwaysReturns = true;
}

void optimization::dce::DeadCodeEliminator::visit(ir::IRDoWhileStmt* dowhileStmt){
    alwaysReturns = false;
    dowhileStmt->getStmt()->accept(*this);
}

void optimization::dce::DeadCodeEliminator::visit(ir::IRSwitchStmt* switchStmt){
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

void optimization::dce::DeadCodeEliminator::visit(ir::IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void optimization::dce::DeadCodeEliminator::visit(ir::IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void optimization::dce::DeadCodeEliminator::visit(ir::IRSwitchBlockStmt* switchBlockStmt){
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
