#include "../stack_frame_analyzer.hpp"

#include <latch>
#include <string>

 Optimization::sfa::StackFrameAnalyzer::StackFrameAnalyzer(ThreadPool& threadPool) : threadPool{threadPool} {}

thread_local size_t  Optimization::sfa::StackFrameAnalyzer::variableCounter{};

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRProgram* program){
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

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRFunction* function){
    variableCounter = 0;
    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
    }

    function->setRequiredMemory(std::to_string(regSize * variableCounter));
}

void  Optimization::sfa::StackFrameAnalyzer::visit([[maybe_unused]] IR::node::IRParameter* parameter){
    // intentionally empty
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRVariableDeclStmt* variableDecl){
    ++variableCounter;
    if(variableDecl->hasTemporaryExpr()){
        variableDecl->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRAssignStmt* assignStmt){
    if(assignStmt->hasTemporaryExpr()){
        assignStmt->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRForStmt* forStmt){
    forStmt->getStmt()->accept(*this);
    if(forStmt->hasTemporaryExpr()){
        forStmt->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRFunctionCallStmt* callStmt){
    callStmt->getFunctionCallExpr()->accept(*this);
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRIfStmt* ifStmt){
    for(const auto& stmt : ifStmt->getStmts()){
        stmt->accept(*this);
    }

    for(const auto& tempExpr : ifStmt->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRReturnStmt* returnStmt){
    if(returnStmt->hasTemporaryExpr()){
        returnStmt->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRWhileStmt* whileStmt){
    whileStmt->getStmt()->accept(*this);
    if(whileStmt->hasTemporaryExpr()){
        whileStmt->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRDoWhileStmt* dowhileStmt){
    dowhileStmt->getStmt()->accept(*this);
    if(dowhileStmt->hasTemporaryExpr()){
        dowhileStmt->getTemporaryExpr()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRSwitchStmt* switchStmt){
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);
    }

    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRSwitchBlockStmt* switchBlockStmt){
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit([[maybe_unused]] IR::node::IRBinaryExpr* binaryExpr){
    // intentionally empty
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRFunctionCallExpr* callExpr){
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void  Optimization::sfa::StackFrameAnalyzer::visit([[maybe_unused]] IR::node::IRIdExpr* idExpr){
    // intentionally empty
}

void  Optimization::sfa::StackFrameAnalyzer::visit([[maybe_unused]] IR::node::IRLiteralExpr* literalExpr){
    // intentionally empty
}

void  Optimization::sfa::StackFrameAnalyzer::visit(IR::node::IRTemporaryExpr* tempExpr){
    for(const auto& expr : tempExpr->getTemporaryExprs()){
        ++variableCounter;
        if(expr->getNodeType() == IR::defs::IRNodeType::CALL){
            expr->accept(*this);
        }
    }
}
