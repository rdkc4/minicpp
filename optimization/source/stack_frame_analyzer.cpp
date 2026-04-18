#include "../stack_frame_analyzer.hpp"

#include <latch>
#include <string>

optimization::sfa::StackFrameAnalyzer::StackFrameAnalyzer(util::concurrency::ThreadPool& threadPool) 
    : threadPool{threadPool} {}

thread_local size_t optimization::sfa::StackFrameAnalyzer::variableCounter{};

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRProgram* program){
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

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRFunction* function){
    variableCounter = 0;
    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
    }

    function->setRequiredMemory(std::to_string(regSize * variableCounter));
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRVariableDeclStmt* variableDecl){
    ++variableCounter;
    if(variableDecl->hasTemporaryExpr()){
        variableDecl->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRAssignStmt* assignStmt){
    if(assignStmt->hasTemporaryExpr()){
        assignStmt->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRForStmt* forStmt){
    forStmt->getStmt()->accept(*this);
    if(forStmt->hasTemporaryExpr()){
        forStmt->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRFunctionCallStmt* callStmt){
    callStmt->getFunctionCallExpr()->accept(*this);
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRIfStmt* ifStmt){
    for(const auto& stmt : ifStmt->getStmts()){
        stmt->accept(*this);
    }

    for(const auto& tempExpr : ifStmt->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRReturnStmt* returnStmt){
    if(returnStmt->hasTemporaryExpr()){
        returnStmt->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRWhileStmt* whileStmt){
    whileStmt->getStmt()->accept(*this);
    if(whileStmt->hasTemporaryExpr()){
        whileStmt->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRDoWhileStmt* dowhileStmt){
    dowhileStmt->getStmt()->accept(*this);
    if(dowhileStmt->hasTemporaryExpr()){
        dowhileStmt->getTemporaryExpr()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRSwitchStmt* switchStmt){
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);
    }

    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRSwitchBlockStmt* switchBlockStmt){
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRFunctionCallExpr* callExpr){
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void optimization::sfa::StackFrameAnalyzer::visit(ir::IRTemporaryExpr* tempExpr){
    for(const auto& expr : tempExpr->getTemporaryExprs()){
        ++variableCounter;
        if(expr->getNodeType() == ir::IRNodeType::CALL){
            expr->accept(*this);
        }
    }
}
