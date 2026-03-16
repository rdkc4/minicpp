#include "../stack_frame_analyzer.hpp"

#include <latch>
#include <string>

StackFrameAnalyzer::StackFrameAnalyzer(ThreadPool& threadPool) : threadPool{threadPool} {}

thread_local size_t StackFrameAnalyzer::variableCounter{};

void StackFrameAnalyzer::visit(IRProgram* program){
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& function : program->getFunctions()){
        threadPool.enqueue([this, function=function.get(), &doneLatch]{
            function->accept(*this);
            doneLatch.count_down();
        });
    }

    doneLatch.wait();
}

void StackFrameAnalyzer::visit(IRFunction* function){
    variableCounter = 0;
    for(const auto& stmt : function->getBody()){
        stmt->accept(*this);
    }

    function->setRequiredMemory(std::to_string(regSize * variableCounter));
}

void StackFrameAnalyzer::visit([[maybe_unused]] IRParameter* parameter){
    // intentionally empty
}

void StackFrameAnalyzer::visit(IRVariableDeclStmt* variableDecl){
    ++variableCounter;
    if(variableDecl->hasTemporaryExpr()){
        variableDecl->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRAssignStmt* assignStmt){
    if(assignStmt->hasTemporaryExpr()){
        assignStmt->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRForStmt* forStmt){
    forStmt->getStmt()->accept(*this);
    if(forStmt->hasTemporaryExpr()){
        forStmt->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRFunctionCallStmt* callStmt){
    callStmt->getFunctionCallExpr()->accept(*this);
}

void StackFrameAnalyzer::visit(IRIfStmt* ifStmt){
    for(const auto& stmt : ifStmt->getStmts()){
        stmt->accept(*this);
    }

    for(const auto& tempExpr : ifStmt->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void StackFrameAnalyzer::visit(IRReturnStmt* returnStmt){
    if(returnStmt->hasTemporaryExpr()){
        returnStmt->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRWhileStmt* whileStmt){
    whileStmt->getStmt()->accept(*this);
    if(whileStmt->hasTemporaryExpr()){
        whileStmt->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRDoWhileStmt* dowhileStmt){
    dowhileStmt->getStmt()->accept(*this);
    if(dowhileStmt->hasTemporaryExpr()){
        dowhileStmt->getTemporaryExpr()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRSwitchStmt* switchStmt){
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);
    }

    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
}

void StackFrameAnalyzer::visit(IRCaseStmt* caseStmt){
    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void StackFrameAnalyzer::visit(IRDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void StackFrameAnalyzer::visit(IRSwitchBlockStmt* switchBlockStmt){
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void StackFrameAnalyzer::visit([[maybe_unused]] IRBinaryExpr* binaryExpr){
    // intentionally empty
}

void StackFrameAnalyzer::visit(IRFunctionCallExpr* callExpr){
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr){
            tempExpr->accept(*this);
        }
    }
}

void StackFrameAnalyzer::visit([[maybe_unused]] IRIdExpr* idExpr){
    // intentionally empty
}

void StackFrameAnalyzer::visit([[maybe_unused]] IRLiteralExpr* literalExpr){
    // intentionally empty
}

void StackFrameAnalyzer::visit(IRTemporaryExpr* tempExpr){
    for(const auto& expr : tempExpr->getTemporaryExprs()){
        ++variableCounter;
        if(expr->getNodeType() == IRNodeType::CALL){
            expr->accept(*this);
        }
    }
}
